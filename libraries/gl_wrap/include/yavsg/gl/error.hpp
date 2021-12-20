#pragma once


#include <yavsg/gl_wrap.hpp>

#include <iosfwd>   // ostream (fwd)
#include <string>
#include <utility>  // move
#include <vector>


namespace JadeMatrix::yavsg::gl
{
    class summary_error : public error
    {
    public:
        std::vector< GLenum > const error_codes;
        
        summary_error(
            std::string    const& message,
            std::vector< GLenum > codes_in
        ) :
            error( message ),
            error_codes( std::move( codes_in ) )
        {}
    };
    
    // Pulls out any & all OpenGL errors and throws a summary exception if there
    // were any.  This is a macro so MESSAGE_EXPRESSION is only evaluated if
    // there's an error.
    #define YAVSG_GL_THROW_FOR_ERRORS( MESSAGE_EXPRESSION ) \
    { \
        GLenum gl_error_code = gl::GetError(); \
        if( gl_error_code != GL_NO_ERROR ) \
        { \
            std::vector< GLenum > error_codes{ gl_error_code }; \
             \
            while( ( gl_error_code = gl::GetError() ) != GL_NO_ERROR ) \
            { \
                error_codes.push_back( gl_error_code ); \
            } \
             \
            throw yavsg::gl::summary_error( \
                ( MESSAGE_EXPRESSION ), \
                std::move( error_codes ) \
            ); \
        } \
    }
    
    // Utility function for use in `catch()` blocks
    void print_summary_error_codes( std::ostream&, summary_error const& );
}
