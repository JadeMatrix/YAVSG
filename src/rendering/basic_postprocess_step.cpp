#include "../../include/rendering/basic_postprocess_step.hpp"

#include "../../include/rendering/gl_tut.hpp"
#include "../../include/rendering/shader_variable_names.hpp"

#include "../../include/gl/shader.hpp"


namespace yavsg
{
    basic_postprocess_step::basic_postprocess_step(
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
        postprocess_program.link_attribute< 0 >(
            shader_string( shader_string_id::VERTEX_IN_POSITION ),
            vertices
        );
        postprocess_program.link_attribute< 1 >(
            shader_string( shader_string_id::VERTEX_IN_TEXTURE ),
            vertices
        );
        postprocess_program.bind_target< 0 >(
            shader_string( shader_string_id::FRAGMENT_OUT_COLOR )
        );
    }
    
    void basic_postprocess_step::run( framebuffer_type& source )
    {
        // TODO: error handling
        
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        glDisable( GL_DEPTH_TEST );
        
        source.color_buffer< 0 >().bind_as< 0 >();
        postprocess_program.set_uniform(
            shader_string( shader_string_id::FRAMEBUFFER_SOURCE_COLOR ),
            0
        );
        
        // Depth buffer
        source.depth_stencil_buffer().bind_as< 1 >();
        postprocess_program.set_uniform(
            shader_string( shader_string_id::FRAMEBUFFER_SOURCE_DEPTH_STENCIL ),
            1
        );
        
        postprocess_program.set_uniform(
            shader_string( shader_string_id::FRAMEBUFFER_TARGET_WIDTH ),
            ( GLfloat )gl_tut::window_width
        );
        postprocess_program.set_uniform(
            shader_string( shader_string_id::FRAMEBUFFER_TARGET_HEIGHT ),
            ( GLfloat )gl_tut::window_height
        );
        
        postprocess_program.run(
            vertices,
            indices
        );
    }
}