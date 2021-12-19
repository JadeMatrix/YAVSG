#include <yavsg/windowsys/window.hpp>

#include <yavsg/logging.hpp>
#include <yavsg/windowsys/frame.hpp>
#include <yavsg/tasking/task.hpp>
#include <yavsg/tasking/tasking.hpp>

#include <fmt/format.h>

#include <cmath>        // isnan
#include <limits>
#include <sstream>
#include <stdexcept>    // runtime_error, invalid_argument
#include <tuple>        // tie


namespace
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;
    
    auto const log_ = JadeMatrix::yavsg::log_handle();
}


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
    constexpr std::size_t default_window_width  =  1280;
    constexpr std::size_t default_window_height =   720;
    constexpr std::size_t sdl_max_window_size   = 16384;
    constexpr std::size_t     min_window_size   =     1;
    
    bool clamp_window_dimensions( JadeMatrix::yavsg::window_state& state )
    {
        bool clamped = false;
        
        for( auto& [ value, default_value ] : {
            std::tie( state.width , default_window_width  ),
            std::tie( state.height, default_window_height )
        } )
        {
            // This condition will pass NaNs
            if( !( value >= min_window_size && value <= sdl_max_window_size ) )
            {
                if( value < min_window_size )
                {
                    value = min_window_size;
                }
                else if( value > sdl_max_window_size )
                {
                    value = sdl_max_window_size;
                }
                else // NaN
                {
                    value = default_value;
                }
                
                clamped = true;
            }
        }
        
        return clamped;
    }
    
    // TODO: Rewrite or remove
    [[maybe_unused]] std::string window_state_repr(
        JadeMatrix::yavsg::window_state const& state
    )
    {
        std::stringstream ss;
        
        ss
            << "window \""sv
            << state.title
            << "\" "sv
            << state.width
            << "×"sv
            << state.height
            << " @ "sv
            << state.x
            << ","sv
            << state.y
            << "; scale="sv
            << state.scale_factor
            << "; "sv
            << ( state.resizable  ? "resizable"sv : "fixed size"sv )
            << "; "sv
            << ( state.has_border ? "bordered"sv  : "borderless"sv )
            << "; arrangement: "sv
        ;
        
        switch( state.arranged )
        {
        case JadeMatrix::yavsg::window_arrange_state::invalid:
            ss << "invalid"sv;
            break;
        case JadeMatrix::yavsg::window_arrange_state::normal:
            ss << "normal"sv;
            break;
        case JadeMatrix::yavsg::window_arrange_state::fullscreen_native:
            ss << "native fullscreen"sv;
            break;
        case JadeMatrix::yavsg::window_arrange_state::fullscreen_custom:
            ss << "custom fullscreen"sv;
            break;
        case JadeMatrix::yavsg::window_arrange_state::minimized:
            ss << "minimized"sv;
            break;
        case JadeMatrix::yavsg::window_arrange_state::hidden:
            ss << "hidden"sv;
            break;
        }
        
        return ss.str();
    }
}


// Internal window tasks definitions ///////////////////////////////////////////

namespace
{
    using update_window_flags_type = std::size_t;
    namespace update_window_flag
    {
        constexpr update_window_flags_type none     = 0x00;
        constexpr update_window_flags_type center   = 0x01 << 0;
        constexpr update_window_flags_type maximize = 0x01 << 1;
        constexpr update_window_flags_type front    = 0x01 << 2;
    }
}

class JadeMatrix::yavsg::window::update_task : public task
{
public:
    update_task(
        std::shared_ptr< window_reference > r,
        window_state                 const& state,
        update_window_flags_type            f = update_window_flag::none
    );
    task_flags_type flags() const override;
    bool operator()() override;
    
private:
    std::shared_ptr< window_reference > target_reference_;
    window_state                        new_state_;
    update_window_flags_type            update_flags_;
};

class JadeMatrix::yavsg::window::cleanup_task : public task
{
public:
    cleanup_task( SDL_Window* w, SDL_GLContext g );
    task_flags_type flags() const override;
    bool operator()() override;

private:
    SDL_Window  * sdl_window_;
    SDL_GLContext gl_context_;
};


// Update task implementation //////////////////////////////////////////////////

