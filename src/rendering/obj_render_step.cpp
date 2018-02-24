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
    ) : model_scale_transform(
        square_matrix< GLfloat, 4 >::make_uninitialized()
    )
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
        
        // Load textures
        for( auto& material : obj_materials )
        // for( int i = 0; i < obj_materials.size(); ++i )
        {
            render_groups.push_back( { false, 0, {} } );
            auto& group = render_groups.back();
            
            if( material.ambient_texname.size() )
            {
                group.has_color_map = true;
                glGenTextures( 1, &group.color_map );
                glBindTexture( GL_TEXTURE_2D, group.color_map );
                std::string texture_filename =
                    obj_mtl_directory + material.ambient_texname;
                
                // DEBUG:
                std::cout
                    << "loading texture "
                    << texture_filename
                    << std::endl
                ;
                
                yavsg::gl::load_bound_texture( texture_filename );
            }
        }
        
        // Create vertices
        for( auto& obj_shape : obj_shapes )
        {
            size_t index_offset = 0;
            
            // DEBUG:
            std::cout
                << "loading a shape with "
                << obj_shape.mesh.num_face_vertices.size()
                << " faces"
                << std::endl
            ;
            
            // for( auto& vertex_count : obj_shape.mesh.num_face_vertices )
            for( size_t face = 0; face < obj_shape.mesh.num_face_vertices.size(); ++face )
            {
                auto vertex_count = obj_shape.mesh.num_face_vertices[ face ];
                
                if( vertex_count != 3 )
                    throw std::runtime_error( "vertex count = " + std::to_string( vertex_count ) );
                
                auto& group = render_groups[
                    obj_shape.mesh.material_ids[ face ]
                ];
                
                for( size_t i = 0; i < vertex_count; ++i )
                {
                    auto index = obj_shape.mesh.indices[ index_offset + i ];
                    
                    auto vx = obj_attributes.vertices[ 3 * index.vertex_index + 0 ];
                    auto vy = obj_attributes.vertices[ 3 * index.vertex_index + 1 ];
                    auto vz = obj_attributes.vertices[ 3 * index.vertex_index + 2 ];
                    
                    auto cr = obj_attributes.colors[ 3 * index.vertex_index + 0 ];
                    auto cb = obj_attributes.colors[ 3 * index.vertex_index + 1 ];
                    auto cg = obj_attributes.colors[ 3 * index.vertex_index + 2 ];
                    
                    auto tu = obj_attributes.texcoords[ 2 * index.texcoord_index + 0 ];
                    auto tv = obj_attributes.texcoords[ 2 * index.texcoord_index + 1 ];
                    
                    group.vertices.push_back( {
                        // { vx, vy, vz },
                        { vx, vz, vy },
                        { cr, cb, cg },
                        { tu, tv }
                    } );
                    
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
        
        // DEBUG:
        std::cout
            << "loaded "
            << render_groups.size()
            << " render groups:"
            << std::endl
        ;
        for( auto& group : render_groups )
            std::cout
                << "  group:"
                << std::endl
                << "    color map: "
                << ( group.has_color_map ? std::to_string( group.color_map ) : "none" )
                << std::endl
                << "    vertices: "
                << group.vertices.size()
                << std::endl
            ;
        
        // // DEBUG:
        // for( auto& group : render_groups )
        // {
        //     std::cout
        //         << "group:"
        //         << std::endl
        //         << "  color map: "
        //         << ( group.has_color_map ? std::to_string( group.color_map ) : "none" )
        //         << std::endl
        //         << "  vertices ("
        //         << group.vertices.size()
        //         << "):"
        //         << std::endl
        //     ;
            
        //     for( auto& vertex : group.vertices )
        //         std::cout
        //             << "    "
        //             << std::get< 0 >( vertex )
        //             << ", "
        //             << std::get< 1 >( vertex )
        //             << ", "
        //             << std::get< 2 >( vertex )
        //             << std::endl
        //         ;
        // }
        
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
        
        auto vertex_shader = yavsg::gl::shader::from_file(
            GL_VERTEX_SHADER,
            "../src/shaders/obj_scene.vert"
        );
        auto fragment_shader = yavsg::gl::shader::from_file(
            GL_FRAGMENT_SHADER,
            "../src/shaders/obj_scene.frag"
        );
        
        // TODO: re-add separate buffers and don't require them for shader_program's constructor
        scene_program = new program_type(
            {
                vertex_shader.id,
                fragment_shader.id
            },
            &render_groups[ 0 ].vertices[ 0 ],
            render_groups[ 0 ].vertices.size()
        );
        
        scene_program -> link_attribute< 0 >( "position"         );
        scene_program -> link_attribute< 1 >( "color_in"         );
        scene_program -> link_attribute< 2 >( "texture_coord_in" );
    }
    
    obj_render_step::~obj_render_step()
    {
        delete scene_program;
        for( auto group : render_groups )
            if( group.has_color_map )
                glDeleteTextures( 1, &group.color_map );
    }
    
    void obj_render_step::run()
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        
        glEnable( GL_DEPTH_TEST );
        glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        // TODO: probably not needed anymore
        scene_program -> use();
        
        scene_program -> set_uniform(
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
        scene_program -> set_uniform(
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
        scene_program -> set_uniform(
            "transform_projection",
            transform_projection
        );
        
        scene_program -> set_uniform(
            "time_absolute",
            std::chrono::duration_cast<
                std::chrono::duration< float >
            >( current_time - start_time ).count()
        );
        
        scene_program -> set_uniform(
            "time_delta",
            std::chrono::duration_cast<
                std::chrono::duration< float >
            >( current_time - previous_time ).count()
        );
        
        scene_program -> set_uniform(
            "tint",
            yavsg::vector< GLfloat, 3 >( 1.0f, 1.0f, 1.0f )
        );
        
        for( auto& group : render_groups )
        {
            scene_program -> set_data(
                &group.vertices[ 0 ],
                group.vertices.size()
            );
            
            scene_program -> link_attribute< 0 >( "position"         );
            scene_program -> link_attribute< 1 >( "color_in"         );
            scene_program -> link_attribute< 2 >( "texture_coord_in" );
            
            glActiveTexture( GL_TEXTURE0 );
            if( group.has_color_map )
            {
                glBindTexture( GL_TEXTURE_2D, group.color_map );
                scene_program -> set_uniform< GLint >( "color_map", 0 );
            }
            else
                glBindTexture( GL_TEXTURE_2D, 0);
            
            glDrawArrays(
                GL_TRIANGLES,
                0,
                group.vertices.size()
            );
        }
    }
}
