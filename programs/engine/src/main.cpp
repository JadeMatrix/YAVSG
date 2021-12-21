#include <yavsg/gl_wrap.hpp>    // For GLEW initialization, if any

#include <yavsg/gl/error.hpp>   // gl::error
#include <yavsg/sdl/utils.hpp>
#include <yavsg/tasking/tasking.hpp>

// DEVEL:
#include <yavsg/asserts.hpp>
#include <yavsg/windowsys/frame.hpp>
#include <yavsg/events/event_listener.hpp>
#include <yavsg/logging.hpp>
#include <yavsg/sdl/sdl.hpp>
#include <yavsg/tasking/tasking.hpp>
#include <yavsg/tasking/utility_tasks.hpp>
#include <yavsg/windowsys/window.hpp>
#include <yavsg/rendering/obj.hpp>
#include <yavsg/units/angular.hpp>

#include <cstddef>  // size_t
#include <exception>
#include <iostream> // clog
#include <string_view>


namespace
{
    constexpr std::size_t window_width  = 1280;
    constexpr std::size_t window_height = 720;
    
    namespace yavsg = JadeMatrix::yavsg;
    
    using namespace std::string_view_literals;
    
    auto const log_ = yavsg::log_handle();
}


int main( int argc, char* argv[] )
{
    // CONSIDER: Move this to a support library
    doctest::Context global_doctest_context;
    global_doctest_context.setAsDefaultForAssertsOutOfTestCases();
    global_doctest_context.setAssertHandler( yavsg::doctest_assert_handler );
    
    if( argc < 3 )
    {
        log_.error(
            "Usage: env YAVSG_SHADERS_DIR=<shaders-dir> {} <obj-file> "
                "<materials-dir>"sv,
            argv[ 0 ]
        );
        return -1;
    }

    try
    {
        yavsg::SDL_manager sdl;
        
        yavsg::event_listener< SDL_QuitEvent > quit_listener{
            []( SDL_QuitEvent const& ){
                log_.info( "Quitting..."sv );
                yavsg::submit_task(
                    std::make_unique< yavsg::stop_task_system_task >()
                );
            }
        };
        
        /*
        TODO:
        
        create window
        create scene
        add scene to window
        submit obj load for scene
        
        */
        
        // yavsg::submit_task( std::make_unique< yavsg::frame_task >() );
        
        // DEVEL:
        auto test_window = new yavsg::window( {
            window_width,
            window_height,
            -1,
            -1,
            yavsg::window_arrange_state::normal,
            "YAVSG",
            1.0f,
            true,
            true
        } );
        
        auto const desired_fov = yavsg::radians< GLfloat >{
            yavsg::degrees< GLfloat >{ 90 }
        };
        test_window->main_scene.main_camera.fov( desired_fov );
        test_window->main_scene.main_camera.look_at( { 0.0f, 0.0f, 0.0f } );
        test_window->main_scene.main_camera.focal_point( 1.0f );
        
        submit_task( std::make_unique< yavsg::load_obj_task >(
            test_window->main_scene.object_manager,
            argv[ 1 ],
            argv[ 2 ]
        ) );
        
        SDL_SetRelativeMouseMode( SDL_TRUE );
        
        yavsg::event_listener< SDL_MouseMotionEvent > camera_look_listener{
            [ window_ref = test_window->reference() ](
                SDL_MouseMotionEvent const& e
            ){
                std::unique_lock wrlock( window_ref->reference_mutex );
                if( !window_ref->window )
                {
                    return;
                }
                
                window_ref->window->main_scene.main_camera.increment_pitch_yaw(
                    static_cast< float >( e.yrel ) / -4.0f,
                    static_cast< float >( e.xrel ) / -4.0f
                );
            }
        };
        yavsg::event_listener< SDL_MouseWheelEvent > focal_adjust_listener{
            [ window_ref = test_window->reference() ](
                SDL_MouseWheelEvent const& e
            ){
                std::unique_lock wrlock( window_ref->reference_mutex );
                if( !window_ref->window )
                {
                    return;
                }
                
                auto new_focal_point = (
                    static_cast< float >( e.y )
                    / 4.0f
                    + window_ref->window->main_scene.main_camera.focal_point()
                );
                auto near = window_ref->window->main_scene.main_camera.near_point();
                auto  far = window_ref->window->main_scene.main_camera. far_point();
                if( new_focal_point < near )
                {
                    new_focal_point = near + 0.1f;
                }
                else if( new_focal_point > far )
                {
                    new_focal_point = far - 0.1f;
                }
                window_ref->window->main_scene.main_camera.focal_point(
                    new_focal_point
                );
            }
        };
        yavsg::event_listener< SDL_KeyboardEvent > center_window_listener{
            [ test_window ]( SDL_KeyboardEvent const& e ){
                if( e.type == SDL_KEYUP && e.keysym.sym == SDLK_c )
                {
                    log_.info( "Centering window..."sv );
                    test_window->center();
                }
            }
        };
        yavsg::event_listener< SDL_KeyboardEvent > break_listener{
            []( SDL_KeyboardEvent const& e ){
                if( e.type == SDL_KEYUP && e.keysym.sym == SDLK_SPACE )
                {
                    std::clog
                        << "\n--------------------------------------\n\n"sv
                    ;
                }
            }
        };
        yavsg::event_listener< SDL_KeyboardEvent > close_window_listener{
            [ test_window ]( SDL_KeyboardEvent const& e ){
                if( e.type == SDL_KEYUP && e.keysym.sym == SDLK_ESCAPE )
                {
                    log_.info( "Closing window..."sv );
                    delete test_window;
                    SDL_Event quit_event;
                    quit_event.quit.type = SDL_QUIT;
                    SDL_PushEvent( &quit_event );
                    log_.info( "Quit event pushed"sv );
                }
            },
            yavsg::task_flag::main_thread
        };
        
        // TODO: Figure out why quit events don't work in single-threaded mode
        // yavsg::initialize_task_system( static_cast< std::size_t >( 0 ) );
        yavsg::initialize_task_system( true );
        log_.info( "task system initialized, becoming worker..."sv );
        yavsg::become_task_worker( yavsg::task_flag::main_thread );
        yavsg::stop_task_system( true );
        log_.info( "task system stopped"sv );
        
        return 0;
    }
    catch( yavsg::gl::error const& e )
    {
        log_.error( "Program exiting due to OpenGL error: {}"sv, e.what() );
    }
    catch( std::exception const& e )
    {
        log_.error( "Program exiting: {}"sv, e.what() );
    }
    catch( ... )
    {
        log_.error( "Program exiting due to uncaught non-std::exception"sv );
    }
    
    return -1;
}
