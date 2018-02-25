#include "../../include/rendering/gl_tut.hpp"
#include "../../include/rendering/gl_tut_postprocess_render_step.hpp"

#include "../../include/gl/shader.hpp"


namespace gl_tut // gl_tut_postprocess_render_step implementations ////////////////////
{
    postprocess_render_step::postprocess_render_step(
        const std::string& fragment_shader_filename
    ) :
        postprocess_program( {
            yavsg::gl::shader::from_file(
                GL_VERTEX_SHADER,
                "../src/shaders/postprocess.vert"
            ).id,
            yavsg::gl::shader::from_file(
                GL_FRAGMENT_SHADER,
                fragment_shader_filename
            ).id
        } ),
        vertices( {
            { { -1.0f,  1.0f }, { 0.0f, 0.0f } }, //    top left
            { {  1.0f,  1.0f }, { 1.0f, 0.0f } }, //    top right
            { {  1.0f, -1.0f }, { 1.0f, 1.0f } }, // bottom right
            { { -1.0f, -1.0f }, { 0.0f, 1.0f } }  // bottom left
        } ),
        indices( {
            0, 1, 2,
            2, 3, 0
        } )
    {
        postprocess_program.link_attribute< 0 >( "position"         );
        postprocess_program.link_attribute< 1 >( "texture_coord_in" );
        postprocess_program.bind_target< 0 >( "color_out" );
    }
    
    void postprocess_render_step::run( yavsg::gl::framebuffer< 1 >& source )
    {
        // TODO: error handling
        
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        glDisable( GL_DEPTH_TEST );
        
        source.color_buffer< 0 >().bind_as< 0 >();
        postprocess_program.set_uniform( "framebuffer", 0 );
        
        // Depth buffer
        source.depth_stencil_buffer().bind_as< 1 >();
        postprocess_program.set_uniform( "framebuffer_depth_stencil", 1 );
        
        postprocess_program.set_uniform(
            "view_width",
            ( GLfloat )window_width
        );
        postprocess_program.set_uniform(
            "view_height",
            ( GLfloat )window_height
        );
        
        postprocess_program.run(
            vertices,
            indices
        );
    }
}