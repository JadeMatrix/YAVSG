#include <yavsg/windowsys/window.hpp>

#include <yavsg/windowsys/frame.hpp>
#include <yavsg/tasking/task.hpp>
#include <yavsg/tasking/tasking.hpp>

#include <cmath>        // std::isnan()
#include <exception>    // std::runtime_error, std::invalid_argument
#include <iostream>     // std::cerr
#include <limits>       // std::numeric_limits
#include <sstream>      // std::stringstream


namespace // OpenGL initialization utilities ///////////////////////////////////
{
    std::once_flag opengl_extensions_initialized_flag;
    
    void ensure_opengl_extensions_initialized()
    {
        std::call_once(
            opengl_extensions_initialized_flag,
            [](){
            #ifndef __APPLE__
                glewExperimental = GL_TRUE;
                if( glewInit() != GLEW_OK )
                    throw std::runtime_error( "failed to initialize GLEW" );
            #endif
            }
        );
    }
}


namespace // Window dimension utilities ////////////////////////////////////////
{
    static const std::size_t default_window_width  =  1280;
    static const std::size_t default_window_height =   720;
    static const std::size_t sdl_max_window_size   = 16384;
    static const std::size_t     min_window_size   =     1;
    
    bool clamp_window_dimensions( JadeMatrix::yavsg::window_state& state )
    {
        bool clamped = false;
        
        struct dim_info
        {
            JadeMatrix::yavsg::dpi::points< float >& value;
            std::size_t          default_value;
        };
        
        for( auto& info : {
            dim_info{ state.width , default_window_width  },
            dim_info{ state.height, default_window_height }
        } )
            if( !(
                   info.value >= min_window_size
                && info.value <= sdl_max_window_size
            ) )
            {
                if( info.value < min_window_size )
                    info.value = min_window_size;
                else if( info.value > sdl_max_window_size )
                    info.value = sdl_max_window_size;
                else // NaN
                    info.value = info.default_value;
                
                clamped = true;
            }
        
        return clamped;
    }
    
    std::string window_state_repr(
        const JadeMatrix::yavsg::window_state& state
    )
    {
        std::stringstream ss;
        
        ss
            << "window \""
            << state.title
            << "\" "
            << state.width
            << "×"
            << state.height
            << " @ "
            << state.x
            << ","
            << state.y
            << "; scale="
            << state.scale_factor
            << "; "
            << ( state.resizable  ? "resizable" : "fixed size" )
            << "; "
            << ( state.has_border ? "bordered"  : "borderless" )
            << "; arrangement: "
        ;
        
        switch( state.arranged )
        {
        case JadeMatrix::yavsg::window_arrange_state::INVALID:
            ss << "invalid";
            break;
        case JadeMatrix::yavsg::window_arrange_state::NORMAL:
            ss << "normal";
            break;
        case JadeMatrix::yavsg::window_arrange_state::FULLSCREEN_NATIVE:
            ss << "native fullscreen";
            break;
        case JadeMatrix::yavsg::window_arrange_state::FULLSCREEN_CUSTOM:
            ss << "custom fullscreen";
            break;
        case JadeMatrix::yavsg::window_arrange_state::MINIMIZED:
            ss << "minimized";
            break;
        case JadeMatrix::yavsg::window_arrange_state::HIDDEN:
            ss << "hidden";
            break;
        }
        
        return ss.str();
    }
}


namespace JadeMatrix::yavsg // Window update task //////////////////////////////
{
    using update_window_flags_type = std::size_t;
    namespace update_window_flag
    {
        static const update_window_flags_type NONE     = 0x00;
        static const update_window_flags_type CENTER   = 0x01 << 0;
        static const update_window_flags_type MAXIMIZE = 0x01 << 1;
        static const update_window_flags_type FRONT    = 0x01 << 2;
    }
    
    class update_window_task : public task
    {
    protected:
        std::shared_ptr< window_reference > target_reference;
        window_state             new_state;
        update_window_flags_type update_flags;
        
    public:
        update_window_task(
            std::shared_ptr< window_reference > r,
            const window_state&      state,
            update_window_flags_type f = update_window_flag::NONE
        ) :
            target_reference{ r     },
            new_state{        state },
            update_flags{     f     }
        {}
        
