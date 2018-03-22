#include "../../include/gl/_gl_base.hpp" // For GLEW initialization, if any

#include "../../include/gl/error.hpp"
#include "../../include/tasking/tasking.hpp"

// DEVEL:
#include "../../include/engine/frame.hpp"
#include "../../include/events/event_listener.hpp"
#include "../../include/sdl/_sdl_base.hpp"
#include "../../include/tasking/tasking.hpp"
#include "../../include/tasking/utility_tasks.hpp"
#include "../../include/windowsys/window.hpp"

#include <exception>
#include <iostream>


int main( int argc, char* argv[] )
{
    try
    {
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
            1280,
            720,
            -1,
            -1,
            yavsg::window_arrange_state::NORMAL,
            "YAVSG",
            1.0f,
            true
        } );
        // WARNING: have to destroy before exiting as worker!
        yavsg::event_listener< SDL_KeyboardEvent > close_window_listener{
            [ test_window ]( const SDL_KeyboardEvent& e ){
                if( e.type == SDL_KEYUP && e.keysym.sym == SDLK_ESCAPE )
                {
                    std::cout << "closing window...\n";
                    delete test_window;
                    SDL_Event quit_event;
                    quit_event.quit.type = SDL_QUIT;
                    SDL_PushEvent( &quit_event );
                }
            },
            yavsg::task_flag::MAIN_THREAD
        };
        
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
