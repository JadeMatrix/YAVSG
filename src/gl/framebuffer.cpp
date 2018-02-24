#include "../../include/gl/framebuffer.hpp"


namespace yavsg { namespace gl
{
    framebuffer::framebuffer(
        GLsizei width,
        GLsizei height
    )
    {
        glGenFramebuffers( 1, &id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't generate framebuffer for yavsg::gl::framebuffer"
        );
        bind();
        
        try
        {
            glGenTextures( 1, &color_buffer );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't generate texture for yavsg::gl::framebuffer"
            );
            
            try
            {
                glBindTexture( GL_TEXTURE_2D, color_buffer );
                YAVSG_GL_THROW_FOR_ERRORS(
                    "couldn't bind texture "
                    + std::to_string( color_buffer )
                    + " for yavsg::gl::framebuffer"
                );
                
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
                YAVSG_GL_THROW_FOR_ERRORS(
                    "couldn't allocate "
                    + std::to_string( width )
                    + "x"
                    + std::to_string( height )
                    + " texture "
                    + std::to_string( color_buffer )
                    + " for yavsg::gl::framebuffer"
                );
                glTexParameteri(
                    GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR
                );
                YAVSG_GL_THROW_FOR_ERRORS(
                    "couldn't set min filter for texture "
                    + std::to_string( color_buffer )
                    + " for yavsg::gl::framebuffer"
                );
                glTexParameteri(
                    GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR
                );
                YAVSG_GL_THROW_FOR_ERRORS(
                    "couldn't set mag filter for texture "
                    + std::to_string( color_buffer )
                    + " for yavsg::gl::framebuffer"
                );
                
                try
                {
                    glFramebufferTexture2D(
                        GL_FRAMEBUFFER,
                        GL_COLOR_ATTACHMENT0,   // Which attachment
                        GL_TEXTURE_2D,
                        color_buffer,
                        0                       // Mipmap level (not useful)
                    );
                    YAVSG_GL_THROW_FOR_ERRORS(
                        "couldn't attach texture "
                        + std::to_string( color_buffer )
                        + " to framebuffer "
                        + std::to_string( id )
                        + " for yavsg::gl::framebuffer"
                    );
                    
                    glGenRenderbuffers( 1, &depth_stencil_buffer );
                    YAVSG_GL_THROW_FOR_ERRORS(
                        "couldn't generate depth & stencil buffer for framebuffer "
                        + std::to_string( id )
                        + " for yavsg::gl::framebuffer"
                    );
                    glBindRenderbuffer( GL_RENDERBUFFER, depth_stencil_buffer );
                    YAVSG_GL_THROW_FOR_ERRORS(
                        "couldn't bind depth & stencil buffer "
                        + std::to_string( depth_stencil_buffer )
                        + " for framebuffer "
                        + std::to_string( id )
                        + " for yavsg::gl::framebuffer"
                    );
                    
                    glRenderbufferStorage(
                        GL_RENDERBUFFER,
                        GL_DEPTH24_STENCIL8,
                        width, height
                    );
                    YAVSG_GL_THROW_FOR_ERRORS(
                        "couldn't allocate "
                        + std::to_string( width )
                        + "x"
                        + std::to_string( height )
                        + " depth & stencil buffer "
                        + std::to_string( depth_stencil_buffer )
                        + " for framebuffer "
                        + std::to_string( id )
                        + " for yavsg::gl::framebuffer"
                    );
                    
                    glFramebufferRenderbuffer(
                        GL_FRAMEBUFFER,
                        GL_DEPTH_STENCIL_ATTACHMENT,
                        GL_RENDERBUFFER,
                        depth_stencil_buffer
                    );
                    YAVSG_GL_THROW_FOR_ERRORS(
                        "couldn't attach depth & stencil buffer "
                        + std::to_string( depth_stencil_buffer )
                        + " to framebuffer "
                        + std::to_string( id )
                        + " for yavsg::gl::framebuffer"
                    );
                    
                    if(
                        glCheckFramebufferStatus( GL_FRAMEBUFFER )
                        != GL_FRAMEBUFFER_COMPLETE
                    )
                        throw std::runtime_error(
                            "failed to complete famebuffer"
                        );
                }
                catch( ... )
                {
                    glDeleteRenderbuffers( 1, &depth_stencil_buffer );
                    throw;
                }
            }
            catch( ... )
            {
                glDeleteTextures( 1, &color_buffer );
                throw;
            }
        }
        catch( ... )
        {
            glDeleteFramebuffers( 1, &id );
            throw;
        }
    }
    
    framebuffer::~framebuffer()
    {
        glDeleteFramebuffers( 1, &id );
        glDeleteTextures( 1, &color_buffer );
        glDeleteRenderbuffers( 1, &depth_stencil_buffer );
    }
    
    void framebuffer::bind()
    {
        glBindFramebuffer( GL_FRAMEBUFFER, id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind framebuffer "
            + std::to_string( id )
            + " for yavsg::gl::framebuffer"
        );
    }
} }


namespace yavsg { namespace gl
{
    void bind_default_framebuffer()
    {
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind default framebuffer"
        );
    }
} }