        virtual task_flags_type flags() const
        {
            return task_flag::MAIN_THREAD;
        }
        
        virtual bool operator()()
        {
            // Artificial scope because the frame task (possibly submitted at
            // the end of the function) also needs to lock the reference mutex
            {
                std::unique_lock< std::mutex > ref_lock(
                    target_reference -> reference_mutex
                );
                if( !target_reference -> window )
                    // Window is closed
                    return false;
                
                std::unique_lock< std::mutex > window_lock(
                    target_reference -> window -> state_mutex
                );
                
                using arng_state = window_arrange_state;
                
                if( new_state.arranged == arng_state::INVALID )
                    throw std::invalid_argument(
                        "cannot update window to an invalid arrangement"
                    );
                
                auto requested_width  = new_state.width;
                auto requested_height = new_state.height;
                if( clamp_window_dimensions( new_state ) )
                    std::cerr
                        << "requested window width,height "
                        << requested_width
                        << ","
                        << requested_height
                        << " was clamped to "
                        << new_state.width
                        << ","
                        << new_state.height
                        << std::endl
                    ;
                
                int x, y;
                if( update_flags & update_window_flag::CENTER )
                {
                    x = y = SDL_WINDOWPOS_CENTERED;
                    std::cerr
                        << "requested window x,y ignored because centering"
                        << std::endl
                    ;
                }
                else
                {
                    if( new_state.x > 0 )
                        x = static_cast< int >( new_state.x );
                    else
                        x = SDL_WINDOWPOS_UNDEFINED;
                    
                    if( new_state.y > 0 )
                        y = static_cast< int >( new_state.y );
                    else
                        y = SDL_WINDOWPOS_UNDEFINED;
                }
                
                if( !target_reference -> window -> sdl_window )
                {
                    Uint32 sdl_window_flags = SDL_WINDOW_OPENGL;
                    
                    if( new_state.scale_factor > 1 )
                        sdl_window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
                    
                    if( new_state.resizable )
                        sdl_window_flags |= SDL_WINDOW_RESIZABLE;
                    
                    if( !new_state.has_border )
                        sdl_window_flags |= SDL_WINDOW_BORDERLESS;
                    
                    if( new_state.arranged == arng_state::HIDDEN )
                        sdl_window_flags |= SDL_WINDOW_HIDDEN;
                    else
                    {
                        sdl_window_flags |= SDL_WINDOW_SHOWN;
                        
                        if(
                               new_state.arranged == arng_state::FULLSCREEN_NATIVE
                            || new_state.arranged == arng_state::FULLSCREEN_CUSTOM
                        )
                            sdl_window_flags |= SDL_WINDOW_FULLSCREEN;
                    }
                    
                    target_reference -> window -> sdl_window = SDL_CreateWindow(
                        new_state.title.c_str(),
                        x,
                        y,
                        static_cast< int >( new_state.width  ),
                        static_cast< int >( new_state.height ),
                        sdl_window_flags
                    );
                    if( target_reference -> window -> sdl_window == nullptr )
                        throw std::runtime_error(
                            "couldn't create SDL2 window: "
                            + std::string{ SDL_GetError() }
                        );
                    
                    target_reference -> window -> gl_context = SDL_GL_CreateContext(
                        target_reference -> window -> sdl_window
                    );
                    if( target_reference -> window -> gl_context == nullptr )
                    {
                        std::string context_error_string{
                            "failed to create OpenGL context via SDL2 window: "
                        };
                        context_error_string += SDL_GetError();
                        SDL_DestroyWindow(
                            target_reference -> window -> sdl_window
                        );
                        throw std::runtime_error( context_error_string );
                    }
                    
                    // Run GLEW stuff _after_ creating SDL/GL context
                    ensure_opengl_extensions_initialized();
                }
                else
                {
                    if( new_state.arranged == arng_state::HIDDEN )
                        SDL_HideWindow(
                            target_reference -> window -> sdl_window
                        );
                    else
                    {
                        SDL_ShowWindow(
                            target_reference -> window -> sdl_window
                        );
                        
                        if(
                               new_state.arranged == arng_state::FULLSCREEN_NATIVE
                            || new_state.arranged == arng_state::FULLSCREEN_CUSTOM
                        )
                        {
                            if( SDL_SetWindowFullscreen(
                                target_reference -> window -> sdl_window,
                                SDL_WINDOW_FULLSCREEN
                            ) )
                                throw std::runtime_error(
                                    "couldn't set SDL2 window fullscreen: "
                                    + std::string{ SDL_GetError() }
                                );
                        }
                        else
                        {
                            if( SDL_SetWindowFullscreen(
                                target_reference -> window -> sdl_window,
                                0x00
                            ) )
                                throw std::runtime_error(
                                    "couldn't set SDL2 window windowed: "
                                    + std::string{ SDL_GetError() }
                                );
                        }
                    }
                    
                    SDL_SetWindowResizable(
                        target_reference -> window -> sdl_window,
                        static_cast< SDL_bool >( new_state.resizable )
                    );
                    
                    SDL_SetWindowBordered(
                        target_reference -> window -> sdl_window,
                        static_cast< SDL_bool >( new_state.has_border )
                    );
                    
                    SDL_SetWindowTitle(
                        target_reference -> window -> sdl_window,
                        new_state.title.c_str()
                    );
                    SDL_SetWindowPosition(
                        target_reference -> window -> sdl_window,
                        x,
                        y
                    );
                    SDL_SetWindowSize(
                        target_reference -> window -> sdl_window,
                        static_cast< int >( new_state.width  ),
                        static_cast< int >( new_state.height )
                    );
                }
                
                if( update_flags & update_window_flag::MAXIMIZE )
                    SDL_MaximizeWindow(
                        target_reference -> window -> sdl_window
                    );
                
                if( update_flags & update_window_flag::FRONT )
                    SDL_RaiseWindow( target_reference -> window -> sdl_window );
                
                switch( new_state.arranged )
                {
                case arng_state::INVALID: // Already handled
                case arng_state::HIDDEN:  // Already handled
                case arng_state::NORMAL:  // Nothing to do
                    break;
                case arng_state::FULLSCREEN_NATIVE:
                    {
                        auto display_index = SDL_GetWindowDisplayIndex(
                            target_reference -> window -> sdl_window
                        );
                        if( display_index < 1 )
                            throw std::runtime_error(
                                "couldn't get SDL2 window display index: "
                                + std::string{ SDL_GetError() }
                            );
                        
                        SDL_DisplayMode mode;
                        
                        if( SDL_GetDisplayMode( display_index, 0, &mode ) )
                            throw std::runtime_error(
                                "couldn't get SDL2 display mode for display "
                                + std::to_string( display_index )
                                + ": "
                                + std::string{ SDL_GetError() }
                            );
                        
                        if( SDL_SetWindowDisplayMode(
                            target_reference -> window -> sdl_window,
                            &mode
                        ) )
                            throw std::runtime_error(
                                "couldn't set SDL2 window display mode to "
                                "native mode: "
                                + std::string{ SDL_GetError() }
                            );
                    }
                    break;
                case arng_state::FULLSCREEN_CUSTOM:
                    {
                        SDL_DisplayMode mode;
                        
                        if( SDL_GetWindowDisplayMode(
                            target_reference -> window -> sdl_window,
                            &mode
                        ) )
                            throw std::runtime_error(
                                "couldn't get SDL2 window display mode: "
                                + std::string{ SDL_GetError() }
                            );
                        
                        mode.w = static_cast< int >( new_state.width  );
                        mode.h = static_cast< int >( new_state.height );
                        
                        if( SDL_SetWindowDisplayMode(
                            target_reference -> window -> sdl_window,
                            &mode
                        ) )
                            throw std::runtime_error(
                                "couldn't set SDL2 window display mode to "
                                + std::to_string( mode.w )
                                + "x"
                                + std::to_string( mode.h )
                                + ": "
                                + std::string{ SDL_GetError() }
                            );
                    }
                    break;
                case arng_state::MINIMIZED:
                    SDL_MinimizeWindow( target_reference -> window -> sdl_window );
                    break;
                }
                
                if(
                    !std::isnan( static_cast< float >(
                        target_reference -> window -> current_state.scale_factor
                    ) ) && (
                        new_state.scale_factor
                        != target_reference -> window -> current_state.scale_factor
                    )
                )
                    std::cerr << (
                        "ignoring change of window scale factor to "
                        + std::to_string( static_cast< float >(
                            new_state.scale_factor
                        ) )
                        + "\n"
                    );
                
                // Dont' need to call the window's `update_internal_state()` as
                // the above SDL calls will trigger `SDL_WindowEvent`s that will
                // be handled by the window's state change listener.  The update
                // won't be immediate but it should be fast enough for most
                // applications (except for tight loops checking the window
                // state, which are probably a bad idea for several reasons).
            }
            
            std::call_once(
                target_reference -> window -> frame_task_submit_flag,
                [ target_reference = this -> target_reference ](){
                    submit_task( std::make_unique< yavsg::frame_task >(
                        target_reference
                    ) );
                }
            );
            
            return false;
        }
    };
}


