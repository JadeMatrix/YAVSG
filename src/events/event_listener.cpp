#include "../../include/events/event_listener.hpp"

#include "../../include/sdl/_sdl_base.hpp"
#include "../../include/tasking/task.hpp"
#include "../../include/tasking/tasking.hpp"
#include "../../include/tasking/utility_tasks.hpp"

#include <map>
#include <mutex>    // std::mutex, std::once_flag, std::call_once()


namespace
{
    std::once_flag poll_events_task_submitted_flag;
    
    std::mutex listeners_mutex;
    yavsg::listener_id next_listener_id = 1;
    
    #define DEFINE_LISTENER_MAP_FOR( EVENT, MAPNAME ) \
    std::map< \
        yavsg::listener_id, \
        std::function< void( const EVENT& ) > \
    > MAPNAME;
    
    DEFINE_LISTENER_MAP_FOR( SDL_KeyboardEvent   , keyboard_event_listeners     )
    DEFINE_LISTENER_MAP_FOR( SDL_MouseMotionEvent, mouse_motion_event_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_MouseWheelEvent , scroll_event_listeners       )
    
    #undef DEFINE_LISTENER_MAP_FOR
}


namespace // Event consumer task ///////////////////////////////////////////////
{
    class poll_events_task : public yavsg::task
    {
    public:
        virtual yavsg::task_flags_type flags() const
        {
            // SDL needs to poll events on the main thread
            return yavsg::task_flag::MAIN_THREAD;
        }
        
        virtual bool operator()()
        {
            bool should_quit = false;
            
            // TODO: submit callback tasks
            
            SDL_Event window_event;
            while( SDL_PollEvent( &window_event ) )
                switch( window_event.type )
                {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    for( auto& callback_pair : keyboard_event_listeners )
                        callback_pair.second( window_event.key );
                    break;
                case SDL_MOUSEMOTION:
                    for( auto& callback_pair : mouse_motion_event_listeners )
                        callback_pair.second( window_event.motion );
                    break;
                case SDL_MOUSEWHEEL:
                    for( auto& callback_pair : scroll_event_listeners )
                        callback_pair.second( window_event.wheel );
                    break;
                case SDL_QUIT:
                    should_quit = true;
                    break;
                default:
                    break;
                }
            
            if( should_quit )
            {
                // std::cout << "quitting..." << std::endl;
                yavsg::submit_task(
                    std::make_unique< yavsg::stop_task_system_task >()
                );
            }
            
            return !should_quit;
        }
    };
}


namespace yavsg // Listener constructor implementations ////////////////////////
{
    #define DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( EVENT, MAPNAME ) \
    template<> \
    event_listener< EVENT >::event_listener( \
        std::function< void( const EVENT& ) > callback \
    ) \
    { \
        std::call_once( \
            poll_events_task_submitted_flag, \
            [](){ \
                submit_task( std::make_unique< poll_events_task >() ); \
            } \
        ); \
        std::unique_lock< std::mutex > _lock( listeners_mutex ); \
        auto _id = next_listener_id++; \
        MAPNAME[ _id ] = callback; \
    }
    
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_KeyboardEvent   , keyboard_event_listeners     )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_MouseMotionEvent, mouse_motion_event_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_MouseWheelEvent , scroll_event_listeners       )
    
    #undef DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR
}


namespace yavsg // Listener destructor implementations /////////////////////////
{
    #define DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( EVENT, MAPNAME ) \
    template<> \
    event_listener< EVENT >::~event_listener() \
    { \
        MAPNAME.erase( _id ); \
    }
    
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_KeyboardEvent,    keyboard_event_listeners     )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_MouseMotionEvent, mouse_motion_event_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_MouseWheelEvent , scroll_event_listeners       )
    
    #undef DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR
}
