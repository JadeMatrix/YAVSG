#include <yavsg/gl/error.hpp>

#include <yavsg/sdl/sdl.hpp>    // GL_STACK_OVERFLOW, GL_STACK_UNDERFLOW,
                                //  GL_TABLE_TOO_LARGE

#include <ostream>
#include <string_view>


namespace
{
    using namespace std::string_view_literals;
}


void JadeMatrix::yavsg::gl::print_summary_error_codes(
    std::ostream       & out,
    summary_error const& e
)
{
    if( !e.error_codes.empty() )
    {
        for( auto code : e.error_codes )
        {
            out << "  "sv;
            switch( code )
            {
            case GL_INVALID_ENUM:
                out << "GL_INVALID_ENUM"sv;
                break;
            case GL_INVALID_VALUE:
                out << "GL_INVALID_VALUE"sv;
                break;
            case GL_INVALID_OPERATION:
                out << "GL_INVALID_OPERATION"sv;
                break;
            case GL_STACK_OVERFLOW:
                out << "GL_STACK_OVERFLOW"sv;
                break;
            case GL_STACK_UNDERFLOW:
                out << "GL_STACK_UNDERFLOW"sv;
                break;
            case GL_OUT_OF_MEMORY:
                out << "GL_OUT_OF_MEMORY"sv;
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                out << "GL_INVALID_FRAMEBUFFER_OPERATION"sv;
                break;
            case 0x0507: // GL_CONTEXT_LOST available in 4.5+
                out << "GL_CONTEXT_LOST"sv;
                break;
            case GL_TABLE_TOO_LARGE:
                out << "GL_TABLE_TOO_LARGE"sv;
                break;
            default:
                out << std::to_string( code );
                break;
            }
            out << '\n';
        }
    }
    else
    {
        out << "  (none)\n"sv;
    }
}
