#include <yavsg/rendering/multi_postprocess_step.hpp>

#include <yavsg/gl/error.hpp>
#include <yavsg/rendering/shader_utils.hpp> // shaders_dir
#include <yavsg/rendering/shader_variable_names.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>    // std::filesystem::path support

#include <exception>
#include <fstream>
#include <set>
#include <string_view>


namespace
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;
    
    auto const fragment_shader_header = R"<<<(
#version 150 core

in VERTEX_OUT
{
    vec2 texture;
} fragment_in;

uniform sampler2D framebuffer_source_color;
uniform sampler2D framebuffer_source_depth;
uniform struct
{
    float width;
    float height;
} framebuffer_target;

out vec4 fragment_out_color;
)<<<"s;
    
    auto const fragment_shader_body_pre = R"<<<(
void main()
{
    fragment_out_color = texture(
        framebuffer_source_color,
        fragment_in.texture
    );
)<<<"s;
    
    auto const fragment_shader_body_post = "}"s;
}


JadeMatrix::yavsg::multi_postprocess_step::multi_postprocess_step(
    std::vector< std::string > const& function_names
) :
    vertices{ {
        { { -1.0f,  1.0f }, { 0.0f, 0.0f } }, //    top left
        { {  1.0f,  1.0f }, { 1.0f, 0.0f } }, //    top right
        { {  1.0f, -1.0f }, { 1.0f, 1.0f } }, // bottom right
        { { -1.0f, -1.0f }, { 0.0f, 1.0f } }  // bottom left
    } },
    indices{ {
        0, 1, 2,
        2, 3, 0
    } },
    multi_program{ {
        // FIXME: this creates references to temporaries
        gl::shader::from_file(
            GL_VERTEX_SHADER,
            shaders_dir() / "postprocess.vert"sv
        ).id,
        generate_fragment_shader( function_names ).id
    } }
{
    multi_program.link_attribute< 0 >(
        shader_string_id::vertex_in_position,
        vertices
    );
    multi_program.link_attribute< 1 >(
        shader_string_id::vertex_in_texture,
        vertices
    );
    multi_program.bind_target< 0 >(
        shader_string_id::fragment_out_color
    );
}

void JadeMatrix::yavsg::multi_postprocess_step::run(
    gl::framebuffer< gl::texture< GLfloat, 3 > > const& source,
    gl::write_only_framebuffer                        & target
)
{
    gl::ClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    gl::Clear(
          GL_COLOR_BUFFER_BIT
        | GL_DEPTH_BUFFER_BIT
        | GL_STENCIL_BUFFER_BIT
    );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't clear buffer for yavsg::multi_postprocess_step::run()"s
    );
    
    gl::Disable( GL_DEPTH_TEST );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't disable depth testing for "
        "yavsg::multi_postprocess_step::run()"s
    );
    
    source.color_buffer< 0 >().bind_as< 0 >();
    multi_program.set_uniform(
        shader_string_id::framebuffer_source_color,
        0
    );
    
    source.depth_stencil_buffer().bind_as< 1 >();
    multi_program.set_uniform(
        shader_string_id::framebuffer_source_depth,
        1
    );
    
    multi_program.set_uniform(
        shader_string_id::framebuffer_target_width,
        static_cast< GLfloat >( target.width() )
    );
    multi_program.set_uniform(
        shader_string_id::framebuffer_target_height,
        static_cast< GLfloat >( target.height() )
    );
    
    multi_program.run( vertices, indices );
}

JadeMatrix::yavsg::gl::shader
JadeMatrix::yavsg::multi_postprocess_step::generate_fragment_shader(
    std::vector< std::string > const& function_names
)
{
    std::string fragment_shader_source = fragment_shader_header;
    
    // Iterating over a set of function names ensures each one is defined
    // only once even if it's called multiple times
    for( auto& function_name : std::set< std::string >{
        function_names.begin(),
        function_names.end()
    } )
    {
        auto const filename = (
            shaders_dir()
            / "postprocess"
            / ( function_name + ".frag" )
        );
        
        std::filebuf function_source_file;
        function_source_file.open( filename, std::ios_base::in );
        if( !function_source_file.is_open() )
        {
            throw std::runtime_error( fmt::format(
                "could not open shader function source file {}"sv,
                filename
            ) );
        }
        
        fragment_shader_source += std::string(
            std::istreambuf_iterator< char >( &function_source_file ),
            {}
        );
    }
    
    fragment_shader_source += fragment_shader_body_pre;
    
    for( auto& function_name : function_names )
    {
        fragment_shader_source += function_name + "();";
    }
    
    fragment_shader_source += fragment_shader_body_post;
    
    return gl::shader{ GL_FRAGMENT_SHADER, fragment_shader_source };
}
