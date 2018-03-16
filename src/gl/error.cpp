#include "../../include/gl/error.hpp"

// GL_STACK_OVERFLOW, GL_STACK_UNDERFLOW, GL_TABLE_TOO_LARGE
#include "../../include/sdl/sdl_utils.hpp"


namespace yavsg { namespace gl
{
    void print_summary_error_codes( std::ostream& out, const summary_error& e )
    {
        if( e.error_codes.size() )
        {
            for( auto code : e.error_codes )
            {
                out << "  ";
                switch( code )
                {
                case GL_INVALID_ENUM:
                    out << "GL_INVALID_ENUM";
                    break;
                case GL_INVALID_VALUE:
                    out << "GL_INVALID_VALUE";
                    break;
                case GL_INVALID_OPERATION:
                    out << "GL_INVALID_OPERATION";
                    break;
                case GL_STACK_OVERFLOW:
                    out << "GL_STACK_OVERFLOW";
                    break;
                case GL_STACK_UNDERFLOW:
                    out << "GL_STACK_UNDERFLOW";
                    break;
                case GL_OUT_OF_MEMORY:
                    out << "GL_OUT_OF_MEMORY";
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    out << "GL_INVALID_FRAMEBUFFER_OPERATION";
                    break;
                case 0x0507: // GL_CONTEXT_LOST available in 4.5+
                    out << "GL_CONTEXT_LOST";
                    break;
                case GL_TABLE_TOO_LARGE:
                    out << "GL_TABLE_TOO_LARGE";
                    break;
                default:
                    out << std::to_string( code );
                    break;
                }
                out << std::endl;
            }
        }
        else
            out << "  (none)" << std::endl;
    }
} }