JadeMatrix::yavsg::window::update_task::update_task(
    std::shared_ptr< window_reference > r,
    window_state                 const& state,
    update_window_flags_type            f
) :
    target_reference_{ r     },
    new_state_       { state },
    update_flags_    { f     }
{}

JadeMatrix::yavsg::task_flags_type
JadeMatrix::yavsg::window::update_task::flags() const
{
    return task_flag::main_thread;
}

bool JadeMatrix::yavsg::window::update_task::operator()()
{
    // Artificial scope because the frame task (possibly submitted at the end of
    // the function) also needs to lock the reference mutex
    {
        std::unique_lock ref_lock( target_reference_->reference_mutex );
        if( !target_reference_->window )
        {
            // Window is closed
            return false;
        }
        
        std::unique_lock window_lock( target_reference_->window->state_mutex_ );
        
        using arng_state = window_arrange_state;
        
        if( new_state_.arranged == arng_state::invalid )
        {
            throw std::invalid_argument(
                "cannot update window to an invalid arrangement"s
            );
        }
        
        auto requested_width  = new_state_.width;
        auto requested_height = new_state_.height;
        if( clamp_window_dimensions( new_state_ ) )
        {
            log_.verbose(
                "Requested window width,height {},{} was clamped to {},{}"sv,
                static_cast< float >( requested_width   ),
                static_cast< float >( requested_height  ),
                static_cast< float >( new_state_.width  ),
                static_cast< float >( new_state_.height )
            );
        }
        
        int x, y;
        if( update_flags_ & update_window_flag::center )
        {
            x = y = SDL_WINDOWPOS_CENTERED;
            log_.warning( "Requested window x,y ignored because centering"sv );
        }
        else
        {
            if( new_state_.x > 0 )
            {
                x = static_cast< int >( new_state_.x );
            }
            else
            {
                x = SDL_WINDOWPOS_UNDEFINED;
            }
            
            if( new_state_.y > 0 )
            {
                y = static_cast< int >( new_state_.y );
            }
            else
            {
                y = SDL_WINDOWPOS_UNDEFINED;
            }
        }
        
        if( !target_reference_->window->sdl_window_ )
        {
            Uint32 sdl_window_flags = SDL_WINDOW_OPENGL;
            
            if( new_state_.scale_factor > 1 )
            {
                sdl_window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
            }
            
            if( new_state_.resizable )
            {
                sdl_window_flags |= SDL_WINDOW_RESIZABLE;
            }
            
            if( !new_state_.has_border )
            {
                sdl_window_flags |= SDL_WINDOW_BORDERLESS;
            }
            
            if( new_state_.arranged == arng_state::hidden )
            {
                sdl_window_flags |= SDL_WINDOW_HIDDEN;
            }
            else
            {
                sdl_window_flags |= SDL_WINDOW_SHOWN;
                
                if(
                       new_state_.arranged == arng_state::fullscreen_native
                    || new_state_.arranged == arng_state::fullscreen_custom
                )
                {
                    sdl_window_flags |= SDL_WINDOW_FULLSCREEN;
                }
            }
            
            target_reference_->window->sdl_window_ = SDL_CreateWindow(
                new_state_.title.c_str(),
                x,
                y,
                static_cast< int >( new_state_.width  ),
                static_cast< int >( new_state_.height ),
                sdl_window_flags
            );
            if( target_reference_->window->sdl_window_ == nullptr )
            {
                throw std::runtime_error( fmt::format(
                    "couldn't create SDL2 window: {}"sv,
                    SDL_GetError()
                ) );
            }
            
            target_reference_->window->gl_context_ = SDL_GL_CreateContext(
                target_reference_->window->sdl_window_
            );
            if( target_reference_->window->gl_context_ == nullptr )
            {
                auto context_error_string = fmt::format(
                    "failed to create OpenGL context via SDL2 window: {}"sv,
                    SDL_GetError()
                );
                SDL_DestroyWindow( target_reference_->window->sdl_window_ );
                throw std::runtime_error( context_error_string );
            }
            
            // Run GLEW stuff _after_ creating SDL/GL context
            ensure_opengl_extensions_initialized();
        }
        else
        {
            if( new_state_.arranged == arng_state::hidden )
            {
                SDL_HideWindow( target_reference_->window->sdl_window_ );
            }
            else
            {
                SDL_ShowWindow( target_reference_->window->sdl_window_ );
                
                if(
                       new_state_.arranged == arng_state::fullscreen_native
                    || new_state_.arranged == arng_state::fullscreen_custom
                )
                {
                    if( SDL_SetWindowFullscreen(
                        target_reference_->window->sdl_window_,
                        SDL_WINDOW_FULLSCREEN
                    ) )
                    {
                        throw std::runtime_error( fmt::format(
                            "couldn't set SDL2 window fullscreen: {}"sv,
                            SDL_GetError()
                        ) );
                    }
                }
                else
                {
                    if( SDL_SetWindowFullscreen(
                        target_reference_->window->sdl_window_,
                        0x00
                    ) )
                    {
                        
                        throw std::runtime_error( fmt::format(
                            "couldn't set SDL2 window windowed: {}"sv,
                            SDL_GetError()
                        ) );
                    }
                }
            }
            
            SDL_SetWindowResizable(
                target_reference_->window->sdl_window_,
                static_cast< SDL_bool >( new_state_.resizable )
            );
            
            SDL_SetWindowBordered(
                target_reference_->window->sdl_window_,
                static_cast< SDL_bool >( new_state_.has_border )
            );
            
            SDL_SetWindowTitle(
                target_reference_->window->sdl_window_,
                new_state_.title.c_str()
            );
            SDL_SetWindowPosition(
                target_reference_->window->sdl_window_,
                x,
                y
            );
            SDL_SetWindowSize(
                target_reference_->window->sdl_window_,
                static_cast< int >( new_state_.width  ),
                static_cast< int >( new_state_.height )
            );
        }
        
        if( update_flags_ & update_window_flag::maximize )
        {
            SDL_MaximizeWindow( target_reference_->window->sdl_window_ );
        }
        
        if( update_flags_ & update_window_flag::front )
        {
            SDL_RaiseWindow( target_reference_->window->sdl_window_ );
        }
        
        switch( new_state_.arranged )
        {
        case arng_state::invalid: // Already handled
        case arng_state::hidden:  // Already handled
        case arng_state::normal:  // Nothing to do
            break;
        case arng_state::fullscreen_native:
            {
                auto display_index = SDL_GetWindowDisplayIndex(
                    target_reference_->window->sdl_window_
                );
                if( display_index < 1 )
                {
                    throw std::runtime_error( fmt::format(
                        "couldn't get SDL2 window display index: {}"sv,
                        SDL_GetError()
                    ) );
                }
                
                SDL_DisplayMode mode;
                
                if( SDL_GetDisplayMode( display_index, 0, &mode ) )
                {
                    throw std::runtime_error( fmt::format(
                        "couldn't get SDL2 display mode for display {}: {}"sv,
                        display_index,
                        SDL_GetError()
                    ) );
                }
                
                if( SDL_SetWindowDisplayMode(
                    target_reference_->window->sdl_window_,
                    &mode
                ) )
                {
                    throw std::runtime_error( fmt::format(
                        "couldn't set SDL2 window display mode to native mode: "
                            "{}"sv,
                        SDL_GetError()
                    ) );
                }
            }
            break;
        case arng_state::fullscreen_custom:
            {
                SDL_DisplayMode mode;
                
                if( SDL_GetWindowDisplayMode(
                    target_reference_->window->sdl_window_,
                    &mode
                ) )
                {
                    throw std::runtime_error( fmt::format(
                        "couldn't get SDL2 window display mode: {}"sv,
                        SDL_GetError()
                    ) );
                }
                
                mode.w = static_cast< int >( new_state_.width  );
                mode.h = static_cast< int >( new_state_.height );
                
                if( SDL_SetWindowDisplayMode(
                    target_reference_->window->sdl_window_,
                    &mode
                ) )
                {
                    throw std::runtime_error( fmt::format(
                        "couldn't set SDL2 window display mode to {}×{}: {}"sv,
                        mode.w,
                        mode.h,
                        SDL_GetError()
                    ) );
                }
            }
            break;
        case arng_state::minimized:
            SDL_MinimizeWindow( target_reference_->window->sdl_window_ );
            break;
        }
        
        if(
            !std::isnan( static_cast< float >(
                target_reference_->window->current_state_.scale_factor
            ) ) && (
                new_state_.scale_factor
                != target_reference_->window->current_state_.scale_factor
            )
        )
        {
            log_.warning(
                "Ignoring change of window scale factor to {}"sv,
                static_cast< float >( new_state_.scale_factor )
            );
        }
        
        // Dont' need to call the window's `update_internal_state()` as the
        // above SDL calls will trigger `SDL_WindowEvent`s that will be handled
        // by the window's state change listener.  The update won't be immediate
        // but it should be fast enough for most applications (except for tight
        // loops checking the window state, which are probably a bad idea for
        // several reasons).
    }
    
    std::call_once(
        target_reference_->window->frame_task_submit_flag_,
        [ target_reference_ = this->target_reference_ ](){
            submit_task( std::make_unique< yavsg::frame_task >(
                target_reference_
            ) );
        }
    );
    
    return false;
}


