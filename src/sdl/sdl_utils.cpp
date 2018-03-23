#include "../../include/sdl/sdl_utils.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

#include <exception>
#include <string>


namespace yavsg
{
    SDL_manager::SDL_manager()
    {
        if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
            throw std::runtime_error(
                "unable to initialize SDL2: "
                + std::string( SDL_GetError() )
            );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3                           );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2                           );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_CORE );
        SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE         , 8                           );
        
        int img_flags_in  = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
        int img_flags_out = IMG_Init( img_flags_in );
        if( img_flags_in != img_flags_out )
        {
            std::string img_error_string =
                "failed to initialize SDL2-image ";
            if( img_flags_out & IMG_INIT_JPG )
                img_error_string += "JPG";
            else if( img_flags_out & IMG_INIT_PNG )
                img_error_string += "PNG";
            else if( img_flags_out & IMG_INIT_TIF )
                img_error_string += "TIF";
            img_error_string += (
                " support: "
                + std::string( IMG_GetError() )
                + " (note IMG error string is not always meaningful when IMG_Init() fails)"
            );
            SDL_Quit();
            throw std::runtime_error( img_error_string.c_str() );
        }
    }
    
    SDL_manager::~SDL_manager()
    {
        IMG_Quit();
        SDL_Quit();
    }
}
