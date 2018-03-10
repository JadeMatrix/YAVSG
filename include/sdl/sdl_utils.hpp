#pragma once
#ifndef YAVSG_SDL_UTILS_HPP
#define YAVSG_SDL_UTILS_HPP


#include "_sdl_base.hpp"
#include "../gl/framebuffer.hpp"
#include "../gl/texture.hpp"

#include <string>
#include <utility>  // std::size_t


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
        using default_framebuffer_type = yavsg::gl::write_only_framebuffer<
            yavsg::gl::texture< GLfloat, 3 >
        >;
        
    protected:
        default_framebuffer_type* _default_framebuffer;
        
    public:
        SDL_Window*   sdl_window;
        SDL_GLContext gl_context;
        
        SDL_window_manager(
            const std::string& title,
            std::size_t x,
            std::size_t y,
            std::size_t w,
            std::size_t h,
            Uint32 flags
        );
        ~SDL_window_manager();
        
        default_framebuffer_type& default_framebuffer();
    };
}


#endif
