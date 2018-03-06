#include "../../include/gl/framebuffer.hpp"


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
            + " for yavsg::gl::framebuffer::bind()"
        );
    }
} }
