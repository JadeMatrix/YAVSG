#pragma once
#ifndef YAVSG_SDL_UTILS_HPP
#define YAVSG_SDL_UTILS_HPP


// Must be included before SDL2/SDL_opengl.h for OpenGL versioning
#include "../gl/_gl_base.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <string>


namespace yavsg
{
    class SDL_manager
    {
    public:
        SDL_manager();
        ~SDL_manager();
    };
    
    class SDL_window_manager
    {
    public:
        SDL_Window*   sdl_window;
        SDL_GLContext gl_context;
        
        SDL_window_manager(
            const std::string& title,
            int x,
            int y,
            int w,
            int h,
            Uint32 flags
        );
        ~SDL_window_manager();
    };
}


#endif
