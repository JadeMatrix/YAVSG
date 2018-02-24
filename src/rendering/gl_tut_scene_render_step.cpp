#include "../../include/rendering/gl_tut.hpp"
#include "../../include/rendering/gl_tut_scene_render_step.hpp"

#include "../../include/gl/shader.hpp"
#include "../../include/gl/texture.hpp"
#include "../../include/math/common_transforms.hpp"
#include "../../include/math/matrix.hpp"


namespace gl_tut
{
    scene_render_step::scene_render_step()
    {
        start_time = std::chrono::high_resolution_clock::now();
        previous_time = start_time;
        
        // Shader program attributes ///////////////////////////////////////////
        
        program_type::tuple_type vertices[] = {
        //    Position                 Color                 Texture
            
            // Cube //////////////////////////////////////////
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },

            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },

            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },

            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },

            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
            { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
            { {  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
            { { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
            { { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },

            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
            { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
            { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
            { { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
            { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
            
            // Reflection ////////////////////////////////////
            { { -1.0f, -1.0f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
            { {  1.0f, -1.0f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
            { {  1.0f,  1.0f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
            { {  1.0f,  1.0f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
            { { -1.0f,  1.0f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
            { { -1.0f, -1.0f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } }
        };
        
        // Create shader program & link attributes /////////////////////////////
        
        auto vertex_shader = yavsg::gl::shader::from_file(
            GL_VERTEX_SHADER,
            "../src/shaders/scene.vert"
        );
        auto fragment_shader = yavsg::gl::shader::from_file(
            GL_FRAGMENT_SHADER,
            "../src/shaders/scene.frag"
        );
        
        scene_program = new program_type(
            {
                vertex_shader.id,
                fragment_shader.id
            },
            vertices,
            sizeof( vertices ) / sizeof( program_type::tuple_type )
        );
        scene_program -> use();
        
        scene_program -> link_attribute< 0 >( "position"         );
        scene_program -> link_attribute< 1 >( "color_in"         );
        scene_program -> link_attribute< 2 >( "texture_coord_in" );
        
        // Load textures ///////////////////////////////////////////////////////
        
        glGenTextures( 2, textures );
        glBindTexture( GL_TEXTURE_2D, textures[ 0 ] );
        yavsg::gl::load_bound_texture( "../local/textures/rgb.png" );
        glBindTexture( GL_TEXTURE_2D, textures[ 1 ] );
        yavsg::gl::load_bound_texture( "../local/textures/b&w.png" );
    }
    
    scene_render_step::~scene_render_step()
    {
        glDeleteTextures( 2, textures );
        // glDeleteBuffers( 1, &triangle_vbo );
        // delete scene_program;
    }
    
    void scene_render_step::run()
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        
        glEnable( GL_DEPTH_TEST );
        glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        scene_program -> use();
        
        // Set up matrices /////////////////////////////////////////////////////
        
        auto transform_model = yavsg::identity_matrix< GLfloat, 4 >();
        scene_program -> set_uniform(
            "transform_model",
            transform_model
        );
        
        // auto transform_view = yavsg::look_at< GLfloat >(
        //     yavsg::vector< GLfloat, 3 >( 2.2f, 2.2f, 2.2f ), // Camera position
        //     yavsg::vector< GLfloat, 3 >( 0.0f, 0.0f, 0.0f ), // Look-at point
        //     yavsg::vector< GLfloat, 3 >( 0.0f, 0.0f, 1.0f )  // Up vector
        // );
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
                ( float )window_width / ( float )window_height
            ),
            1.0f,
            10.0f
        );
        // auto transform_projection = yavsg::orthographic< GLfloat >(
        //     yavsg::ratio< GLfloat >(
        //         ( float )window_width / ( float )window_height
        //     ),
        //     1.0f,
        //     10.0f
        // );
        scene_program -> set_uniform(
            "transform_projection",
            transform_projection
        );
        
        // Set other program uniforms //////////////////////////////////////////
        
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, textures[ 0 ] );
        scene_program -> set_uniform< GLint >( "texture_A", 0 );
        
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, textures[ 1 ] );
        scene_program -> set_uniform< GLint >( "texture_B", 1 );
        
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
        
        scene_program -> use();
        
        // Draw cube ///////////////////////////////////////////////////////////
        
        glDrawArrays(
            GL_TRIANGLES,   // Type of primitive
            0,              // Starting at element
            36              // Number of elements
        );
        
        // Draw floor //////////////////////////////////////////////////////////
        
        glEnable( GL_STENCIL_TEST );
        
        glStencilFunc(
            GL_ALWAYS,  // Stencil function (GL_NEVER, GL_LESS, GL_LEQUAL, GL_GREATER, GL_GEQUAL, GL_EQUAL, GL_NOTEQUAL, GL_ALWAYS)
            1,          // Stencil compare value
            0xFF        // Stencil value mask
        );
        glStencilOp(
            // GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, GL_DECR, GL_DECR_WRAP, GL_INVERT
            GL_KEEP,    // Stencil fail action
            GL_KEEP,    // Stencil pass, depth fail action
            GL_REPLACE  // Stencil & depth pass action
        );
        glStencilMask(
            0xFF        // Stencil write mask
        );
        
        glClear( GL_STENCIL_BUFFER_BIT );
        
        glDepthMask( GL_FALSE );
        glDrawArrays(
            GL_TRIANGLES,   // Type of primitive
            36,             // Starting at element
            6               // Number of elements
        );
        glDepthMask( GL_TRUE );
        
        // Draw reflected cube /////////////////////////////////////////////////
        
        glStencilFunc( GL_EQUAL, 1, 0xFF );
        glStencilMask( 0x00 );
        
        // Center is in the middle of the model, so first flip upside down then
        // translate down model_height * 2 (-1 * 0.5 * 2 = -1)
        auto flip_top_to_bottom = yavsg::scaling< GLfloat >(
            yavsg::vector< GLfloat, 3 >( 1.0f, 1.0f, -1.0f )
        );
        auto move_down_model_height = yavsg::translation< GLfloat >(
            yavsg::vector< GLfloat, 3 >( 0.0f, 0.0f, -( 1.0f ) )
        );
        transform_model = (
              move_down_model_height
            * flip_top_to_bottom
        );
        scene_program -> set_uniform(
            "transform_model",
            transform_model
        );
        
        scene_program -> set_uniform(
            "tint",
            yavsg::vector< GLfloat, 3 >( 0.3f, 0.3f, 0.3f )
        );
        glDrawArrays(
            GL_TRIANGLES,   // Type of primitive
            0,              // Starting at element
            36              // Number of elements
        );
        scene_program -> set_uniform(
            "tint",
            yavsg::vector< GLfloat, 3 >( 1.0f, 1.0f, 1.0f )
        );
        
        glDisable( GL_STENCIL_TEST );
        
        // Cleanup /////////////////////////////////////////////////////////////
        
        glDisable( GL_DEPTH_TEST );
        
        previous_time = current_time;
    }
}
