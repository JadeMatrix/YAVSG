#pragma once


#include <yavsg/events/event_listener.hpp>
#include <yavsg/gl/framebuffer.hpp>
#include <yavsg/rendering/scene.hpp>
#include <yavsg/sdl/sdl.hpp>
#include <yavsg/units/dpi.hpp>
#include <yavsg/units/unit.hpp> // ratio

#include <cstddef>  // size_t
#include <memory>   // unique_ptr, shared_ptr
#include <mutex>    // mutex, once_flag, call_once
#include <string>


namespace JadeMatrix::yavsg
{
    enum class window_arrange_state
    {
        invalid,
        normal,
        fullscreen_native,
        fullscreen_custom,
        minimized,
        hidden
    };
    
    struct window_state
    {
        dpi::points< float > width;
        dpi::points< float > height;
        dpi::points< float > x;
        dpi::points< float > y;
        window_arrange_state arranged;
        std::string          title;
        ratio< float >       scale_factor;
        bool                 resizable;
        bool                 has_border;
    };
    
    // Window reference utility type for concurrent window updates
    class window;
    struct window_reference
    {
        std::mutex reference_mutex;
        window   * window;
    };
    
    class frame_task;
    
    class window
    {
        friend class frame_task;
        
    public:
        window( window_state const& );
        ~window();
        
        window_state state() const;
        void         state( window_state const& );
        
        void center();
        void maximize();
        
        std::shared_ptr< window_reference >   reference();
        gl::write_only_framebuffer& default_framebuffer();
        
        scene main_scene;
        
    protected:
        class update_task;
        class cleanup_task;
        
        SDL_Window  * sdl_window_;
        SDL_GLContext gl_context_;
        
        std::shared_ptr< window_reference > self_reference_;
        
        std::mutex mutable state_mutex_;
        window_state       current_state_;
        
        std::unique_ptr< gl::write_only_framebuffer > default_framebuffer_;
        
        event_listener< SDL_WindowEvent > state_change_listener_;
        
        // TODO: use frame timer instead
        std::once_flag frame_task_submit_flag_;
        
        void update_internal_state();
    };
}


namespace JadeMatrix::yavsg::dpi
{
    template< typename T > pixels< T > to_pixels(
        points< T >  const  pt,
        window_state const& state
    )
    {
        return static_cast< T >( pt ) * static_cast< T >( state.scale_factor );
    }
    
    template< typename T > points< T > to_points(
        pixels< T >  const  px,
        window_state const& state
    )
    {
        return static_cast< T >( px ) / static_cast< T >( state.scale_factor );
    }
}
