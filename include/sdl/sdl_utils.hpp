#pragma once
#ifndef YAVSG_SDL_UTILS_HPP
#define YAVSG_SDL_UTILS_HPP


#include "_sdl_base.hpp"
#include "../gl/framebuffer.hpp"
#include "../events/event_listener.hpp"

#include <string>
#include <memory>   // std::unique_ptr
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
    protected:
        std::unique_ptr< gl::write_only_framebuffer > _default_framebuffer;
        event_listener< SDL_WindowEvent > window_change_listener;
        
        // Update internal state
        void resize( std::size_t w, std::size_t h );
        
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
        
        yavsg::gl::write_only_framebuffer& default_framebuffer();
    };
}


#endif
