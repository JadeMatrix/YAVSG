#include "../../include/engine/scene.hpp"

#include <tiny_obj_loader.h>

#include <algorithm>    // std::max()
#include <iostream>     // std::cerr for tinyobj warnings


namespace yavsg
{
    scene::scene() :
        main_camera(
            { 2.2f, 0.0f, 2.2f },
            0.1f,
            1.0f,
            10.0f
        )
    {}
    
    void scene::insert_model(
        const std::string& obj_filename,
        const std::string& obj_mtl_directory
    )
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
            throw std::runtime_error(
                "unable to load OBJ file: " + tinyobj_error
            );
        else if( !tinyobj_error.empty() )
            std::cerr << "tinyobj warning: " << tinyobj_error << std::endl;
        
        // Load textures
        std::vector< material_description > materials;
        for( auto& material : obj_materials )
        {
            material_description description;
            
            struct texture_info
            {
                material_texture_type& texture;
                const std::string& filename;
                gl::texture_flags_type flags;
            };
            
            material_texture_type    color_map;
            material_texture_type   normal_map;
            material_texture_type specular_map;
            
            for( auto& info : {
                texture_info{    color_map, material.diffuse_texname , gl::texture_flag::NONE         },
                texture_info{   normal_map, material.bump_texname    , gl::texture_flag::LINEAR_INPUT },
                texture_info{ specular_map, material.specular_texname, gl::texture_flag::LINEAR_INPUT }
            } )
            {
                if( info.filename.size() )
                {
                    std::string texture_filename =
                        obj_mtl_directory + info.filename;
                    
                    using settings = gl::texture_filter_settings;
                    
                    info.texture = material_texture_type::from_file(
                        texture_filename,
                        {
                            settings::magnify_mode::LINEAR,
                            settings::minify_mode::LINEAR,
                            settings::mipmap_type::LINEAR,
                        },
                        info.flags
                    );
                }
            }
            
            materials.push_back( material_description{
                   color_map,
                  normal_map,
                specular_map
            } );
        }
        
        // Create vertices
        std::vector< vertex_type > vertices;
        std::vector< std::vector< GLuint > > indices( materials.size() );
        GLfloat obj_max_x = 0.0f;
        GLfloat obj_min_x = 0.0f;
        GLfloat obj_max_y = 0.0f;
        GLfloat obj_min_y = 0.0f;
        GLfloat obj_max_z = 0.0f;
        GLfloat obj_min_z = 0.0f;
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
                    throw std::runtime_error(
                        "vertex count = "
                        + std::to_string( vertex_count )
                    );
                
                auto& group_indices = indices[
                    obj_shape.mesh.material_ids[ face ]
                ];
                
                for( size_t i = 0; i < vertex_count; ++i )
                {
                    auto index = obj_shape.mesh.indices[ index_offset + i ];
                    
                    auto vx = obj_attributes.vertices[ 3 * index.vertex_index + 0 ];
                    auto vy = obj_attributes.vertices[ 3 * index.vertex_index + 1 ];
                    auto vz = obj_attributes.vertices[ 3 * index.vertex_index + 2 ];
                    
                    auto nx = obj_attributes.normals[ 3 * index.normal_index + 0 ];
                    auto ny = obj_attributes.normals[ 3 * index.normal_index + 1 ];
                    auto nz = obj_attributes.normals[ 3 * index.normal_index + 2 ];
                    
                    // Colors use index.vertex_index
                    auto cr = obj_attributes.colors[ 3 * index.vertex_index + 0 ];
                    auto cb = obj_attributes.colors[ 3 * index.vertex_index + 1 ];
                    auto cg = obj_attributes.colors[ 3 * index.vertex_index + 2 ];
                    
                    auto tu = obj_attributes.texcoords[ 2 * index.texcoord_index + 0 ];
                    auto tv = obj_attributes.texcoords[ 2 * index.texcoord_index + 1 ];
                    
                    vertices.push_back( {
                        { vx, vz, vy }, // { vx, vy, vz },
                        { nx, nz, ny },
                        { cr, cb, cg },
                        { tu, tv }
                    } );
                    group_indices.push_back( vertices.size() - 1 );
                    
                    if( vx > obj_max_x )
                        obj_max_x = vx;
                    if( vx < obj_min_x )
                        obj_min_x = vx;
                    
                    if( vy > obj_max_y )
                        obj_max_y = vy;
                    if( vy < obj_min_y )
                        obj_min_y = vy;
                    
                    if( vz > obj_max_z )
                        obj_max_z = vz;
                    if( vz < obj_min_z )
                        obj_min_z = vz;
                }
                
                index_offset += vertex_count;
            }
        }
        
        // Move OBJ data over to manager
        {
            using group_type = render_object_manager_type::render_group;
            
            auto objects_ref = object_manager.write();
            
            GLfloat width  = obj_max_x - obj_min_x;
            GLfloat length = obj_max_y - obj_min_y;
            GLfloat height = obj_max_z - obj_min_z;
            GLfloat scale  = 13.0f / std::max( { width, length, height } );
            
            objects_ref -> emplace_back(
                std::vector< group_type >{},
                vertices,
                vector< GLfloat, 3 >{ 0.0f, 0.0f, 0.0f },
                vector< GLfloat, 3 >{ scale, scale, scale },
                versor< GLfloat >::from_euler(
                    radians< GLfloat >( 0.0f ),
                    vector< GLfloat, 3 >{ 0.0f, 0.0f, 1.0f }
                )
            );
            
            auto& groups = objects_ref -> back().render_groups;
            
            for( std::size_t i = 0; i < materials.size(); ++i )
                groups.emplace_back(
                    std::move( materials[ i ] ),
                    indices[ i ]
                );
        }
    }
}
