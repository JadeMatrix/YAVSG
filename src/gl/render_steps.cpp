#include "render_steps.hpp"

#include "../../include/math/matrix.hpp"
#include "../../include/math/vector.hpp"
#include "../../include/math/transforms.hpp"
#include "../../include/math/common_transforms.hpp"
#include "../../include/gl/shader_program.hpp"

#include <exception>

// DEBUG:
#include <iostream>


namespace
{
    const std::string folder = "../src/gl";
}


namespace gl // scene_render_step //////////////////////////////////////////////
{
    scene_render_step::scene_render_step()
    {
        start_time = std::chrono::high_resolution_clock::now();
        previous_time = start_time;
        
        auto vertex_shader = shader::from_file(
            GL_VERTEX_SHADER,
            folder + "/vertex_shader.vert"
        );
        auto fragment_shader = shader::from_file(
            GL_FRAGMENT_SHADER,
            folder + "/fragment_shader.frag"
        );
        
        scene_program = new shader_program( {
            vertex_shader.id,
            fragment_shader.id
        } );
        scene_program -> use();
        
        // Load textures ///////////////////////////////////////////////////
        
        glGenTextures( 2, textures );
        
        glBindTexture( GL_TEXTURE_2D, textures[ 0 ] );
        load_bound_texture( "../local/textures/rgb.png" );
        
        glBindTexture( GL_TEXTURE_2D, textures[ 1 ] );
        load_bound_texture( "../local/textures/b&w.png" );
        
        
        // Create shader input data ////////////////////////////////////////
        
        float triangle_vertices[] = {
        //  Position ----------| Color ----------| Texture --|
        //      X,     Y,     Z,    R,    G,    B,    U,    V
            
            // Cube //////////////////////////////////////////
            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

            -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

            -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

            -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            
            // Reflection ////////////////////////////////////
            -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
             1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
             1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
        };
        glGenBuffers( 1, &triangle_vbo );
        glBindBuffer( GL_ARRAY_BUFFER, triangle_vbo );
        glBufferData(
           GL_ARRAY_BUFFER,
           sizeof( triangle_vertices ),
           triangle_vertices,
           GL_STATIC_DRAW // GL_DYNAMIC_DRAW, GL_STREAM_DRAW
        );
        
        
        // Configure shader program attributes /////////////////////////////
        
        /* INPROGRESS:
        
        using tutorial_buffer_type = yavsg::attribute_buffer<
            yavsg::vector< GLfloat, 3 >,
            yavsg::vector< GLfloat, 3 >,
            yavsg::vector< GLfloat, 2 >
        >;
        
        tutorial_buffer_type::tuple_type vertex_data[] = {
        //  Position                 Color                 Texture
            { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f },
            ...
        };
        
        auto buffer = tutorial_buffer_type(
            vertex_data,
            sizeof( vertex_data ) / sizeof( tutorial_buffer_type::tuple_type )
        );
        
        auto vertex_shader = shader::from_file(
            GL_VERTEX_SHADER,
            folder + "/vertex_shader.vert"
        );
        auto fragment_shader = shader::from_file(
            GL_FRAGMENT_SHADER,
            folder + "/fragment_shader.frag"
        );
        
        scene_program = new shader_program< tutorial_buffer_type >(
            {
                vertex_shader.id,
                fragment_shader.id
            },
            buffer
        );
        scene_program -> use();
        
        scene_program -> link_attribute< 0 >( "position"         );
        scene_program -> link_attribute< 1 >( "color_in"         );
        scene_program -> link_attribute< 2 >( "texture_coord_in" );
        
        glGenTextures( 2, textures );
        
        glBindTexture( GL_TEXTURE_2D, textures[ 0 ] );
        load_bound_texture( "../local/textures/rgb.png" );
        
        glBindTexture( GL_TEXTURE_2D, textures[ 1 ] );
        load_bound_texture( "../local/textures/b&w.png" );
        
        */
        
        GLint position_attr = scene_program -> attribute( "position" );
        glEnableVertexAttribArray( position_attr );
        glVertexAttribPointer(
            position_attr,       // Data source
            3,                   // Components per element
            GL_FLOAT,            // Component type
            GL_FALSE,            // Components should be normalized
            8 * sizeof( float ), // Component stride in bytes (0 = packed)
            NULL                 // Component offset within stride
        );
        
        GLint color_attr = scene_program -> attribute( "color_in" );
        glEnableVertexAttribArray( color_attr );
        glVertexAttribPointer(
            color_attr,          // Data source
            3,                   // Components per element
            GL_FLOAT,            // Component type
            GL_FALSE,            // Components should be normalized
            8 * sizeof( float ), // Component stride in bytes (0 = packed)
            ( void* )( 3 * sizeof( float ) )
                                 // Component offset within stride
        );
        
        GLint texture_coord_attr = scene_program -> attribute(
            "texture_coord_in"
        );
        glEnableVertexAttribArray( texture_coord_attr );
        glVertexAttribPointer(
            texture_coord_attr,  // Data source
            2,                   // Components per element
            GL_FLOAT,            // Component type
            GL_FALSE,            // Components should be normalized
            8 * sizeof( float ), // Component stride in bytes (0 = packed)
            ( void* )( 6 * sizeof( float ) )
                                 // Component offset within stride
        );
    }
    /*
    {
        auto vertex_shader = shader::from_file(
            GL_VERTEX_SHADER,
            folder + "/vertex_shader.vert"
        );
        auto geometry_shader = shader::from_file(
            GL_GEOMETRY_SHADER,
            folder + "/geometry_shader.geom"
        );
        auto fragment_shader = shader::from_file(
            GL_FRAGMENT_SHADER,
            folder + "/fragment_shader.frag"
        );
        
        scene_program = new shader_program( {
            vertex_shader.id,
            geometry_shader.id,
            fragment_shader.id
        } );
        scene_program -> use();
        
        
        // Create shader input data ////////////////////////////////////////
        
        float vertices[] = {
        //  Position -------------| Color ----------| Texture --| Sides
        //       X,      Y,      Z,    R,    G,    B,    U,    V,    N
            -0.45f,  0.45f,         1.0f, 0.0f, 0.0f,             4.0f,
             0.45f,  0.45f,         0.0f, 1.0f, 0.0f,             8.0f,
             0.45f, -0.45f,         0.0f, 0.0f, 1.0f,            16.0f,
            -0.45f, -0.45f,         1.0f, 1.0f, 0.0f,            32.0f
        };
        glGenBuffers( 1, &triangle_vbo );
        glBindBuffer( GL_ARRAY_BUFFER, triangle_vbo );
        glBufferData(
           GL_ARRAY_BUFFER,
           sizeof( vertices ),
           vertices,
           GL_STATIC_DRAW
        );
        
        
        // Configure shader program attributes /////////////////////////////
        
        GLint position_attr = scene_program -> attribute( "position" );
        glEnableVertexAttribArray( position_attr );
        glVertexAttribPointer(
            position_attr,       // Data source
            2,                   // Components per element
            GL_FLOAT,            // Component type
            GL_FALSE,            // Components should be normalized
            6 * sizeof( float ), // Component stride in bytes (0 = packed)
            NULL                 // Component offset within stride
        );
        
        GLint color_attr = scene_program -> attribute( "color_in" );
        glEnableVertexAttribArray( color_attr );
        glVertexAttribPointer(
            color_attr,          // Data source
            3,                   // Components per element
            GL_FLOAT,            // Component type
            GL_FALSE,            // Components should be normalized
            6 * sizeof( float ), // Component stride in bytes (0 = packed)
            ( void* )( 2 * sizeof( float ) )
                                 // Component offset within stride
        );
        
        GLint sides_attr = scene_program -> attribute( "sides" );
        glEnableVertexAttribArray( sides_attr );
        glVertexAttribPointer(
            sides_attr,          // Data source
            1,                   // Components per element
            GL_FLOAT,            // Component type
            GL_FALSE,            // Components should be normalized
            6 * sizeof( float ), // Component stride in bytes (0 = packed)
            ( void* )( 5 * sizeof( float ) )
                                 // Component offset within stride
        );
    }
    */
    
