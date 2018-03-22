#pragma once
#ifndef YAVSG_WINDOWSYS_WINDOW_HPP
#define YAVSG_WINDOWSYS_WINDOW_HPP


#include "../events/event_listener.hpp"
#include "../gl/framebuffer.hpp"
#include "../sdl/_sdl_base.hpp"
#include "../units/dpi.hpp"
#include "../units/unit.hpp"    // yavsg::ratio

#include <memory>               // std::unique_ptr, std::shared_ptr
#include <mutex>
#include <string>
#include <utility>              // std::size_t


namespace yavsg
{
    enum class window_arrange_state
    {
        INVALID,
        NORMAL,
        FULLSCREEN_NATIVE,
        FULLSCREEN_CUSTOM,
        MINIMIZED,
        HIDDEN
    };
    
    struct window_state
    {
        dpi::points< float > width;
        dpi::points< float > height;
        dpi::points< float > x;
        dpi::points< float > y;
        window_arrange_state arranged;
        std::string title;
        ratio< float > scale_factor;
        bool resizable;
        
        // TODO: border
    };
    
    // Window reference utility type for concurrent window updates
    class window;
    struct window_reference
    {
        std::mutex  reference_mutex;
        window*     window;
    };
    
    class update_window_task;
    
    class window
    {
        friend class update_window_task;
        
    protected:
        SDL_Window*   sdl_window;
        SDL_GLContext gl_context;
        
        std::shared_ptr< window_reference > self_reference;
        
        std::mutex   state_mutex;
        window_state current_state;
        
        std::unique_ptr< gl::write_only_framebuffer > _default_framebuffer;
        
        event_listener< SDL_WindowEvent > state_change_listener;
        
        void update_internal_state();
        
    public:
        window( const window_state& );
        ~window();
        
        window_state state() const;
        void         state( const window_state& );
        
        void center();
        
        gl::write_only_framebuffer& default_framebuffer();
    };
}


namespace yavsg { namespace dpi
{
    template< typename T > pixels< T > to_pixels(
        const points< T >   pt,
        const window_state& state
    )
    {
        return static_cast< T >( pt ) * static_cast< T >( state.scale_factor );
    }
    
    template< typename T > points< T > to_points(
        const pixels< T >   px,
        const window_state& state
    )
    {
        return static_cast< T >( px ) / static_cast< T >( state.scale_factor );
    }
} }


#endif
