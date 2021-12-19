#pragma once


#include <yavsg/gl_wrap.hpp>

#include <filesystem>
#include <string>


namespace JadeMatrix::yavsg::gl
{
    class shader
    {
    public:
        GLuint id;
        
        shader( GLenum shader_type, std::string const& source );
        ~shader();
        
        static shader from_file(
            GLenum                       shader_type,
            std::filesystem::path const& filename
        );
    };
}