    scene_render_step::~scene_render_step()
    {
        glDeleteTextures( 2, textures );
        glDeleteBuffers( 1, &triangle_vbo );
        delete scene_program;
    }
    /*
    {
        glDeleteBuffers( 1, &triangle_vbo );
        delete scene_program;
    }
    */
    
    void scene_render_step::run( framebuffer& previous_framebuffer )
    {
        scene_program -> use();
        
        auto current_time = std::chrono::high_resolution_clock::now();
        
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, textures[ 0 ] );
        scene_program -> set_uniform( "texture_A", 0 );
        
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, textures[ 1 ] );
        scene_program -> set_uniform( "texture_B", 1 );
        
        scene_program -> try_set_uniform(
            "time_absolute",
            std::chrono::duration_cast<
                std::chrono::duration< float >
            >( current_time - start_time ).count()
        );
        scene_program -> try_set_uniform(
            "time_delta",
            std::chrono::duration_cast<
                std::chrono::duration< float >
            >( current_time - previous_time ).count()
        );
        
        previous_time = current_time;
        
        glEnable( GL_DEPTH_TEST );
        glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        
        // Set up matrices /////////////////////////////////////////////////
        
        auto transform_model = yavsg::identity_matrix< GLfloat, 4 >();
        scene_program -> try_set_uniform(
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
        scene_program -> try_set_uniform(
            "transform_view",
            transform_view
        );
        
        auto transform_projection = yavsg::perspective< GLfloat >(
            yavsg::radians< GLfloat >( 45 ),
            yavsg::  ratio< GLfloat >( 800.0f / 600.0f ),
            1.0f,
            10.0f
        );
        // auto transform_projection = yavsg::orthographic< GLfloat >(
        //     yavsg::ratio< GLfloat >( 800.0f / 600.0f ),
        //     1.0f,
        //     10.0f
        // );
        scene_program -> try_set_uniform(
            "transform_projection",
            transform_projection
        );
        
        
        // Draw cube ///////////////////////////////////////////////////////
        
        glDrawArrays(
            GL_TRIANGLES,   // Type of primitive
            0,              // Starting at element
            36              // Number of elements
        );
        
        
        // Stencil write for floor /////////////////////////////////////////
        
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
        
        
        // Draw floor //////////////////////////////////////////////////////
        
        glDepthMask( GL_FALSE );
        glDrawArrays(
            GL_TRIANGLES,   // Type of primitive
            36,             // Starting at element
            6               // Number of elements
        );
        glDepthMask( GL_TRUE );
        
        
        // Set stencil test ////////////////////////////////////////////////
        
        glStencilFunc( GL_EQUAL, 1, 0xFF );
        glStencilMask( 0x00 );
        
        
        // Draw reflected cube /////////////////////////////////////////////
        
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
        scene_program -> try_set_uniform(
            "transform_model",
            transform_model
        );
        
        scene_program -> try_set_uniform(
            "tint",
            yavsg::vector< GLfloat, 3 >( 0.3f, 0.3f, 0.3f )
        );
        glDrawArrays(
            GL_TRIANGLES,   // Type of primitive
            0,              // Starting at element
            36              // Number of elements
        );
        scene_program -> try_set_uniform(
            "tint",
            yavsg::vector< GLfloat, 3 >( 1.0f, 1.0f, 1.0f )
        );
        
        
        // Disable stencil testing /////////////////////////////////////////
        
        glDisable( GL_STENCIL_TEST );
    }
    /*
    {
        scene_program -> use();
        
        glEnable( GL_DEPTH_TEST );
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        glDrawArrays(
            GL_POINTS,  // Type of primitive
            0,          // Starting at element
            4           // Number of elements
        );
    }
    */
}


