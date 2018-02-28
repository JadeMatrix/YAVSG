// #include "gl_utils.hpp"
// #include "sdl_utils.hpp"
// #include "render_steps.hpp"

#include "../include/gl/_gl_base.hpp" // For GLEW initialization, if any

#include "../include/gl/error.hpp"
#include "../include/gl/framebuffer.hpp"
#include "../include/gl/shader.hpp"
#include "../include/sdl/sdl_utils.hpp"
#include "../include/rendering/render_step.hpp"
#include "../include/rendering/obj_render_step.hpp"
// #include "../include/rendering/4up_postprocess_render_step.hpp"

#include "../include/rendering/gl_tut.hpp"
// #include "../include/rendering/gl_tut_scene_render_step.hpp"
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
            throw std::runtime_error( "failed to initialize GLEW" );
    #endif
        
        std::vector< yavsg::render_step* > scene_steps = {
            // new gl_tut::render_step()
            new yavsg::obj_render_step(
                "../local/Crytek Sponza Atrium/sponza.obj",
                "../local/Crytek Sponza Atrium/"
                // "../local/cube/cube.obj",
                // "../local/cube/"
            )
        };
        std::vector< yavsg::postprocess_step< 1 >* > postprocess_steps = {
            // new gl_tut::postprocess_render_step(
            //     "../src/shaders/postprocess/sobel.frag"
            // ),
            // new gl_tut::postprocess_render_step(
            //     "../src/shaders/postprocess/box_blur.frag"
            // ),
            // new gl_tut::postprocess_render_step(
            //     "../src/shaders/postprocess/circular_gradient.frag"
            // ),
            // new gl_tut::postprocess_render_step(
            //     "../src/shaders/postprocess/depth.frag"
            // ),
            new gl_tut::postprocess_render_step(
                "../src/shaders/postprocess/dof.frag"
            )
            // new yavsg::debug_4up_postprocess_render_step(
            //     nullptr,
            //     new gl_tut::postprocess_render_step(
            //         "../src/shaders/postprocess/dof.frag"
            //     ),
            //     new gl_tut::postprocess_render_step(
            //         "../src/shaders/postprocess/depth.frag"
            //     ),
            //     new gl_tut::postprocess_render_step(
            //         "../src/shaders/postprocess/sobel.frag"
            //     )
            // )
        };
        
        yavsg::gl::framebuffer< 1 > buffer_A(
            gl_tut::window_width,
            gl_tut::window_height
        );
        yavsg::gl::framebuffer< 1 > buffer_B(
            gl_tut::window_width,
            gl_tut::window_height
        );
        
        yavsg::gl::framebuffer< 1 >* source_buffer = &buffer_A;
        yavsg::gl::framebuffer< 1 >* target_buffer = &buffer_B;
        
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
    catch( const yavsg::gl::summary_error& e )
    {
        std::cerr
            << "program exiting due to OpenGL error: "
            << e.what()
            << "; codes:"
            << std::endl
        ;
        if( e.error_codes.size() )
        {
            for( auto code : e.error_codes )
            {
                std::cerr << "  ";
                switch( code )
                {
                case GL_INVALID_ENUM:
                    std::cerr << "GL_INVALID_ENUM";
                    break;
                case GL_INVALID_VALUE:
                    std::cerr << "GL_INVALID_VALUE";
                    break;
                case GL_INVALID_OPERATION:
                    std::cerr << "GL_INVALID_OPERATION";
                    break;
                case GL_STACK_OVERFLOW:
                    std::cerr << "GL_STACK_OVERFLOW";
                    break;
                case GL_STACK_UNDERFLOW:
                    std::cerr << "GL_STACK_UNDERFLOW";
                    break;
                case GL_OUT_OF_MEMORY:
                    std::cerr << "GL_OUT_OF_MEMORY";
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION";
                    break;
                case 0x0507: // GL_CONTEXT_LOST available in 4.5+
                    std::cerr << "GL_CONTEXT_LOST";
                    break;
                case GL_TABLE_TOO_LARGE:
                    std::cerr << "GL_TABLE_TOO_LARGE";
                    break;
                default:
                    std::cerr << std::to_string( ( unsigned int )code );
                    break;
                }
                std::cerr << std::endl;
            }
        }
        else
            std::cerr << "  (none)" << std::endl;
    }
    catch( const std::exception& e )
    {
        std::cerr
            << "program exiting: "
            << e.what()
            << std::endl
        ;
    }
    
    return -1;
}