namespace // Window cleaup task ////////////////////////////////////////////////
{
    class cleanup_window_task : public JadeMatrix::yavsg::task
    {
    protected:
        SDL_Window*   sdl_window;
        SDL_GLContext gl_context;
        
    public:
        cleanup_window_task( SDL_Window* w, SDL_GLContext g ) :
            sdl_window{ w },
            gl_context{ g }
        {}
        
        virtual JadeMatrix::yavsg::task_flags_type flags() const
        {
            return JadeMatrix::yavsg::task_flag::MAIN_THREAD;
        }
        
        virtual bool operator()()
        {
            SDL_GL_DeleteContext( gl_context );
            SDL_DestroyWindow( sdl_window );
            
            return false;
        }
    };
}


namespace JadeMatrix::yavsg // Window implementation ///////////////////////////
{
    // NOT thread-safe
    void window::update_internal_state()
    {
        if( !sdl_window )
            return;
        
        int width, height, x, y;
        
        SDL_GetWindowSize(     sdl_window, &width, &height );
        SDL_GetWindowPosition( sdl_window, &x    , &y      );
        
        current_state.width  = width;
        current_state.height = height;
        current_state.x      = x;
        current_state.y      = y;
        
        auto sdl_window_flags = SDL_GetWindowFlags( sdl_window );
        
        if(
               sdl_window_flags & SDL_WINDOW_FULLSCREEN
            || sdl_window_flags & SDL_WINDOW_FULLSCREEN_DESKTOP
        )
        {
            auto display_index = SDL_GetWindowDisplayIndex( sdl_window );
            if( display_index < 1 )
                throw std::runtime_error(
                    "couldn't get SDL2 window display index: "
                    + std::string{ SDL_GetError() }
                );
            
            SDL_DisplayMode mode;
            
            if( SDL_GetDisplayMode( display_index, 0, &mode ) )
                throw std::runtime_error(
                    "couldn't get SDL2 display mode for display "
                    + std::to_string( display_index )
                    + ": "
                    + std::string{ SDL_GetError() }
                );
            
            if(
                   mode.w == current_state.width
                && mode.h == current_state.height
            )
                current_state.arranged = window_arrange_state::FULLSCREEN_NATIVE;
            else
                current_state.arranged = window_arrange_state::FULLSCREEN_CUSTOM;
        }
        else if( sdl_window_flags & SDL_WINDOW_MINIMIZED )
            current_state.arranged = window_arrange_state::MINIMIZED;
        else if( sdl_window_flags & SDL_WINDOW_HIDDEN )
            current_state.arranged = window_arrange_state::HIDDEN;
        else
            current_state.arranged = window_arrange_state::NORMAL;
        
        current_state.title = SDL_GetWindowTitle( sdl_window );
        
        // Detect window pixel/point scale factor; see
        // https://wiki.libsdl.org/SDL_CreateWindow
        int pixel_width, pixel_height;
        if(
               current_state.arranged == window_arrange_state::FULLSCREEN_NATIVE
            || current_state.arranged == window_arrange_state::FULLSCREEN_CUSTOM
        )
        {
            SDL_DisplayMode mode;
            if( SDL_GetWindowDisplayMode( sdl_window, &mode ) )
                throw std::runtime_error(
                    "couldn't get SDL2 window display mode: "
                    + std::string{ SDL_GetError() }
                );
            
            pixel_width  = mode.w;
            pixel_height = mode.h;
        }
        else
            SDL_GL_GetDrawableSize( sdl_window, &pixel_width, &pixel_height );
        
        // TODO: This doesn't allow setting the window scale factor independent
        // of the operating system; there's no "real" way to do this, but if
        // application by convention uses the window point sizes scaled to
        // pixels it can be faked.  This will then need to be overridden by any
        // requested scale factor, which with this current setup there's no way
        // to get.
        current_state.scale_factor = (
              ( static_cast< float >( pixel_width  ) / static_cast< float >( width  ) )
            + ( static_cast< float >( pixel_height ) / static_cast< float >( height ) )
        ) / 2.0f;
        
        current_state.resizable = static_cast< bool >(
            sdl_window_flags & SDL_WINDOW_RESIZABLE
        );
        
        current_state.has_border = !static_cast< bool >(
            sdl_window_flags & SDL_WINDOW_BORDERLESS
        );
        
        // Can't use `std::make_unique<>()` because `write_only_framebuffer`'s
        // constructor is protected & friended only with `SDL_window_manager`
        _default_framebuffer = std::unique_ptr< gl::write_only_framebuffer > (
            new gl::write_only_framebuffer(
                0,  // Framebuffer ID
                1,  // Color targets
                static_cast< std::size_t >(
                    to_pixels( current_state.width , current_state )
                ),
                static_cast< std::size_t >(
                    to_pixels( current_state.height, current_state )
                )
            )
        );
    }
    
