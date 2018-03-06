#pragma once
#ifndef YAVSG_SDL_UTILS_HPP
#define YAVSG_SDL_UTILS_HPP


#include "_sdl_base.hpp"
#include "../gl/framebuffer.hpp"

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
        
        yavsg::gl::base_framebuffer default_framebuffer;
        
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
