#include <yavsg/rendering/basic_postprocess_step.hpp>

#include <yavsg/gl/error.hpp>
#include <yavsg/gl/shader.hpp>
#include <yavsg/rendering/shader_utils.hpp> // shaders_dir
#include <yavsg/rendering/shader_variable_names.hpp>

#include <string>
#include <string_view>


namespace
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;
}


JadeMatrix::yavsg::basic_postprocess_step::basic_postprocess_step(
    std::filesystem::path const& fragment_shader_filename
) :
    postprocess_program( {
        yavsg::gl::shader::from_file(
            GL_VERTEX_SHADER,
            shaders_dir() / "postprocess.vert"sv
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

void JadeMatrix::yavsg::basic_postprocess_step::run(
    source_type          const& source,
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
        "couldn't clear buffer for yavsg::basic_postprocess_step::run()"s
    );
    
    glDisable( GL_DEPTH_TEST );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't disable depth testing for "
        "yavsg::basic_postprocess_step::run()"s
    );
    
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
        shader_string_id::framebuffer_target_width,
        static_cast< GLfloat >( target.width() )
    );
    postprocess_program.set_uniform(
        shader_string_id::framebuffer_target_height,
        static_cast< GLfloat >( target.height() )
    );
    
    postprocess_program.run( vertices, indices );
}