// Cleanup task implementation /////////////////////////////////////////////////

JadeMatrix::yavsg::window::cleanup_task::cleanup_task(
    SDL_Window  * w,
    SDL_GLContext g
) : sdl_window_{ w }, gl_context_{ g }
{}

JadeMatrix::yavsg::task_flags_type
JadeMatrix::yavsg::window::cleanup_task::flags() const
{
    return JadeMatrix::yavsg::task_flag::main_thread;
}

bool JadeMatrix::yavsg::window::cleanup_task::operator()()
{
    SDL_GL_DeleteContext( gl_context_ );
    SDL_DestroyWindow( sdl_window_ );
    
    return false;
}


// Window implementation ///////////////////////////////////////////////////////

JadeMatrix::yavsg::window::window( window_state const& init_state ) :
    sdl_window_{ nullptr },
    self_reference_{ new window_reference{ {}, this } },
    current_state_{
        std::numeric_limits< float >::signaling_NaN(),
        std::numeric_limits< float >::signaling_NaN(),
        std::numeric_limits< float >::signaling_NaN(),
        std::numeric_limits< float >::signaling_NaN(),
        window_arrange_state::invalid,
        ""s,
        std::numeric_limits< float >::signaling_NaN(),
        false,
        false
    },
    default_framebuffer_{ nullptr },
    state_change_listener_{
        [ self_reference = this->self_reference_ ]( SDL_WindowEvent const& e ){
            std::unique_lock ref_lock( self_reference->reference_mutex );
            
            if( self_reference->window )
            {
                std::unique_lock window_lock(
                    self_reference->window->state_mutex_
                );
                if(
                    self_reference->window->sdl_window_
                    && e.windowID == SDL_GetWindowID(
                        self_reference->window->sdl_window_
                    )
                )
                {
                    // May be triggered by own update task
                    self_reference->window->update_internal_state();
                }
            }
        },
        task_flag::main_thread
    }
{
    submit_task( std::make_unique< update_task >(
        self_reference_,
        init_state,
        update_window_flag::center | update_window_flag::front
    ) );
}

