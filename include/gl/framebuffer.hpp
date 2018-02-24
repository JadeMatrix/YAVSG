#pragma once
#ifndef YAVSG_GL_FRAMEBUFFER_HPP
#define YAVSG_GL_FRAMEBUFFER_HPP


#include "_gl_base.hpp"
#include "error.hpp"


namespace yavsg { namespace gl
{
    // TODO: template< unsigned int N > class framebuffer
    //      where N = number of render targets between 1 and OpenGL's max
    class framebuffer
    {
    public:
        GLuint id;
        GLuint color_buffer;
        GLuint depth_stencil_buffer;
        
        framebuffer(
            GLsizei width,
            GLsizei height
        );
        ~framebuffer();
        
        void bind();
    };
    
    void bind_default_framebuffer();
} }


#endif
