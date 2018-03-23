#include "../../include/gl/_gl_base.hpp" // For GLEW initialization, if any

#include "../../include/gl/error.hpp"
#include "../../include/sdl/sdl_utils.hpp"
#include "../../include/tasking/tasking.hpp"

// DEVEL:
#include "../../include/engine/frame.hpp"
#include "../../include/events/event_listener.hpp"
#include "../../include/sdl/_sdl_base.hpp"
#include "../../include/tasking/tasking.hpp"
#include "../../include/tasking/utility_tasks.hpp"
#include "../../include/windowsys/window.hpp"
#include "../../include/engine/obj.hpp"
#include "../../include/units/angular.hpp"

#include <exception>
#include <iostream>
#include <utility>  // std::size_t


namespace
{
    const std::size_t window_width  = 1280;
    const std::size_t window_height = 720;
}


int main( int argc, char* argv[] )
{
    try
    {
        yavsg::SDL_manager sdl;
        
        yavsg::event_listener< SDL_QuitEvent > quit_listener{
            []( const SDL_QuitEvent& e ){
                std::cout << "quitting...\n";
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
            yavsg::window_arrange_state::NORMAL,
            "YAVSG",
            1.0f,
            true,
            true
        } );
        
        auto    base_fov = yavsg::radians< GLfloat >{ 45 }; // 58.31f°
        auto desired_fov = yavsg::radians< GLfloat >{
            yavsg::degrees< GLfloat >{ 90 }
        };
        test_window -> main_scene.main_camera.fov( desired_fov );
        test_window -> main_scene.main_camera.look_at( { 0.0f, 0.0f, 0.0f } );
        test_window -> main_scene.main_camera.focal_point( 1.0f );
        
        submit_task( std::make_unique< yavsg::load_obj_task >(
            test_window -> main_scene.object_manager,
            "../local/Crytek Sponza Atrium/sponza.obj",
            "../local/Crytek Sponza Atrium/"
        ) );
        
        SDL_SetRelativeMouseMode( SDL_TRUE );
        yavsg::event_listener< SDL_KeyboardEvent > center_window_listener{
            [ test_window ]( const SDL_KeyboardEvent& e ){
                if( e.type == SDL_KEYUP && e.keysym.sym == SDLK_c )
                {
                    std::cout << "centering window...\n";
                    test_window -> center();
                }
            }
        };
        yavsg::event_listener< SDL_KeyboardEvent > break_listener{
            []( const SDL_KeyboardEvent& e ){
                if( e.type == SDL_KEYUP && e.keysym.sym == SDLK_SPACE )
                    std::cout << "\n--------------------------------------\n\n";
            }
        };
        yavsg::event_listener< SDL_KeyboardEvent > close_window_listener{
            [ test_window ]( const SDL_KeyboardEvent& e ){
                if( e.type == SDL_KEYUP && e.keysym.sym == SDLK_ESCAPE )
                {
                    std::cout << "closing window...\n";
                    delete test_window;
                    SDL_Event quit_event;
                    quit_event.quit.type = SDL_QUIT;
                    SDL_PushEvent( &quit_event );
                    std::cout << "quit event pushed\n";
                }
            },
            yavsg::task_flag::MAIN_THREAD
        };
        
        // TODO: Figure out why quit events don't work in single-threaded mode
        // yavsg::initialize_task_system( static_cast< std::size_t >( 0 ) );
        yavsg::initialize_task_system( true );
        std::cout << "task system initialized, becoming worker...\n";
        yavsg::become_task_worker( yavsg::task_flag::MAIN_THREAD );
        yavsg::stop_task_system( true );
        std::cout << "task system stopped\n";
        
        return 0;
    }
    catch( const yavsg::gl::summary_error& e )
    {
        std::cerr
            << "program exiting due to OpenGL error: "
            << e.what()
            << "; codes:"
            << std::endl
        ;
        yavsg::gl::print_summary_error_codes( std::cerr, e );
    }
    catch( const std::exception& e )
    {
        std::cerr
            << "program exiting: "
            << e.what()
            << std::endl
        ;
    }
    catch( ... )
    {
        std::cerr
            << "program exiting due to uncaught non-std::exception"
            << std::endl
        ;
    }
    
    return -1;
}
