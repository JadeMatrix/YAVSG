#include <yavsg/sdl/utils.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

#include <fmt/format.h>

#include <stdexcept>    // runtime_error
#include <string_view>


namespace
{
    using namespace std::string_view_literals;
}


JadeMatrix::yavsg::SDL_manager::SDL_manager()
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
    {
        throw std::runtime_error( fmt::format(
            "unable to initialize SDL2: {}"sv,
            SDL_GetError()
        ) );
    }
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3                           );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2                           );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE         , 8                           );
    
    auto const img_flags_in  = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    auto const img_flags_out = IMG_Init( img_flags_in );
    auto const img_flags_err = ( img_flags_in ^ img_flags_out ) & img_flags_in;
    if( img_flags_err )
    {
        auto const img_error_string = fmt::format(
            "failed to initialize SDL2-image {} support: {} (note IMG error "
                "string is not always meaningful when IMG_Init() fails)"sv,
            (
                  img_flags_err & IMG_INIT_JPG ? "JPG"sv
                : img_flags_err & IMG_INIT_PNG ? "PNG"sv
                : img_flags_err & IMG_INIT_TIF ? "TIF"sv
                : "?"sv
            ),
            IMG_GetError()
        );
        SDL_Quit();
        throw std::runtime_error( img_error_string );
    }
}

JadeMatrix::yavsg::SDL_manager::~SDL_manager()
{
    IMG_Quit();
    SDL_Quit();
}
