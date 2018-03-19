#include "../../include/gl/_gl_base.hpp" // For GLEW initialization, if any

#include "../../include/gl/error.hpp"
#include "../../include/tasking/tasking.hpp"

// DEVEL:
#include "../../include/engine/frame.hpp"

#include <exception>
#include <iostream>


int main( int argc, char* argv[] )
{
    try
    {
        yavsg::submit_task( std::make_unique< yavsg::frame_task >() );
        
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
