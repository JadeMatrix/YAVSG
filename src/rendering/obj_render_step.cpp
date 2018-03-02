#include "../../include/rendering/obj_render_step.hpp"

#include "../../include/math/common_transforms.hpp"
#include "../../include/gl/shader.hpp"
#include "../../include/gl/texture.hpp"
#include "../../include/rendering/gl_tut.hpp"   // gl_tut::window_width & gl_tut::window_height

#include <tiny_obj_loader.h>

#include <algorithm>    // std::max()
#include <exception>
#include <iostream>     // std::cerr for tinyobj warnings


namespace yavsg
{
    obj_render_step::obj_render_step(
        const std::string& obj_filename,
        const std::string& obj_mtl_directory
    ) :
        model_scale_transform(
            square_matrix< GLfloat, 4 >::make_uninitialized()
        ),
        scene_program( {
            gl::shader::from_file(
                GL_VERTEX_SHADER,
                "../src/shaders/obj_scene.vert"
            ).id,
            gl::shader::from_file(
                GL_FRAGMENT_SHADER,
                "../src/shaders/obj_scene.frag"
            ).id
        } )
    {
        start_time = std::chrono::high_resolution_clock::now();
        previous_time = start_time;
        
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
        
        ////////////////////////////////////////////////////////////////////////
        
        GLfloat obj_max_x = 0.0f;
        GLfloat obj_min_x = 0.0f;
        GLfloat obj_max_y = 0.0f;
        GLfloat obj_min_y = 0.0f;
        GLfloat obj_max_z = 0.0f;
        GLfloat obj_min_z = 0.0f;
        
        // Load textures
        std::vector< material_description > materials;
        for( auto& material : obj_materials )
        {
            material_description description;
            
            struct texture_info
            {
                gl::texture*& texture;
                const std::string& filename;
            };
            
            gl::texture*    color_map = nullptr;
            gl::texture*   normal_map = nullptr;
            gl::texture* specular_map = nullptr;
            
            for( auto& info : {
                texture_info{    color_map, material.diffuse_texname  },
                texture_info{   normal_map, material.bump_texname     },
                texture_info{ specular_map, material.specular_texname }
            } )
            {
                if( info.filename.size() )
                {
                    std::string texture_filename =
                        obj_mtl_directory + info.filename;
                    
                    info.texture = new gl::texture(
                        gl::texture::from_file( texture_filename )
                    );
                    info.texture -> filtering( {
                        gl::texture::filter_settings::magnify_mode::LINEAR,
                        gl::texture::filter_settings::minify_mode::LINEAR,
                        gl::texture::filter_settings::mipmap_type::LINEAR,
                    } );
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
        for( auto& obj_shape : obj_shapes )
        {
            std::size_t index_offset = 0;
            
            // DEBUG:
            std::cout
                << "loading a shape with "
                << obj_shape.mesh.num_face_vertices.size()
                << " faces"
                << std::endl
            ;
            
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
            using group_type = render_object_manager<
                attribute_buffer_type,
                material_description
            >::render_group;
            
            auto objects_ref = object_manager.write();
            
            objects_ref -> emplace_back(
                std::vector< group_type >{},
                vertices
            );
            
            auto& groups = objects_ref -> back().render_groups;
            
            for( std::size_t i = 0; i < materials.size(); ++i )
                groups.emplace_back(
                    std::move( materials[ i ] ),
                    indices[ i ]
                );
            
            GLfloat width  = obj_max_x - obj_min_x;
            GLfloat length = obj_max_y - obj_min_y;
            GLfloat height = obj_max_z - obj_min_z;
            GLfloat scale  = std::max( { width, length, height } );
            // model_scale_transform = scaling< GLfloat >( 1.0f / scale );
            // model_scale_transform = identity_matrix< GLfloat, 4 >();
            model_scale_transform = scaling< GLfloat >( vector< GLfloat, 3 >{
                13.0f / scale /*width*/,
                13.0f / scale /*length*/,
                13.0f / scale /*height*/
            } );
            
            // DEBUG:
            std::cout
                << "OBJ max X = " << obj_max_x << std::endl
                << "OBJ min X = " << obj_min_x << std::endl
                << "OBJ max Y = " << obj_max_y << std::endl
                << "OBJ min Y = " << obj_min_y << std::endl
                << "OBJ max Z = " << obj_max_z << std::endl
                << "OBJ min Z = " << obj_min_z << std::endl
                << "scale transform = "
                << model_scale_transform
                << std::endl
            ;
            
            auto& uploaded_vertices = objects_ref -> back().vertices;
            
            scene_program.link_attribute< 0 >( "position"        , uploaded_vertices );
            scene_program.link_attribute< 1 >( "normal_in"       , uploaded_vertices );
            scene_program.link_attribute< 2 >( "color_in"        , uploaded_vertices );
            scene_program.link_attribute< 3 >( "texture_coord_in", uploaded_vertices );
            scene_program.bind_target< 0 >( "color_out" );
        }
    }
    
    // obj_render_step::~obj_render_step()
    // {
        
    // }
    
    void obj_render_step::run()
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        
        // TODO: error checking
        
        glEnable( GL_DEPTH_TEST );
        glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        scene_program.set_uniform(
            "transform_model",
            model_scale_transform
        );
        
        auto transform_view = (
            yavsg::look_at< GLfloat >(
                yavsg::vector< GLfloat, 3 >( -2.2f, -2.2f, -2.2f ), // Look-at point
                yavsg::vector< GLfloat, 3 >(  0.0f,  0.0f,  1.0f )  // Up vector
            ) * yavsg::translation< GLfloat >(
                yavsg::vector< GLfloat, 3 >( -2.2f, -2.2f, -2.2f )  // Move scene
            )
        );
        scene_program.set_uniform(
            "transform_view",
            transform_view
        );
        
        auto transform_projection = yavsg::perspective< GLfloat >(
            yavsg::radians< GLfloat >( 45 ),
            yavsg::  ratio< GLfloat >(
                ( float )gl_tut::window_width / ( float )gl_tut::window_height
            ),
            1.0f,
            10.0f
        );
        scene_program.set_uniform(
            "transform_projection",
            transform_projection
        );
        
        scene_program.set_uniform(
            "time_absolute",
            std::chrono::duration_cast<
                std::chrono::duration< float >
            >( current_time - start_time ).count()
        );
        
        scene_program.set_uniform(
            "time_delta",
            std::chrono::duration_cast<
                std::chrono::duration< float >
            >( current_time - previous_time ).count()
        );
        
        scene_program.set_uniform(
            "tint",
            yavsg::vector< GLfloat, 3 >( 1.0f, 1.0f, 1.0f )
        );
        
        auto objects_ref = object_manager.read();
        
        for( auto& object : *objects_ref )
        {
            for( auto& group : object.render_groups )
            {
                group.material.bind(
                    scene_program,
                    {
                        "color_map",
                        "normal_map",
                        "specular_map"
                    }
                );
                
                scene_program.run(
                    object.vertices,
                    group.indices
                );
            }
        }
    }
}