    window::window( const window_state& init_state ) :
        sdl_window{           nullptr },
        _default_framebuffer{ nullptr },
        self_reference{ new window_reference{ .window = this } },
        current_state{
            std::numeric_limits< float >::signaling_NaN(),
            std::numeric_limits< float >::signaling_NaN(),
            std::numeric_limits< float >::signaling_NaN(),
            std::numeric_limits< float >::signaling_NaN(),
            window_arrange_state::INVALID,
            "",
            std::numeric_limits< float >::signaling_NaN(),
            false,
            false
        },
        state_change_listener{
            [ self_reference = this -> self_reference ](
                const SDL_WindowEvent& e
            ){
                std::unique_lock< std::mutex > ref_lock(
                    self_reference -> reference_mutex
                );
                
                if( self_reference -> window )
                {
                    std::unique_lock< std::mutex > window_lock(
                        self_reference -> window -> state_mutex
                    );
                    if(
                        self_reference -> window -> sdl_window
                        && e.windowID == SDL_GetWindowID(
                            self_reference -> window -> sdl_window
                        )
                    )
                        // May be triggered by own update task
                        self_reference -> window -> update_internal_state();
                }
            },
            task_flag::MAIN_THREAD
        }
    {
        submit_task( std::make_unique< update_window_task >(
            self_reference,
            init_state,
            update_window_flag::CENTER | update_window_flag::FRONT
        ) );
    }
    