JadeMatrix::yavsg::window::~window()
{
    // Any window system tasks or event callbacks pointing to this window
    // now know it's closed.
    std::unique_lock ref_lock( self_reference_->reference_mutex );
    self_reference_->window = nullptr;
    
    submit_task( std::make_unique< cleanup_task >( sdl_window_, gl_context_ ) );
}

JadeMatrix::yavsg::window_state JadeMatrix::yavsg::window::state() const
{
    std::unique_lock lock( state_mutex_ );
    return current_state_;
}

void JadeMatrix::yavsg::window::state( const window_state& new_state )
{
    submit_task( std::make_unique< update_task >(
        self_reference_,
        new_state
    ) );
}

void JadeMatrix::yavsg::window::center()
{
    std::unique_lock lock( state_mutex_ );
    
    submit_task( std::make_unique< update_task >(
        self_reference_,
        current_state_,
        update_window_flag::center
    ) );
}

void JadeMatrix::yavsg::window::maximize()
{
    std::unique_lock lock( state_mutex_ );
    
    submit_task( std::make_unique< update_task >(
        self_reference_,
        current_state_,
        update_window_flag::maximize
    ) );
}

std::shared_ptr< JadeMatrix::yavsg::window_reference >
JadeMatrix::yavsg::window::reference()
{
    return self_reference_;
}

JadeMatrix::yavsg::gl::write_only_framebuffer&
JadeMatrix::yavsg::window::default_framebuffer()
{
    std::unique_lock lock( state_mutex_ );
    return *default_framebuffer_;
}

