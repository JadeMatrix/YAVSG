// #include "gl_utils.hpp"
// #include "sdl_utils.hpp"
// #include "render_steps.hpp"

#include "../include/gl/_gl_base.hpp" // For GLEW initialization, if any

#include "../include/gl/texture.hpp"
#include "../include/gl/framebuffer.hpp"
#include "../include/gl/shader.hpp"
#include "../include/sdl/sdl_utils.hpp"
#include "../include/math/transforms.hpp"
#include "../include/math/common_transforms.hpp"
#include "../include/rendering/render_step.hpp"
#include "../include/rendering/obj_render_step.hpp"

#include "../include/rendering/gl_tut.hpp"
#include "../include/rendering/gl_tut_scene_render_step.hpp"
#include "../include/rendering/gl_tut_postprocess_render_step.hpp"

#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <vector>


int main( int argc, char* argv[] )
{
    try
    {
        yavsg::SDL_manager sdl;
        
        yavsg::SDL_window_manager window(
            "OpenGL",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            gl_tut::window_width,
            gl_tut::window_height,
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
        
        std::vector< yavsg::scene_render_step* > scene_steps = {
            // new gl_tut::scene_render_step()
            new yavsg::obj_render_step(
                "../local/Crytek Sponza Atrium/sponza.obj",
                "../local/Crytek Sponza Atrium/"
                // "../local/cube/cube.obj",
                // "../local/cube/"
            )
        };
        std::vector< yavsg::postprocess_render_step* > postprocess_steps = {
            // new gl_tut::postprocess_render_step(
            //     "../src/shaders/postprocess/sobel.frag"
            // ),
            // new gl_tut::postprocess_render_step(
            //     "../src/shaders/postprocess/box_blur.frag"
            // ),
            // new gl_tut::postprocess_render_step(
            //     "../src/shaders/postprocess/circular_gradient.frag"
            // )
        };
        
        yavsg::gl::framebuffer buffer_A(
            gl_tut::window_width,
            gl_tut::window_height
        );
        yavsg::gl::framebuffer buffer_B(
            gl_tut::window_width,
            gl_tut::window_height
        );
        
        yavsg::gl::framebuffer* source_buffer = &buffer_A;
        yavsg::gl::framebuffer* target_buffer = &buffer_B;
        
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
            
            // Run all scene render steps against the same framebuffer
            if( postprocess_steps.size() )
                target_buffer -> bind();
            else
                yavsg::gl::bind_default_framebuffer();
            for( auto step : scene_steps )
                step -> run();
            
            auto postprocess_step_iter = postprocess_steps.begin();
            while( postprocess_step_iter != postprocess_steps.end() )
            {
                std::swap( source_buffer, target_buffer );
                
                // Get the current step before advancing the iterator
                auto step = *postprocess_step_iter;
                if( ++postprocess_step_iter == postprocess_steps.end() )
                    yavsg::gl::bind_default_framebuffer();
                else
                    target_buffer -> bind();
                
                step -> run( *source_buffer );
            }
            
            SDL_GL_SwapWindow( window.sdl_window );
        }
        
        for( auto step : postprocess_steps )
            delete step;
        for( auto step : scene_steps )
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
