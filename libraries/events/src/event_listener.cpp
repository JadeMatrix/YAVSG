#include <yavsg/events/event_listener.hpp>

#include <yavsg/sdl/sdl.hpp>
#include <yavsg/tasking/task.hpp>
#include <yavsg/tasking/tasking.hpp>
#include <yavsg/tasking/utility_tasks.hpp>

#include <doctest/doctest.h>    // REQUIRE

#include <map>
#include <mutex>    // mutex, once_flag, call_once


namespace
{
    std::once_flag poll_events_task_submitted_flag;
    
    std::mutex listeners_mutex;
    JadeMatrix::yavsg::listener_id next_listener_id = 1;
    
    #define DEFINE_LISTENER_MAP_FOR( EVENT, MAPNAME ) \
        std::map< \
            JadeMatrix::yavsg::listener_id, \
            std::pair< \
                std::function< void( SDL_##EVENT const& ) >, \
                JadeMatrix::yavsg::task_flags_type \
            > \
        > MAPNAME##_listeners;
    
    DEFINE_LISTENER_MAP_FOR( CommonEvent          , common            )
    DEFINE_LISTENER_MAP_FOR( WindowEvent          , window            )
    DEFINE_LISTENER_MAP_FOR( KeyboardEvent        , keyboard          )
    DEFINE_LISTENER_MAP_FOR( TextEditingEvent     , text_edit         )
    DEFINE_LISTENER_MAP_FOR( TextInputEvent       , text_input        )
    DEFINE_LISTENER_MAP_FOR( MouseMotionEvent     , mouse_motion      )
    DEFINE_LISTENER_MAP_FOR( MouseButtonEvent     , mouse_button      )
    DEFINE_LISTENER_MAP_FOR( MouseWheelEvent      , scroll            )
    DEFINE_LISTENER_MAP_FOR( JoyAxisEvent         , joy_axis          )
    DEFINE_LISTENER_MAP_FOR( JoyBallEvent         , joy_ball          )
    DEFINE_LISTENER_MAP_FOR( JoyHatEvent          , joy_hat           )
    DEFINE_LISTENER_MAP_FOR( JoyButtonEvent       , joy_button        )
    DEFINE_LISTENER_MAP_FOR( JoyDeviceEvent       , joy_device        )
    DEFINE_LISTENER_MAP_FOR( ControllerAxisEvent  , controller_axis   )
    DEFINE_LISTENER_MAP_FOR( ControllerButtonEvent, controller_button )
    DEFINE_LISTENER_MAP_FOR( ControllerDeviceEvent, controller_device )
    DEFINE_LISTENER_MAP_FOR( AudioDeviceEvent     , audio_device      )
    DEFINE_LISTENER_MAP_FOR( QuitEvent            , quit              )
    DEFINE_LISTENER_MAP_FOR( UserEvent            , user_event        )
    DEFINE_LISTENER_MAP_FOR( SysWMEvent           , sys_wm            )
    DEFINE_LISTENER_MAP_FOR( TouchFingerEvent     , touch             )
    DEFINE_LISTENER_MAP_FOR( MultiGestureEvent    , gesture           )
    DEFINE_LISTENER_MAP_FOR( DollarGestureEvent   , dollar_gesture    )
    DEFINE_LISTENER_MAP_FOR( DropEvent            , drop              )
    
    #undef DEFINE_LISTENER_MAP_FOR
}


namespace // Event consumer task ///////////////////////////////////////////////
{
    class poll_events_task : public JadeMatrix::yavsg::task
    {
    public:
        JadeMatrix::yavsg::task_flags_type flags() const override
        {
            // SDL needs to poll events on the main thread
            return JadeMatrix::yavsg::task_flag::MAIN_THREAD;
        }
        