    window::~window()
    {
        // Any window system tasks or event callbacks pointing to this window
        // now know it's closed.
        std::unique_lock< std::mutex > ref_lock(
            self_reference -> reference_mutex
        );
        self_reference -> window = nullptr;
        
        submit_task( std::make_unique< cleanup_window_task >(
            sdl_window,
            gl_context
        ) );
    }
    
    window_state window::state() const
    {
        std::unique_lock< std::mutex > lock(
            const_cast< std::mutex& >( state_mutex )
        );
        return current_state;
    }
    
    void window::state( const window_state& new_state )
    {
        submit_task( std::make_unique< update_window_task >(
            self_reference,
            new_state
        ) );
    }
    
    void window::center()
    {
        std::unique_lock< std::mutex > lock( state_mutex );
        
        submit_task( std::make_unique< update_window_task >(
            self_reference,
            current_state,
            update_window_flag::CENTER
        ) );
    }
    
    void window::maximize()
    {
        std::unique_lock< std::mutex > lock( state_mutex );
        
        submit_task( std::make_unique< update_window_task >(
            self_reference,
            current_state,
            update_window_flag::MAXIMIZE
        ) );
    }
    
    std::shared_ptr< window_reference > window::reference()
    {
        return self_reference;
    }
    
    gl::write_only_framebuffer& window::default_framebuffer()
    {
        std::unique_lock< std::mutex > lock( state_mutex );
        return *_default_framebuffer;
    }
}