namespace gl // postprocess_render_step ////////////////////////////////////////
{
    postprocess_render_step::postprocess_render_step()
    {
        auto vertex_shader = shader::from_file(
            GL_VERTEX_SHADER,
            folder + "/postprocess.vert"
        );
        auto fragment_shader = shader::from_file(
            GL_FRAGMENT_SHADER,
            folder + "/postprocess.frag"
        );
        
        postprocess_program = new shader_program( {
            vertex_shader.id,
            fragment_shader.id
        } );
        postprocess_program -> use();
        
        float triangle_vertices[] = {
        //      X,     Y,    S,    T
            -1.0f,  1.0f, 0.0f, 0.0f, //    top left
             1.0f,  1.0f, 1.0f, 0.0f, //    top right
             1.0f, -1.0f, 1.0f, 1.0f, // bottom right
            -1.0f, -1.0f, 0.0f, 1.0f  // bottom left
        };
        glGenBuffers( 1, &triangle_vbo );
        glBindBuffer( GL_ARRAY_BUFFER, triangle_vbo );
        glBufferData(
           GL_ARRAY_BUFFER,
           sizeof( triangle_vertices ),
           triangle_vertices,
           GL_STATIC_DRAW // GL_DYNAMIC_DRAW, GL_STREAM_DRAW
        );
        
        GLuint triangle_elements[] = {
            0, 1, 2,
            2, 3, 0
        };
        glGenBuffers( 1, &triangle_ebo );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, triangle_ebo );
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof( triangle_elements ),
            triangle_elements,
            GL_STATIC_DRAW
        );
        
        GLint position_attr = postprocess_program -> attribute( "position" );
        glEnableVertexAttribArray( position_attr );
        glVertexAttribPointer(
            position_attr,       // Data source
            2,                   // Components per element
            GL_FLOAT,            // Component type
            GL_FALSE,            // Components should be normalized
            4 * sizeof( float ), // Component stride in bytes (0 = packed)
            NULL                 // Component offset within stride
        );
        
        GLint texture_coord_attr = postprocess_program -> attribute(
            "texture_coord_in"
        );
        glEnableVertexAttribArray( texture_coord_attr );
        glVertexAttribPointer(
            texture_coord_attr,  // Data source
            2,                   // Components per element
            GL_FLOAT,            // Component type
            GL_FALSE,            // Components should be normalized
            4 * sizeof( float ), // Component stride in bytes (0 = packed)
            ( void* )( 2 * sizeof( float ) )
                                 // Component offset within stride
        );
    }
    
    postprocess_render_step::~postprocess_render_step()
    {
        delete postprocess_program;
        glDeleteBuffers( 1, &triangle_vbo );
    }
    
    void postprocess_render_step::run( framebuffer& previous_framebuffer )
    {
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        glDisable( GL_DEPTH_TEST );
        postprocess_program -> use();
        
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, previous_framebuffer.color_buffer );
        postprocess_program -> set_uniform( "framebuffer", 0 );
        
        glBindVertexArray( triangle_vbo );
        
        glDrawElements(
            GL_TRIANGLES,       // Type of primitive
            6,                  // Number of elements
            GL_UNSIGNED_INT,    // Type of element
            0                   // Starting at element
        );
    }
}


namespace gl // compute_render_step ////////////////////////////////////////////
{
    // compute_render_step::compute_render_step()
    
    // compute_render_step::~compute_render_step()
    
    // void compute_render_step::run( framebuffer& previous_framebuffer )
}
