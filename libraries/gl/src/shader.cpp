#include <yavsg/gl/shader.hpp>

#include <yavsg/gl/error.hpp>

#include <exception>
#include <fstream>


namespace yavsg { namespace gl
{
    shader::shader( GLenum shader_type, const std::string& source )
    {
        id = glCreateShader( shader_type );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't create shader for yavsg::gl::shader"
        );
        
        try
        {
            auto source_c_string = source.c_str();
            glShaderSource( id, 1, &source_c_string, nullptr );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't load source for shader "
                + std::to_string( id )
                + " for yavsg::gl::shader"
            );
            
            glCompileShader( id );
            
            GLint status;
            glGetShaderiv( id, GL_COMPILE_STATUS, &status );
            if( status != GL_TRUE )
            {
                constexpr GLsizei log_buffer_length = 1024;
                char log_buffer[ log_buffer_length ];
                GLsizei got_buffer_length;
                std::string log;
                
                while( true )
                {
                    glGetShaderInfoLog(
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
                
                throw std::runtime_error{ "failed to compile shader:\n" + log };
            }
        }
        catch( ... )
        {
            glDeleteShader( id );
            throw;
        }
    }
    
    shader::~shader()
    {
        glDeleteShader( id );
    }
    
    shader shader::from_file(
        GLenum shader_type,
        const std::string& filename
    )
    {
        std::filebuf source_file;
        source_file.open( filename, std::ios_base::in );
        if( !source_file.is_open() )
            throw std::runtime_error(
                "could not open shader source file \""
                + filename
                + "\""
            );
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
            throw std::runtime_error(
                "failed to compile shader file \""
                + filename
                + "\": "
                + e.what()
            );
        }
    }
} }