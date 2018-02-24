#include "../../include/rendering/gl_tut.hpp"
#include "../../include/rendering/gl_tut_postprocess_render_step.hpp"

#include "../../include/gl/shader.hpp"


namespace gl_tut // gl_tut_postprocess_render_step implementations ////////////////////
{
    postprocess_render_step::postprocess_render_step(
        const std::string& fragment_shader_filename
    )
    {
        // Shader program attributes ///////////////////////////////////////////
        
        program_type::tuple_type vertices[] = {
            { { -1.0f,  1.0f }, { 0.0f, 0.0f } }, //    top left
            { {  1.0f,  1.0f }, { 1.0f, 0.0f } }, //    top right
            { {  1.0f, -1.0f }, { 1.0f, 1.0f } }, // bottom right
            { { -1.0f, -1.0f }, { 0.0f, 1.0f } }  // bottom left
        };
        
        // Create shader program & link attributes /////////////////////////////
        
        auto vertex_shader = yavsg::gl::shader::from_file(
            GL_VERTEX_SHADER,
            "../src/shaders/postprocess.vert"
        );
        auto fragment_shader = yavsg::gl::shader::from_file(
            GL_FRAGMENT_SHADER,
            fragment_shader_filename
        );
        postprocess_program = new program_type(
            {
                vertex_shader.id,
                fragment_shader.id
            },
            vertices,
            sizeof( vertices ) / sizeof( program_type::tuple_type )
        );
        
        // TODO: error handling & stuff
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
        
        postprocess_program -> link_attribute< 0 >( "position"         );
        postprocess_program -> link_attribute< 1 >( "texture_coord_in" );
    }
    
    postprocess_render_step::~postprocess_render_step()
    {
        delete postprocess_program;
    }
    
    void postprocess_render_step::run( yavsg::gl::framebuffer& source )
    {
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        glDisable( GL_DEPTH_TEST );
        postprocess_program -> use();
        
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, source.color_buffer );
        postprocess_program -> set_uniform( "framebuffer", 0 );
        
        postprocess_program -> set_uniform(
            "view_width",
            ( GLfloat )window_width
        );
        postprocess_program -> set_uniform(
            "view_height",
            ( GLfloat )window_height
        );
        
        glDrawElements(
            GL_TRIANGLES,       // Type of primitive
            6,                  // Number of elements
            GL_UNSIGNED_INT,    // Type of element
            0                   // Starting at element
        );
    }
}