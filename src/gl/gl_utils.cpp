#include "gl_utils.hpp"

#include <SDL2/SDL_image.h>

#include <exception>
#include <fstream>


namespace gl // shader /////////////////////////////////////////////////////////
{
    shader::shader( GLenum shader_type, const std::string& source )
    {
        id = glCreateShader( shader_type );
        
        auto source_c_string = source.c_str();
        glShaderSource( id, 1, &source_c_string, nullptr );
        glCompileShader( id );
        
        GLint status;
        glGetShaderiv( id, GL_COMPILE_STATUS, &status );
        if( status != GL_TRUE )
        {
            char log_buffer[ 1024 ];
            glGetShaderInfoLog(
                id,
                1024,
                NULL,
                log_buffer
            );
            std::string shader_error_string = (
                "failed to compile shader:\n"
                + std::string( log_buffer )
            );
            glDeleteShader( id );
            throw std::runtime_error( shader_error_string );
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
}


namespace gl // shader_program /////////////////////////////////////////////////
{
    shader_program::shader_program( const std::vector< GLuint >& shaders )
    {
        glGenVertexArrays( 1, &vao_id );
        glBindVertexArray( vao_id );
        
        id = glCreateProgram();
        for( auto& shader_id : shaders )
            glAttachShader( id, shader_id );
        
        // Note: use glDrawBuffers when rendering to multiple buffers,
        // because only the first output will be enabled by default.
        glBindFragDataLocation( id, 0, "color_out" );
        
        glLinkProgram( id );
    }
    /* {
        glGenVertexArrays( 1, &vao_id );
        glBindVertexArray( vao_id );
        
        id = glCreateProgram();
        for( auto& shader_id : shaders )
            glAttachShader( id, shader_id );
        
        // // Note: use glDrawBuffers when rendering to multiple buffers,
        // // because only the first output will be enabled by default.
        // glBindFragDataLocation( id, 0, "color_out" );
        
        const GLchar* feedback_varyings[] = { "value_out" };
        glTransformFeedbackVaryings(
            id,
            1,                      // Number of varyings
            feedback_varyings,      // Varying outputs array
            GL_INTERLEAVED_ATTRIBS  // How data should be written (vs. GL_SEPARATE_ATTRIBS)
        );
        
        glLinkProgram( id );
    } */
    
    shader_program::~shader_program()
    {
        glDeleteProgram( id );
        glDeleteVertexArrays( 1, &vao_id );
    }
    
    void shader_program::use()
    {
        glUseProgram( id );
        glBindVertexArray( vao_id );
    }
    
    GLint shader_program::attribute( const std::string& attribute_name )
    {
        GLint result = glGetAttribLocation( id, attribute_name.c_str() );
        if( result == -1 )
            throw no_such_variable(
                "unable to get attribute \""
                + attribute_name
                + "\" from shader program "
                + std::to_string( id )
                + " (nonexistent or reserved)"
            );
        return result;
    }
    
    GLint shader_program::uniform( const std::string& uniform_name )
    {
        GLint result = glGetUniformLocation( id, uniform_name.c_str() );
        if( result == -1 )
            throw no_such_variable(
                "unable to get uniform \""
                + uniform_name
                + "\" from shader program "
                + std::to_string( id )
                + " (nonexistent or reserved)"
            );
        return result;
    }
}


namespace gl // framebuffer ////////////////////////////////////////////////////
{
    framebuffer::framebuffer(
        GLsizei width,
        GLsizei height
    )
    {
        glGenFramebuffers( 1, &id );
        glBindFramebuffer( GL_FRAMEBUFFER, id );
        
        glGenTextures( 1, &color_buffer );
        glBindTexture( GL_TEXTURE_2D, color_buffer );
        
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            width, height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            nullptr
        );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,   // Which attachment
            GL_TEXTURE_2D,
            color_buffer,
            0                       // Mipmap level (not useful)
        );
        
        glGenRenderbuffers( 1, &depth_stencil_buffer );
        glBindRenderbuffer( GL_RENDERBUFFER, depth_stencil_buffer );
        
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            width, height
        );
        
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            depth_stencil_buffer
        );
        
        if(
            glCheckFramebufferStatus( GL_FRAMEBUFFER )
            != GL_FRAMEBUFFER_COMPLETE
        )
        {
            glDeleteFramebuffers( 1, &id );
            glDeleteTextures( 1, &color_buffer );
            glDeleteRenderbuffers( 1, &depth_stencil_buffer );
            throw std::runtime_error( "failed to complete famebuffer" );
        }
    }
    
    framebuffer::~framebuffer()
    {
        glDeleteFramebuffers( 1, &id );
        glDeleteTextures( 1, &color_buffer );
        glDeleteRenderbuffers( 1, &depth_stencil_buffer );
    }
}


namespace gl // load_bound_texture /////////////////////////////////////////////
{
    void load_bound_texture( const std::string& filename )
    {
        SDL_Surface* sdl_surface = IMG_Load(
            filename.c_str()
        );
        if( !sdl_surface )
            throw std::runtime_error(
                "failed to load texture \""
                + filename
                + "\": "
                + IMG_GetError()
            );
        // else
        //     std::cout
        //         << "loaded texture \""
        //         << filename
        //         << "\": w="
        //         << sdl_surface -> w
        //         << " h="
        //         << sdl_surface -> h
        //         << " bpp="
        //         << ( int )( sdl_surface -> format -> BytesPerPixel )
        //         << std::endl
        //     ;
        
        GLenum texture_mode = (
            sdl_surface -> format -> BytesPerPixel == 4 ?
            GL_RGBA : GL_RGB
        );
        glTexImage2D(   // Loads starting at 0,0 as bottom left
            GL_TEXTURE_2D,
            0,                      // LoD, 0 = base
            texture_mode,           // Internal format
            sdl_surface -> w,       // Width
            sdl_surface -> h,       // Height
            0,                      // Border; must be 0
            texture_mode,           // Incoming format
            GL_UNSIGNED_BYTE,       // Pixel type
            sdl_surface -> pixels   // Pixel data
        );
        SDL_FreeSurface( sdl_surface );
        
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
        // float texture_border_color[] = { 1.0f, 0.0f, 0.0f };
        // glTexParameterfv(
        //     GL_TEXTURE_2D,
        //     GL_TEXTURE_BORDER_COLOR,
        //     texture_border_color
        // );
        
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glGenerateMipmap( GL_TEXTURE_2D );
    }
}
