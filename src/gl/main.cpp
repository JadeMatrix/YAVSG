#include "gl_utils.hpp"
#include "sdl_utils.hpp"
#include "render_steps.hpp"

#include <iostream>
#include <exception>
#include <vector>


namespace
{
    const int window_width  = 800;
    const int window_height = 600;
}


int main( int argc, char* argv[] )
{
    try
    {
        gl::SDL_manager sdl;
        
        gl::SDL_window_manager window(
            "OpenGL",
            SDL_WINDOWPOS_CENTERED, // 100,
            SDL_WINDOWPOS_CENTERED, // 100,
            window_width,
            window_height,
            SDL_WINDOW_OPENGL // | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN
        );
        
        // Run GLEW stuff _after_ creating SDL/GL context
    #ifndef __APPLE__
        glewExperimental = GL_TRUE;
        if( glewInit() != GLEW_OK )
        {
            std::cerr << "failed to initialize GLEW" << std::endl;
            return -1;
        }
    #endif
        
        std::vector< gl::render_step* > render_steps = {
            // new gl::feedback_render_step(),
            new gl::scene_render_step(),
            new gl::postprocess_render_step()
        };
        
        gl::framebuffer preprocessing_framebuffer(
            window_width,
            window_height
        );
        
        SDL_Event window_event;
        while( true )
        {
            if( SDL_PollEvent( &window_event ) )
            {
                if( window_event.type == SDL_QUIT )
                    break;
                else if(
                    window_event.type == SDL_KEYUP
                    && window_event.key.keysym.sym == SDLK_ESCAPE
                    && SDL_GetWindowFlags( window.sdl_window ) & (
                          SDL_WINDOW_FULLSCREEN
                        | SDL_WINDOW_FULLSCREEN_DESKTOP
                    )
                )
                    break;
            }
            
            auto step_iter = render_steps.begin();
            while( step_iter != render_steps.end() )
            {
                auto step = *step_iter;
                
                if( ++step_iter == render_steps.end() )
                    // Bind default framebuffer
                    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
                else
                    glBindFramebuffer(
                        GL_FRAMEBUFFER,
                        preprocessing_framebuffer.id
                    );
                
                step -> run( preprocessing_framebuffer );
            }
            
            SDL_GL_SwapWindow( window.sdl_window );
        }
        
        for( auto step : render_steps )
            delete step;
        
        return 0;
    }
    catch( const std::exception& e )
    {
        std::cerr
            << "program exiting: "
            << e.what()
            << std::endl
        ;
        return -1;
    }
}
