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
    
    // Utility function for use in `catch()` blocks
    void print_summary_error_codes( std::ostream&, summary_error const& );
}