// NOT thread-safe
void JadeMatrix::yavsg::window::update_internal_state()
{
    if( !sdl_window_ )
    {
        return;
    }
    
    int width, height, x, y;
    SDL_GetWindowSize    ( sdl_window_, &width, &height );
    SDL_GetWindowPosition( sdl_window_, &x    , &y      );
    
    current_state_.width  = width;
    current_state_.height = height;
    current_state_.x      = x;
    current_state_.y      = y;
    
    auto sdl_window_flags = SDL_GetWindowFlags( sdl_window_ );
    
    if(
           sdl_window_flags & SDL_WINDOW_FULLSCREEN
        || sdl_window_flags & SDL_WINDOW_FULLSCREEN_DESKTOP
    )
    {
        auto display_index = SDL_GetWindowDisplayIndex( sdl_window_ );
        if( display_index < 1 )
        {
            throw std::runtime_error( fmt::format(
                "couldn't get SDL2 window display index: {}"sv,
                SDL_GetError()
            ) );
        }
        
        SDL_DisplayMode mode;
        
        if( SDL_GetDisplayMode( display_index, 0, &mode ) )
        {
            throw std::runtime_error( fmt::format(
                "couldn't get SDL2 display mode for display {}: {}"sv,
                display_index,
                SDL_GetError()
            ) );
        }
        
        if(
               mode.w == current_state_.width
            && mode.h == current_state_.height
        )
        {
            current_state_.arranged = window_arrange_state::fullscreen_native;
        }
        else
        {
            current_state_.arranged = window_arrange_state::fullscreen_custom;
        }
    }
    else if( sdl_window_flags & SDL_WINDOW_MINIMIZED )
    {
        current_state_.arranged = window_arrange_state::minimized;
    }
    else if( sdl_window_flags & SDL_WINDOW_HIDDEN )
    {
        current_state_.arranged = window_arrange_state::hidden;
    }
    else
    {
        current_state_.arranged = window_arrange_state::normal;
    }
    
    current_state_.title = SDL_GetWindowTitle( sdl_window_ );
    
    // Detect window pixel/point scale factor; see
    // https://wiki.libsdl.org/SDL_CreateWindow
    int pixel_width, pixel_height;
    if(
           current_state_.arranged == window_arrange_state::fullscreen_native
        || current_state_.arranged == window_arrange_state::fullscreen_custom
    )
    {
        SDL_DisplayMode mode;
        if( SDL_GetWindowDisplayMode( sdl_window_, &mode ) )
        {
            throw std::runtime_error( fmt::format(
                "couldn't get SDL2 window display mode: {}"sv,
                SDL_GetError()
            ) );
        }
        
        pixel_width  = mode.w;
        pixel_height = mode.h;
    }
    else
    {
        SDL_GL_GetDrawableSize( sdl_window_, &pixel_width, &pixel_height );
    }
    
    // TODO: This doesn't allow setting the window scale factor independent
    // of the operating system; there's no "real" way to do this, but if
    // application by convention uses the window point sizes scaled to
    // pixels it can be faked.  This will then need to be overridden by any
    // requested scale factor, which with this current setup there's no way
    // to get.
    current_state_.scale_factor = (
          ( static_cast< float >( pixel_width  ) / static_cast< float >( width  ) )
        + ( static_cast< float >( pixel_height ) / static_cast< float >( height ) )
    ) / 2.0f;
    
    current_state_.resizable = static_cast< bool >(
        sdl_window_flags & SDL_WINDOW_RESIZABLE
    );
    
    current_state_.has_border = !static_cast< bool >(
        sdl_window_flags & SDL_WINDOW_BORDERLESS
    );
    
    // Can't use `std::make_unique<>()` because `write_only_framebuffer`'s
    // constructor is protected & friended only with `SDL_window_manager`
    default_framebuffer_ = std::unique_ptr< gl::write_only_framebuffer > (
        new gl::write_only_framebuffer(
            0,  // Framebuffer ID
            1,  // Color targets
            static_cast< std::size_t >(
                to_pixels( current_state_.width , current_state_ )
            ),
            static_cast< std::size_t >(
                to_pixels( current_state_.height, current_state_ )
            )
        )
    );
}
