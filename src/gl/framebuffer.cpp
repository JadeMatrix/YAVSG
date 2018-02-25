#include "../../include/gl/framebuffer.hpp"


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
