#include <yavsg/rendering/dof_postprocess_step.hpp>

#include <yavsg/gl/error.hpp>
#include <yavsg/gl/shader.hpp>
#include <yavsg/rendering/shader_utils.hpp> // shaders_dir
#include <yavsg/rendering/shader_variable_names.hpp>

#include <string_view>


namespace
{
    using namespace std::string_view_literals;
}


JadeMatrix::yavsg::dof_postprocess_step::dof_postprocess_step(
    camera const& sc
) :
    postprocess_program( {
        yavsg::gl::shader::from_file(
            GL_VERTEX_SHADER,
            shaders_dir() / "postprocess.vert"sv
        ).id,
        yavsg::gl::shader::from_file(
            GL_FRAGMENT_SHADER,
            shaders_dir() / "dof.frag"sv
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
        shader_string_id::vertex_in_position,
        vertices
    );
    postprocess_program.link_attribute< 1 >(
        shader_string_id::vertex_in_texture,
        vertices
    );
    postprocess_program.bind_target< 0 >(
        shader_string_id::fragment_out_color
    );
}

void JadeMatrix::yavsg::dof_postprocess_step::run(
    source_type          const& source,
    gl::write_only_framebuffer& target
)
{
    gl::ClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    gl::Clear(
          GL_COLOR_BUFFER_BIT
        | GL_DEPTH_BUFFER_BIT
        | GL_STENCIL_BUFFER_BIT
    );
    gl::Disable( GL_DEPTH_TEST );
    
    source.color_buffer< 0 >().bind_as< 0 >();
    postprocess_program.set_uniform(
        shader_string_id::framebuffer_source_color,
        0
    );
    
    // Depth buffer
    source.depth_stencil_buffer().bind_as< 1 >();
    postprocess_program.set_uniform(
        shader_string_id::framebuffer_source_depth,
        1
    );
    
    postprocess_program.set_uniform(
        shader_string_id::camera_point_near,
        scene_camera.near_point()
    );
    postprocess_program.set_uniform(
        shader_string_id::camera_point_focal,
        scene_camera.focal_point()
    );
    postprocess_program.set_uniform(
        shader_string_id::camera_point_far,
        scene_camera.far_point()
    );
    
    postprocess_program.set_uniform(
        shader_string_id::framebuffer_target_width,
        static_cast< GLfloat >( target.width() )
    );
    postprocess_program.set_uniform(
        shader_string_id::framebuffer_target_height,
        static_cast< GLfloat >( target.height() )
    );
    
    postprocess_program.run( vertices, indices );
}