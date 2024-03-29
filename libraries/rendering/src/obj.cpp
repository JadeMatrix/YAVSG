#include <yavsg/rendering/obj.hpp>

#include <yavsg/logging.hpp>
#include <yavsg/math/quaternion.hpp>
#include <yavsg/math/vector.hpp>

#include <doctest/doctest.h>    // REQUIRE
#include <fmt/format.h>
#include <tiny_obj_loader.h>

#include <algorithm>    // max
#include <exception>
#include <limits>
#include <string_view>
#include <tuple>        // tie
#include <vector>


namespace
{
    using namespace std::string_view_literals;
    
    auto const log_ = JadeMatrix::yavsg::log_handle();
}


JadeMatrix::yavsg::task_flags_type
JadeMatrix::yavsg::load_obj_task::flags() const
{
    return ( upload_mode ? task_flag::gpu_thread : task_flag::none );
}

bool JadeMatrix::yavsg::load_obj_task::operator()()
{
    if( !upload_mode )
    {
        tinyobj::attrib_t obj_attributes;
        std::vector< tinyobj::shape_t    > obj_shapes;
        std::vector< tinyobj::material_t > obj_materials;
        
        std::string tinyobj_error;
        if( !tinyobj::LoadObj(
            &obj_attributes,
            &obj_shapes,
            &obj_materials,
            &tinyobj_error,
            obj_filename.c_str(),
            obj_mtl_directory.c_str()
        ) )
        {
            throw std::runtime_error( fmt::format(
                "unable to load OBJ file: {}"sv,
                tinyobj_error
            ) );
        }
        else if( !tinyobj_error.empty() )
        {
            log_.split_on_newlines_as(
                ext::log::level::warning,
                tinyobj_error,
                "tinyobj: {}"sv
            );
        }
        
        // Load textures
        for( auto& material : obj_materials )
        {
            scene::material_description description;
            
            scene::material_texture_type color;
            scene::material_texture_type normal;
            scene::material_texture_type specular;
            
            for( auto& [ texture, filename, flags ] : {
                std::tie( color   , material. diffuse_texname, gl::texture_flag::none         ),
                std::tie( normal  , material.    bump_texname, gl::texture_flag::linear_input ),
                std::tie( specular, material.specular_texname, gl::texture_flag::linear_input ),
            } )
            {
                if( filename.empty() ) continue;
                
                auto const texture_filename = obj_mtl_directory / filename;
                
                using settings = gl::texture_filter_settings;
                
                texture = scene::material_texture_type::from_file(
                    texture_filename,
                    {
                        settings::magnify_mode::linear,
                        settings::minify_mode::linear,
                        settings::mipmap_type::linear,
                    },
                    flags
                );
            }
            
            materials.push_back( scene::material_description{
                std::move( color    ),
                std::move( normal   ),
                std::move( specular )
            } );
        }
        
        // Create vertices
        indices.resize( materials.size(), {} );
        obj_max_x = 0.0f;
        obj_min_x = 0.0f;
        obj_max_y = 0.0f;
        obj_min_y = 0.0f;
        obj_max_z = 0.0f;
        obj_min_z = 0.0f;
        for( auto& obj_shape : obj_shapes )
        {
            std::size_t index_offset = 0;
            
            for(
                std::size_t face = 0;
                face < obj_shape.mesh.num_face_vertices.size();
                ++face
            )
            {
                auto vertex_count = obj_shape.mesh.num_face_vertices[ face ];
                
                // TODO: Suport other vertex counts
                if( vertex_count != 3 )
                {
                    throw std::runtime_error( fmt::format(
                        "vertex count = {}"sv,
                        vertex_count
                    ) );
                }
                
                REQUIRE( obj_shape.mesh.material_ids[ face ] >= 0 );
                REQUIRE( static_cast< decltype( indices )::size_type >(
                    obj_shape.mesh.material_ids[ face ]
                ) <= indices.size() );
                auto& group_indices = indices[
                    static_cast< decltype( indices )::size_type >(
                        obj_shape.mesh.material_ids[ face ]
                    )
                ];
                
                for( size_t i = 0; i < vertex_count; ++i )
                {
                    auto index = obj_shape.mesh.indices[ index_offset + i ];
                    
                    auto vx = obj_attributes.vertices[ static_cast< std::size_t >( 3 * index.vertex_index + 0 ) ];
                    auto vy = obj_attributes.vertices[ static_cast< std::size_t >( 3 * index.vertex_index + 1 ) ];
                    auto vz = obj_attributes.vertices[ static_cast< std::size_t >( 3 * index.vertex_index + 2 ) ];
                    
                    auto nx = obj_attributes.normals[ static_cast< std::size_t >( 3 * index.normal_index + 0 ) ];
                    auto ny = obj_attributes.normals[ static_cast< std::size_t >( 3 * index.normal_index + 1 ) ];
                    auto nz = obj_attributes.normals[ static_cast< std::size_t >( 3 * index.normal_index + 2 ) ];
                    
                    // Colors use index.vertex_index
                    auto cr = obj_attributes.colors[ static_cast< std::size_t >( 3 * index.vertex_index + 0 ) ];
                    auto cb = obj_attributes.colors[ static_cast< std::size_t >( 3 * index.vertex_index + 1 ) ];
                    auto cg = obj_attributes.colors[ static_cast< std::size_t >( 3 * index.vertex_index + 2 ) ];
                    
                    auto tu = obj_attributes.texcoords[ static_cast< std::size_t >( 2 * index.texcoord_index + 0 ) ];
                    auto tv = obj_attributes.texcoords[ static_cast< std::size_t >( 2 * index.texcoord_index + 1 ) ];
                    
                    vertices.push_back( {
                        { vx, vz, vy }, // { vx, vy, vz },
                        { nx, nz, ny },
                        { cr, cb, cg },
                        { tu, tv }
                    } );
                    REQUIRE(
                        vertices.size() - 1
                        <= std::numeric_limits< GLuint >::max()
                    );
                    group_indices.push_back(
                        static_cast< GLuint >( vertices.size() - 1 )
                    );
                    
                    if( vx > obj_max_x ) obj_max_x = vx;
                    if( vx < obj_min_x ) obj_min_x = vx;
                    
                    if( vy > obj_max_y ) obj_max_y = vy;
                    if( vy < obj_min_y ) obj_min_y = vy;
                    
                    if( vz > obj_max_z ) obj_max_z = vz;
                    if( vz < obj_min_z ) obj_min_z = vz;
                }
                
                index_offset += vertex_count;
            }
        }
        
        upload_mode = true;
        
        return true;
    }
    else // Move OBJ data over to manager
    {
        using group_type = scene::render_object_manager_type::render_group;
        
        auto objects_ref = object_manager.write();
        
        GLfloat width  = obj_max_x - obj_min_x;
        GLfloat length = obj_max_y - obj_min_y;
        GLfloat height = obj_max_z - obj_min_z;
        GLfloat scale  = 13.0f / std::max( { width, length, height } );
        
        objects_ref->emplace_back(
            std::vector< group_type >{},
            vertices,
            vector< GLfloat, 3 >{ 0.0f, 0.0f, 0.0f },
            vector< GLfloat, 3 >{ scale, scale, scale },
            versor< GLfloat >::from_euler(
                radians< GLfloat >( 0.0f ),
                vector< GLfloat, 3 >{ 0.0f, 0.0f, 1.0f }
            )
        );
        
        auto& groups = objects_ref->back().render_groups;
        
        for( std::size_t i = 0; i < materials.size(); ++i )
        {
            groups.emplace_back(
                std::move( materials[ i ] ),
                indices[ i ]
            );
        }
        
        return false;
    }
}
