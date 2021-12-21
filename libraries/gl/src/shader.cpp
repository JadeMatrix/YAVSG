#include <yavsg/gl/shader.hpp>

#include <yavsg/gl/error.hpp>
#include <yavsg/logging.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>    // std::filesystem::path support

#include <stdexcept>    // runtime_error
#include <fstream>
#include <string_view>


namespace
{
    using namespace std::string_view_literals;
    
    auto const log_ = JadeMatrix::yavsg::log_handle();
}


JadeMatrix::yavsg::gl::shader::shader(
    GLenum             shader_type,
    std::string const& source
)
{
    id = gl::CreateShader( shader_type );
    
    try
    {
        auto source_c_string = source.c_str();
        gl::ShaderSource( id, 1, &source_c_string, nullptr );
        
        gl::CompileShader( id );
        
        GLsizei log_length;
        gl::GetShaderiv( id, GL_INFO_LOG_LENGTH, &log_length );
        std::string compile_log(
            (
                log_length  < 0
                ? 0ul
                : static_cast< std::string::size_type >( log_length )
            ),
            '\0'
        );
        gl::GetShaderInfoLog( id, log_length, nullptr, compile_log.data() );
        
        GLint status;
        gl::GetShaderiv( id, GL_COMPILE_STATUS, &status );
        if( status != GL_TRUE )
        {
            throw error{ fmt::format(
                "failed to compile shader:\n{}"sv,
                compile_log
            ) };
        }
        else
        {
            log_.split_on_newlines_as(
                ext::log::level::warning,
                compile_log,
                "OpenGL: {}"sv
            );
        }
    }
    catch( ... )
    {
        gl::DeleteShader( id );
        throw;
    }
}

JadeMatrix::yavsg::gl::shader::~shader()
{
    gl::DeleteShader( id );
}

JadeMatrix::yavsg::gl::shader JadeMatrix::yavsg::gl::shader::from_file(
    GLenum                       shader_type,
    std::filesystem::path const& filename
)
{
    std::filebuf source_file;
    source_file.open( filename, std::ios_base::in );
    if( !source_file.is_open() )
        throw std::runtime_error( fmt::format(
            "could not open shader source file {}"sv,
            filename
        ) );
    std::string source = std::string(
        std::istreambuf_iterator< char >( &source_file ),
        {}
    );
    try
    {
        return shader( shader_type, source );
    }
    catch( const std::runtime_error& e )
    {
        throw std::runtime_error( fmt::format(
            "failed to compile shader file {}: {}"sv,
            filename,
            e.what()
        ) );
    }
}