        bool operator()() override
        {
            SDL_Event window_event;
            while( SDL_PollEvent( &window_event ) )
            {
                #define FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( \
                    MAPNAME, \
                    MEMBER \
                ) \
                    { \
                        std::unique_lock lock( listeners_mutex ); \
                        for( auto& callback_pair : MAPNAME##_listeners ) \
                        { \
                            JadeMatrix::yavsg::submit_task( \
                                std::make_unique< \
                                    JadeMatrix::yavsg::callback_task \
                                >( \
                                    [ \
                                        callback = callback_pair.second.first, \
                                        event    = window_event.MEMBER \
                                    ](){ \
                                        callback( event ); \
                                        return false; \
                                    }, \
                                    callback_pair.second.second \
                                ) \
                            ); \
                        } \
                        break; \
                    }
                
                switch( window_event.type )
                {
                case SDL_WINDOWEVENT                : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( window           , window   )
                case SDL_KEYDOWN                    :
                case SDL_KEYUP                      : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( keyboard         , key      )
                case SDL_TEXTEDITING                : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( text_edit        , edit     )
                case SDL_TEXTINPUT                  : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( text_input       , text     )
                case SDL_MOUSEMOTION                : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( mouse_motion     , motion   )
                case SDL_MOUSEBUTTONDOWN            :
                case SDL_MOUSEBUTTONUP              : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( mouse_button     , button   )
                case SDL_MOUSEWHEEL                 : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( scroll           , wheel    )
                case SDL_JOYAXISMOTION              : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( joy_axis         , jaxis    )
                case SDL_JOYBALLMOTION              : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( joy_ball         , jball    )
                case SDL_JOYHATMOTION               : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( joy_hat          , jhat     )
                case SDL_JOYBUTTONDOWN              :
                case SDL_JOYBUTTONUP                : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( joy_button       , jbutton  )
                case SDL_JOYDEVICEADDED             :
                case SDL_JOYDEVICEREMOVED           : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( joy_device       , jdevice  )
                case SDL_CONTROLLERAXISMOTION       : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( controller_axis  , caxis    )
                case SDL_CONTROLLERBUTTONDOWN       :
                case SDL_CONTROLLERBUTTONUP         : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( controller_button, cbutton  )
                case SDL_CONTROLLERDEVICEADDED      :
                case SDL_CONTROLLERDEVICEREMOVED    :
                case SDL_CONTROLLERDEVICEREMAPPED   : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( controller_device, cdevice  )
                case SDL_AUDIODEVICEADDED           :
                case SDL_AUDIODEVICEREMOVED         : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( audio_device     , adevice  )
                case SDL_QUIT                       : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( quit             , quit     )
                case SDL_USEREVENT                  : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( user_event       , user     )
                case SDL_SYSWMEVENT                 : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( sys_wm           , syswm    )
                case SDL_FINGERDOWN                 :
                case SDL_FINGERUP                   :
                case SDL_FINGERMOTION               : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( touch            , tfinger  )
                case SDL_MULTIGESTURE               : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( gesture          , mgesture )
                case SDL_DOLLARGESTURE              :
                case SDL_DOLLARRECORD               : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( dollar_gesture   , dgesture )
                case SDL_DROPFILE                   :
                case SDL_DROPTEXT                   :
                case SDL_DROPBEGIN                  :
                case SDL_DROPCOMPLETE               : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( drop             , drop     )
                    // Mobile-only, treat as "common" for now
                case SDL_APP_TERMINATING            :
                case SDL_APP_LOWMEMORY              :
                case SDL_APP_WILLENTERBACKGROUND    :
                case SDL_APP_DIDENTERBACKGROUND     :
                case SDL_APP_WILLENTERFOREGROUND    :
                case SDL_APP_DIDENTERFOREGROUND     :
                    // Treat as "common"
                case SDL_KEYMAPCHANGED              :
                case SDL_CLIPBOARDUPDATE            :
                case SDL_RENDER_TARGETS_RESET       :
                case SDL_RENDER_DEVICE_RESET        :
                default                             : FOREACH_LISTENER_SUBMIT_EVENT_CALLBACK_TASK( common           , common   )
                }
                
                #undef DEFINE_SUBMIT_EVENT_CALLBACK_TASK
            }
            
            return true;
        }
    };
}


// Listener constructor implementations ////////////////////////////////////////

#define DEFINE_LISTENER_CONSTRUCTOR_FOR( EVENT, MAPNAME ) \
    template<> JadeMatrix::yavsg::event_listener< \
        SDL_##EVENT \
    >::event_listener( \
        std::function< void( SDL_##EVENT const& ) > callback, \
        task_flags_type                             flags \
    ) \
    { \
        std::call_once( \
            poll_events_task_submitted_flag, \
            [](){ \
                submit_task( std::make_unique< poll_events_task >() ); \
            } \
        ); \
        std::unique_lock lock( listeners_mutex ); \
        id_ = next_listener_id++; \
        auto const [ iter, inserted ] = MAPNAME##_listeners.emplace( \
            id_, \
            std::pair{ \
                std::move( callback ), \
                flags \
            } \
        ); \
        ( void )iter; \
        REQUIRE( inserted ); \
    }

DEFINE_LISTENER_CONSTRUCTOR_FOR( CommonEvent          , common            )
DEFINE_LISTENER_CONSTRUCTOR_FOR( WindowEvent          , window            )
DEFINE_LISTENER_CONSTRUCTOR_FOR( KeyboardEvent        , keyboard          )
DEFINE_LISTENER_CONSTRUCTOR_FOR( TextEditingEvent     , text_edit         )
DEFINE_LISTENER_CONSTRUCTOR_FOR( TextInputEvent       , text_input        )
DEFINE_LISTENER_CONSTRUCTOR_FOR( MouseMotionEvent     , mouse_motion      )
DEFINE_LISTENER_CONSTRUCTOR_FOR( MouseButtonEvent     , mouse_button      )
DEFINE_LISTENER_CONSTRUCTOR_FOR( MouseWheelEvent      , scroll            )
DEFINE_LISTENER_CONSTRUCTOR_FOR( JoyAxisEvent         , joy_axis          )
DEFINE_LISTENER_CONSTRUCTOR_FOR( JoyBallEvent         , joy_ball          )
DEFINE_LISTENER_CONSTRUCTOR_FOR( JoyHatEvent          , joy_hat           )
DEFINE_LISTENER_CONSTRUCTOR_FOR( JoyButtonEvent       , joy_button        )
DEFINE_LISTENER_CONSTRUCTOR_FOR( JoyDeviceEvent       , joy_device        )
DEFINE_LISTENER_CONSTRUCTOR_FOR( ControllerAxisEvent  , controller_axis   )
DEFINE_LISTENER_CONSTRUCTOR_FOR( ControllerButtonEvent, controller_button )
DEFINE_LISTENER_CONSTRUCTOR_FOR( ControllerDeviceEvent, controller_device )
DEFINE_LISTENER_CONSTRUCTOR_FOR( AudioDeviceEvent     , audio_device      )
DEFINE_LISTENER_CONSTRUCTOR_FOR( QuitEvent            , quit              )
DEFINE_LISTENER_CONSTRUCTOR_FOR( UserEvent            , user_event        )
DEFINE_LISTENER_CONSTRUCTOR_FOR( SysWMEvent           , sys_wm            )
DEFINE_LISTENER_CONSTRUCTOR_FOR( TouchFingerEvent     , touch             )
DEFINE_LISTENER_CONSTRUCTOR_FOR( MultiGestureEvent    , gesture           )
DEFINE_LISTENER_CONSTRUCTOR_FOR( DollarGestureEvent   , dollar_gesture    )
DEFINE_LISTENER_CONSTRUCTOR_FOR( DropEvent            , drop              )

#undef DEFINE_LISTENER_CONSTRUCTOR_FOR


// Listener destructor implementations /////////////////////////////////////////

#define DEFINE_LISTENER_DESTRUCTOR_FOR( EVENT, MAPNAME ) \
    template<> JadeMatrix::yavsg::event_listener< \
        SDL_##EVENT \
    >::~event_listener() \
    { \
        std::unique_lock lock( listeners_mutex ); \
        MAPNAME##_listeners.erase( id_ ); \
    }

DEFINE_LISTENER_DESTRUCTOR_FOR( CommonEvent          , common            )
DEFINE_LISTENER_DESTRUCTOR_FOR( WindowEvent          , window            )
DEFINE_LISTENER_DESTRUCTOR_FOR( KeyboardEvent        , keyboard          )
DEFINE_LISTENER_DESTRUCTOR_FOR( TextEditingEvent     , text_edit         )
DEFINE_LISTENER_DESTRUCTOR_FOR( TextInputEvent       , text_input        )
DEFINE_LISTENER_DESTRUCTOR_FOR( MouseMotionEvent     , mouse_motion      )
DEFINE_LISTENER_DESTRUCTOR_FOR( MouseButtonEvent     , mouse_button      )
DEFINE_LISTENER_DESTRUCTOR_FOR( MouseWheelEvent      , scroll            )
DEFINE_LISTENER_DESTRUCTOR_FOR( JoyAxisEvent         , joy_axis          )
DEFINE_LISTENER_DESTRUCTOR_FOR( JoyBallEvent         , joy_ball          )
DEFINE_LISTENER_DESTRUCTOR_FOR( JoyHatEvent          , joy_hat           )
DEFINE_LISTENER_DESTRUCTOR_FOR( JoyButtonEvent       , joy_button        )
DEFINE_LISTENER_DESTRUCTOR_FOR( JoyDeviceEvent       , joy_device        )
DEFINE_LISTENER_DESTRUCTOR_FOR( ControllerAxisEvent  , controller_axis   )
DEFINE_LISTENER_DESTRUCTOR_FOR( ControllerButtonEvent, controller_button )
DEFINE_LISTENER_DESTRUCTOR_FOR( ControllerDeviceEvent, controller_device )
DEFINE_LISTENER_DESTRUCTOR_FOR( AudioDeviceEvent     , audio_device      )
DEFINE_LISTENER_DESTRUCTOR_FOR( QuitEvent            , quit              )
DEFINE_LISTENER_DESTRUCTOR_FOR( UserEvent            , user_event        )
DEFINE_LISTENER_DESTRUCTOR_FOR( SysWMEvent           , sys_wm            )
DEFINE_LISTENER_DESTRUCTOR_FOR( TouchFingerEvent     , touch             )
DEFINE_LISTENER_DESTRUCTOR_FOR( MultiGestureEvent    , gesture           )
DEFINE_LISTENER_DESTRUCTOR_FOR( DollarGestureEvent   , dollar_gesture    )
DEFINE_LISTENER_DESTRUCTOR_FOR( DropEvent            , drop              )

#undef DEFINE_LISTENER_DESTRUCTOR_FOR
