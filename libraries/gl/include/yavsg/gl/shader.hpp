#pragma once
#ifndef YAVSG_GL_SHADER_HPP
#define YAVSG_GL_SHADER_HPP


#include <yavsg/gl_wrap.hpp>

#include <string>


namespace yavsg { namespace gl
{
    class shader
    {
    public:
        GLuint id;
        
        shader( GLenum shader_type, const std::string& source );
        ~shader();
        
        static shader from_file(
            GLenum shader_type,
            const std::string& filename
        );
    };
} }


#endif
