#pragma once
#ifndef YAVSG_GL_BUFFER_HPP
#define YAVSG_GL_BUFFER_HPP


#include <yavsg/gl_wrap.hpp>

#include <exception>
#include <ostream>
#include <string>
#include <vector>


namespace yavsg { namespace gl
{
    class summary_error : public std::runtime_error
    {
    public:
        const std::vector< GLenum > error_codes;
        
        summary_error(
            const std::string& message,
            const std::vector< GLenum >& codes_in
        ) :
            std::runtime_error( message ),
            error_codes( codes_in )
        {}
    };
    
    // Pulls out any & all OpenGL errors and throws a summary exception if there
    // were any.  This is a macro so MESSAGE_EXPRESSION is only evaluated if
    // there's an error.
    #define YAVSG_GL_THROW_FOR_ERRORS( MESSAGE_EXPRESSION ) \
    { \
        GLenum gl_error_code = glGetError(); \
        if( gl_error_code != GL_NO_ERROR ) \
        { \
            std::vector< GLenum > error_codes{ gl_error_code }; \
             \
            while( ( gl_error_code = glGetError() ) != GL_NO_ERROR ) \
                error_codes.push_back( gl_error_code ); \
             \
            throw yavsg::gl::summary_error( \
                ( MESSAGE_EXPRESSION ), \
                error_codes \
            ); \
        } \
    }
    
    // Utility function for use in `catch()` blocks
    void print_summary_error_codes( std::ostream&, const summary_error& );
} }


#endif
