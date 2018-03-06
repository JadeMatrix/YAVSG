#include "../../include/gl/framebuffer.hpp"

#include <exception>


namespace yavsg { namespace gl // Base framebuffer implementation //////////////
{
    base_framebuffer::base_framebuffer(
        GLuint gl_id,
        std::size_t width,
        std::size_t height
    ) :
        gl_id(   gl_id  ),
        _width(  width  ),
        _height( height )
    {}
    
    std::size_t base_framebuffer::width() const
    {
        return _width;
    }
    
    std::size_t base_framebuffer::height() const
    {
        return _height;
    }
    
    void base_framebuffer::bind()
    {
        glBindFramebuffer( GL_FRAMEBUFFER, gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind framebuffer "
            + std::to_string( gl_id )
            + std::string( gl_id == 0 ? " (default)" : "" )
            + " for yavsg::gl::base_framebuffer::bind()"
        );
    }
    
    void base_framebuffer::alpha_blending( alpha_blend_mode mode )
    {
        // Use a switch to get warnings about possible unhandled cases in the
        // future
        switch( mode )
        {
        case alpha_blend_mode::DISABLED:
            glDisablei( GL_BLEND, gl_id );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't disable blending for framebuffer "
                + std::to_string( gl_id )
                + std::string( gl_id == 0 ? " (default)" : "" )
                + " for yavsg::gl::base_framebuffer::alpha_blending()"
            );
            break;
        case alpha_blend_mode::PREMULTIPLIED:
        case alpha_blend_mode::BASIC:
        case alpha_blend_mode::PASSTHROUGH:
            {
                glEnablei( GL_BLEND, gl_id );
                YAVSG_GL_THROW_FOR_ERRORS(
                    "couldn't enable blending for framebuffer "
                    + std::to_string( gl_id )
                    + std::string( gl_id == 0 ? " (default)" : "" )
                    + " for yavsg::gl::base_framebuffer::alpha_blending()"
                );
                
                glBlendEquationSeparate( GL_FUNC_ADD, GL_FUNC_ADD );
                YAVSG_GL_THROW_FOR_ERRORS(
                    "couldn't set separate blending equations for framebuffer "
                    + std::to_string( gl_id )
                    + std::string( gl_id == 0 ? " (default)" : "" )
                    + " for yavsg::gl::base_framebuffer::alpha_blending()"
                );
                
                switch( mode )
                {
                case alpha_blend_mode::DISABLED:
                    // Shouldn't really be possible, but makes the compiler shut
                    // up without disabling the warning for this code
                    throw std::logic_error(
                        "impossible blending state reached in "
                        "yavsg::gl::base_framebuffer::alpha_blending()"
                    );
                case alpha_blend_mode::PREMULTIPLIED:
                    glBlendFuncSeparate(
                        GL_ONE,
                        GL_ONE_MINUS_SRC_ALPHA,
                        GL_ONE,
                        GL_ONE_MINUS_SRC_ALPHA
                    );
                    break;
                case alpha_blend_mode::BASIC:
                    glBlendFuncSeparate(
                        GL_SRC_ALPHA,
                        GL_ONE_MINUS_SRC_ALPHA,
                        GL_ONE,
                        GL_ONE_MINUS_SRC_ALPHA
                    );
                    break;
                case alpha_blend_mode::PASSTHROUGH:
                    glBlendFuncSeparate(
                        GL_SRC_ALPHA,
                        GL_ONE_MINUS_SRC_ALPHA,
                        GL_ONE,
                        GL_ZERO
                    );
                    break;
                }
                YAVSG_GL_THROW_FOR_ERRORS(
                    "couldn't set separate blending functions for framebuffer "
                    + std::to_string( gl_id )
                    + std::string( gl_id == 0 ? " (default)" : "" )
                    + " for yavsg::gl::base_framebuffer::alpha_blending()"
                );
            }
            break;
        }
    }
} }
