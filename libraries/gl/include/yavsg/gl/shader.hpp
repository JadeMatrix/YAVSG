#pragma once


#include <yavsg/gl_wrap.hpp>

#include <string>


namespace JadeMatrix::yavsg::gl
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
}
