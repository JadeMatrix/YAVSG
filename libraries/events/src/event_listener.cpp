#include <yavsg/events/event_listener.hpp>

#include <yavsg/sdl/sdl.hpp>
#include <yavsg/tasking/task.hpp>
#include <yavsg/tasking/tasking.hpp>
#include <yavsg/tasking/utility_tasks.hpp>

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
        std::pair< \
            std::function< void( const EVENT& ) >, \
            yavsg::task_flags_type \
        > \
    > MAPNAME;
    
    DEFINE_LISTENER_MAP_FOR( SDL_CommonEvent          ,            common_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_WindowEvent          ,            window_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_KeyboardEvent        ,          keyboard_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_TextEditingEvent     ,         text_edit_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_TextInputEvent       ,        text_input_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_MouseMotionEvent     ,      mouse_motion_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_MouseButtonEvent     ,      mouse_button_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_MouseWheelEvent      ,            scroll_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_JoyAxisEvent         ,          joy_axis_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_JoyBallEvent         ,          joy_ball_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_JoyHatEvent          ,           joy_hat_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_JoyButtonEvent       ,        joy_button_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_JoyDeviceEvent       ,        joy_device_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_ControllerAxisEvent  ,   controller_axis_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_ControllerButtonEvent, controller_button_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_ControllerDeviceEvent, controller_device_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_AudioDeviceEvent     ,      audio_device_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_QuitEvent            ,              quit_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_UserEvent            ,        user_event_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_SysWMEvent           ,            sys_wm_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_TouchFingerEvent     ,             touch_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_MultiGestureEvent    ,           gesture_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_DollarGestureEvent   ,    dollar_gesture_listeners )
    DEFINE_LISTENER_MAP_FOR( SDL_DropEvent            ,              drop_listeners )
    
    #undef DEFINE_LISTENER_MAP_FOR
}


