#include <yavsg/gl/shader.hpp>

#include <yavsg/gl/error.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>    // std::filesystem::path support

#include <stdexcept>    // runtime_error
#include <fstream>
#include <string_view>


namespace
{
    using namespace std::string_view_literals;
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
        
        GLint status;
        gl::GetShaderiv( id, GL_COMPILE_STATUS, &status );
        if( status != GL_TRUE )
        {
            constexpr GLsizei log_buffer_length = 1024;
            char log_buffer[ log_buffer_length ];
            GLsizei got_buffer_length;
            std::string log;
            
            while( true )
            {
                gl::GetShaderInfoLog(
                    id,
                    log_buffer_length,
                    &got_buffer_length,
                    log_buffer
                );
                if( got_buffer_length <= 0 )
                    break;
                log += std::string(
                    log_buffer,
                    static_cast< std::size_t >( got_buffer_length )
                );
            }
            
            throw std::runtime_error{ fmt::format(
                "failed to compile shader:\n{}"sv,
                log
            ) };
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
