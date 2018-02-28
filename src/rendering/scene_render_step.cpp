#include "../../include/rendering/scene_render_step.hpp"

#include "../../include/math/common_transforms.hpp"
#include "../../include/gl/error.hpp"
#include "../../include/gl/shader.hpp"
#include "../../include/gl/texture.hpp"
#include "../../include/rendering/gl_tut.hpp"   // gl_tut::window_width & gl_tut::window_height


namespace yavsg
{
    scene_render_step::scene_render_step(
        const std::string&   vertex_shader_filename,
        const std::string& fragment_shader_filename
    ) :
        scene_program( {
            gl::shader::from_file(
                GL_VERTEX_SHADER,
                vertex_shader_filename
            ).id,
            gl::shader::from_file(
                GL_FRAGMENT_SHADER,
                fragment_shader_filename
            ).id
        } )
    {
        // scene_program.link_attribute< 0 >( "vertex_in_position" );
        // scene_program.link_attribute< 1 >( "vertex_in_normal"   );
        // scene_program.link_attribute< 2 >( "vertex_in_tangent"  );
        // scene_program.link_attribute< 3 >( "vertex_in_color"    );
        // scene_program.link_attribute< 4 >( "vertex_in_texture"  );
        
        // scene_program.bind_target< 0 >( "fragment_out_color" );
    }
    
    void scene_render_step::run()
    {
        update();
        
        if( objects.size() )
        {
            scene_program.link_attribute< 0 >( "vertex_in_position", objects[ 0 ].vertices );
            scene_program.link_attribute< 1 >( "vertex_in_normal"  , objects[ 0 ].vertices );
            scene_program.link_attribute< 2 >( "vertex_in_tangent" , objects[ 0 ].vertices );
            scene_program.link_attribute< 3 >( "vertex_in_color"   , objects[ 0 ].vertices );
            scene_program.link_attribute< 4 >( "vertex_in_texture" , objects[ 0 ].vertices );
        }
        else
        {
            auto some_attribute_buffer = attribute_buffer_type( std::vector< vertex_type >{ {
                { 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f }
            } } );
            scene_program.link_attribute< 0 >( "vertex_in_position", some_attribute_buffer );
            scene_program.link_attribute< 1 >( "vertex_in_normal"  , some_attribute_buffer );
            scene_program.link_attribute< 2 >( "vertex_in_tangent" , some_attribute_buffer );
            scene_program.link_attribute< 3 >( "vertex_in_color"   , some_attribute_buffer );
            scene_program.link_attribute< 4 >( "vertex_in_texture" , some_attribute_buffer );
        }
        
        scene_program.bind_target< 0 >( "fragment_out_color" );
        
        // Set transforms //////////////////////////////////////////////////////
        
        auto transform_view = (
            look_at< GLfloat >(
                vector< GLfloat, 3 >( -2.2f, -2.2f, -2.2f ), // Look-at point
                vector< GLfloat, 3 >(  0.0f,  0.0f,  1.0f )  // Up vector
            ) * translation< GLfloat >(
                vector< GLfloat, 3 >( -2.2f, -2.2f, -2.2f )  // Move scene
            )
        );
        scene_program.set_uniform(
            "transform.view",
            transform_view
        );
        
        auto transform_projection = perspective< GLfloat >(
            radians< GLfloat >( 45 ),
              ratio< GLfloat >(
                  ( GLfloat )gl_tut::window_width
                / ( GLfloat )gl_tut::window_height
            ),
            1.0f,
            10.0f
        );
        scene_program.set_uniform(
            "transform.projection",
            transform_projection
        );
        
        // Clear ///////////////////////////////////////////////////////////////
        
        glEnable( GL_DEPTH_TEST );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't enable depth test for yavsg::scene_render_step::run()"
        );
        
        glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't set clear color for yavsg::scene_render_step::run()"
        );
        
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't clear buffer for yavsg::scene_render_step::run()"
        );
        
        // Draw objects ////////////////////////////////////////////////////////
        
        for( auto& object : objects )
        {
            auto transform_model = (
                translation< GLfloat >( object.position )
                *   scaling< GLfloat >( object.scale    )
                *  rotation< GLfloat >( object.rotation )
            );
            scene_program.set_uniform(
                "transform.model",
                transform_model
            );
            
            for( auto& render_group : object.render_groups )
            {
                // Set up material uniforms ////////////////////////////////////
                
                if( render_group.material.color_map )
                {
                    render_group.material.color_map -> bind_as< 0 >();
                    scene_program.set_uniform< GLint >( "map.color", 0 );
                    scene_program.set_uniform< GLboolean >(
                        "has_map.color",
                        GL_TRUE
                    );
                }
                else
                {
                    gl::unbind_texture< 0 >();
                    scene_program.set_uniform< GLboolean >(
                        "has_map.color",
                        GL_FALSE
                    );
                }
                
                if( render_group.material.normal_map )
                {
                    render_group.material.normal_map -> bind_as< 1 >();
                    scene_program.set_uniform< GLint >( "map.normal", 1 );
                    scene_program.set_uniform< GLboolean >(
                        "has_map.normal",
                        GL_TRUE
                    );
                }
                else
                {
                    gl::unbind_texture< 1 >();
                    scene_program.set_uniform< GLboolean >(
                        "has_map.normal",
                        GL_FALSE
                    );
                }
                
                if( render_group.material.specular_map )
                {
                    render_group.material.specular_map -> bind_as< 2 >();
                    scene_program.set_uniform< GLint >( "map.specular", 2 );
                    scene_program.set_uniform< GLboolean >(
                        "has_map.specular",
                        GL_TRUE
                    );
                }
                else
                {
                    gl::unbind_texture< 2 >();
                    scene_program.set_uniform< GLboolean >(
                        "has_map.specular",
                        GL_FALSE
                    );
                }
                
                // Run program over group vertices /////////////////////////////
                
                scene_program.run(
                    *object.vertices,
                    render_group.vertex_indices
                );
            }
        }
    }
}
