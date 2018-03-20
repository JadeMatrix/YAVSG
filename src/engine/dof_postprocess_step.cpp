#include "../../include/engine/dof_postprocess_step.hpp"

#include "../../include/gl/error.hpp"
#include "../../include/gl/shader.hpp"
#include "../../include/rendering/shader_variable_names.hpp"


namespace yavsg
{
    dof_postprocess_step::dof_postprocess_step( const camera& sc ) :
        postprocess_program( {
            yavsg::gl::shader::from_file(
                GL_VERTEX_SHADER,
                "../src/shaders/postprocess.vert"
            ).id,
            yavsg::gl::shader::from_file(
                GL_FRAGMENT_SHADER,
                "../src/shaders/dof.frag"
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
        } ),
        scene_camera( sc )
    {
        postprocess_program.link_attribute< 0 >(
            shader_string_id::VERTEX_IN_POSITION,
            vertices
        );
        postprocess_program.link_attribute< 1 >(
            shader_string_id::VERTEX_IN_TEXTURE,
            vertices
        );
        postprocess_program.bind_target< 0 >(
            shader_string_id::FRAGMENT_OUT_COLOR
        );
    }
    
    void dof_postprocess_step::run(
        source_type               & source,
        gl::write_only_framebuffer& target
    )
    {
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't clear buffer for yavsg::dof_postprocess_step::run()"
        );
        
        glDisable( GL_DEPTH_TEST );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't disable depth testing for "
            "yavsg::dof_postprocess_step::run()"
        );
        
        source.color_buffer< 0 >().bind_as< 0 >();
        postprocess_program.set_uniform(
            shader_string_id::FRAMEBUFFER_SOURCE_COLOR,
            0
        );
        
        // Depth buffer
        source.depth_stencil_buffer().bind_as< 1 >();
        postprocess_program.set_uniform(
            shader_string_id::FRAMEBUFFER_SOURCE_DEPTH,
            1
        );
        
        postprocess_program.set_uniform(
            shader_string_id::CAMERA_POINT_NEAR,
            scene_camera.near_point()
        );
        postprocess_program.set_uniform(
            shader_string_id::CAMERA_POINT_FOCAL,
            scene_camera.focal_point()
        );
        postprocess_program.set_uniform(
            shader_string_id::CAMERA_POINT_FAR,
            scene_camera.far_point()
        );
        
        postprocess_program.set_uniform(
            shader_string_id::FRAMEBUFFER_TARGET_WIDTH,
            static_cast< GLfloat >( target.width() )
        );
        postprocess_program.set_uniform(
            shader_string_id::FRAMEBUFFER_TARGET_HEIGHT,
            static_cast< GLfloat >( target.height() )
        );
        
        postprocess_program.run(
            vertices,
            indices
        );
    }
}