namespace // Event consumer task ///////////////////////////////////////////////
{
    #define DEFINE_SUBMIT_EVENT_CALLBACK_TASK( EVENT_MEMBER ) \
    { \
        auto callback = callback_pair.second.first; \
        auto event    = window_event.EVENT_MEMBER; \
        yavsg::submit_task( std::make_unique< yavsg::callback_task >( \
            [ callback, event ](){ \
                callback( event ); \
                return false; \
            }, \
            callback_pair.second.second \
        ) ); \
    }
    
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
            SDL_Event window_event;
            while( SDL_PollEvent( &window_event ) )
            {
                switch( window_event.type )
                {
                case SDL_WINDOWEVENT:
                    for( auto& callback_pair : window_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( window )
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    for( auto& callback_pair : keyboard_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( key )
                    break;
                case SDL_TEXTEDITING:
                    for( auto& callback_pair : text_edit_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( edit )
                    break;
                case SDL_TEXTINPUT:
                    for( auto& callback_pair : text_input_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( text )
                    break;
                case SDL_MOUSEMOTION:
                    for( auto& callback_pair : mouse_motion_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( motion )
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    for( auto& callback_pair : mouse_button_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( button )
                    break;
                case SDL_MOUSEWHEEL:
                    for( auto& callback_pair : scroll_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( wheel )
                    break;
                case SDL_JOYAXISMOTION:
                    for( auto& callback_pair : joy_axis_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( jaxis )
                    break;
                case SDL_JOYBALLMOTION:
                    for( auto& callback_pair : joy_ball_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( jball )
                    break;
                case SDL_JOYHATMOTION:
                    for( auto& callback_pair : joy_hat_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( jhat )
                    break;
                case SDL_JOYBUTTONDOWN:
                case SDL_JOYBUTTONUP:
                    for( auto& callback_pair : joy_button_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( jbutton )
                    break;
                case SDL_JOYDEVICEADDED:
                case SDL_JOYDEVICEREMOVED:
                    for( auto& callback_pair : joy_device_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( jdevice )
                    break;
                case SDL_CONTROLLERAXISMOTION:
                    for( auto& callback_pair : controller_axis_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( caxis )
                    break;
                case SDL_CONTROLLERBUTTONDOWN:
                case SDL_CONTROLLERBUTTONUP:
                    for( auto& callback_pair : controller_button_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( cbutton )
                    break;
                case SDL_CONTROLLERDEVICEADDED:
                case SDL_CONTROLLERDEVICEREMOVED:
                case SDL_CONTROLLERDEVICEREMAPPED:
                    for( auto& callback_pair : controller_device_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( cdevice )
                    break;
                case SDL_AUDIODEVICEADDED:
                case SDL_AUDIODEVICEREMOVED:
                    for( auto& callback_pair : audio_device_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( adevice )
                    break;
                case SDL_QUIT:
                    for( auto& callback_pair : quit_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( quit )
                    break;
                case SDL_USEREVENT:
                    for( auto& callback_pair : user_event_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( user )
                    break;
                case SDL_SYSWMEVENT:
                    for( auto& callback_pair : sys_wm_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( syswm )
                    break;
                case SDL_FINGERDOWN:
                case SDL_FINGERUP:
                case SDL_FINGERMOTION:
                    for( auto& callback_pair : touch_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( tfinger )
                    break;
                case SDL_MULTIGESTURE:
                    for( auto& callback_pair : gesture_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( mgesture )
                    break;
                case SDL_DOLLARGESTURE:
                case SDL_DOLLARRECORD:
                    for( auto& callback_pair : dollar_gesture_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( dgesture )
                    break;
                case SDL_DROPFILE:
                case SDL_DROPTEXT:
                case SDL_DROPBEGIN:
                case SDL_DROPCOMPLETE:
                    for( auto& callback_pair : drop_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( drop )
                    break;
                // case SDL_APP_TERMINATING:
                // case SDL_APP_LOWMEMORY:
                // case SDL_APP_WILLENTERBACKGROUND:
                // case SDL_APP_DIDENTERBACKGROUND:
                // case SDL_APP_WILLENTERFOREGROUND:
                // case SDL_APP_DIDENTERFOREGROUND:
                //     // Mobile-only, ignore for now
                // case SDL_KEYMAPCHANGED:
                // case SDL_CLIPBOARDUPDATE:
                // case SDL_RENDER_TARGETS_RESET:
                // case SDL_RENDER_DEVICE_RESET:
                //     // Common
                default:
                    for( auto& callback_pair : common_listeners )
                        DEFINE_SUBMIT_EVENT_CALLBACK_TASK( common )
                    break;
                }
            }
            
            return true;
        }
    };
    
    #undef DEFINE_SUBMIT_EVENT_CALLBACK_TASK
}


namespace yavsg // Listener constructor implementations ////////////////////////
{
    #define DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( EVENT, MAPNAME ) \
    template<> \
    event_listener< EVENT >::event_listener( \
        std::function< void( const EVENT& ) > callback, \
        task_flags_type flags \
    ) \
    { \
        std::call_once( \
            poll_events_task_submitted_flag, \
            [](){ \
                submit_task( std::make_unique< poll_events_task >() ); \
            } \
        ); \
        std::unique_lock< std::mutex > _lock( listeners_mutex ); \
        _id = next_listener_id++; \
        MAPNAME[ _id ] = { callback, flags }; \
    }
    
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_CommonEvent          ,            common_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_WindowEvent          ,            window_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_KeyboardEvent        ,          keyboard_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_TextEditingEvent     ,         text_edit_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_TextInputEvent       ,        text_input_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_MouseMotionEvent     ,      mouse_motion_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_MouseButtonEvent     ,      mouse_button_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_MouseWheelEvent      ,            scroll_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_JoyAxisEvent         ,          joy_axis_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_JoyBallEvent         ,          joy_ball_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_JoyHatEvent          ,           joy_hat_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_JoyButtonEvent       ,        joy_button_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_JoyDeviceEvent       ,        joy_device_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_ControllerAxisEvent  ,   controller_axis_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_ControllerButtonEvent, controller_button_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_ControllerDeviceEvent, controller_device_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_AudioDeviceEvent     ,      audio_device_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_QuitEvent            ,              quit_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_UserEvent            ,        user_event_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_SysWMEvent           ,            sys_wm_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_TouchFingerEvent     ,             touch_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_MultiGestureEvent    ,           gesture_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_DollarGestureEvent   ,    dollar_gesture_listeners )
    DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR( SDL_DropEvent            ,              drop_listeners )
    
    #undef DEFINE_LISTENER_CONSTRUCTOR_OVERLOAD_FOR
}


namespace yavsg // Listener destructor implementations /////////////////////////
{
    #define DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( EVENT, MAPNAME ) \
    template<> \
    event_listener< EVENT >::~event_listener() \
    { \
        std::unique_lock< std::mutex > lock( listeners_mutex ); \
        MAPNAME.erase( _id ); \
    }
    
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_CommonEvent          ,            common_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_WindowEvent          ,            window_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_KeyboardEvent        ,          keyboard_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_TextEditingEvent     ,         text_edit_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_TextInputEvent       ,        text_input_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_MouseMotionEvent     ,      mouse_motion_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_MouseButtonEvent     ,      mouse_button_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_MouseWheelEvent      ,            scroll_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_JoyAxisEvent         ,          joy_axis_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_JoyBallEvent         ,          joy_ball_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_JoyHatEvent          ,           joy_hat_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_JoyButtonEvent       ,        joy_button_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_JoyDeviceEvent       ,        joy_device_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_ControllerAxisEvent  ,   controller_axis_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_ControllerButtonEvent, controller_button_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_ControllerDeviceEvent, controller_device_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_AudioDeviceEvent     ,      audio_device_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_QuitEvent            ,              quit_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_UserEvent            ,        user_event_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_SysWMEvent           ,            sys_wm_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_TouchFingerEvent     ,             touch_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_MultiGestureEvent    ,           gesture_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_DollarGestureEvent   ,    dollar_gesture_listeners )
    DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR( SDL_DropEvent            ,              drop_listeners )
    
    #undef DEFINE_LISTENER_DESTRUCTOR_OVERLOAD_FOR
}
