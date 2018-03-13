#include "../include/gl/_gl_base.hpp" // For GLEW initialization, if any

#include "../include/gl/error.hpp"
#include "../include/gl/framebuffer.hpp"
#include "../include/sdl/sdl_utils.hpp"
#include "../include/rendering/render_step.hpp"

// DEVEL:
#include "../include/rendering/gl_tut.hpp"
#include "../include/rendering/obj_render_step.hpp"
#include "../include/rendering/basic_postprocess_step.hpp"
#include "../include/rendering/4up_postprocess_step.hpp"

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
            "YAVSG",
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
        
        using postprocess_step_type = yavsg::postprocess_step<
            yavsg::gl::texture< GLfloat, 3 >
        >;
        using fb_type = yavsg::gl::framebuffer<
            yavsg::gl::texture< GLfloat, 3 >
        >;
        using wo_fb_type = yavsg::gl::write_only_framebuffer;
        
        std::vector< yavsg::render_step* > scene_steps = {
            new yavsg::obj_render_step(
                "../local/Crytek Sponza Atrium/sponza.obj",
                "../local/Crytek Sponza Atrium/"
            )
        };
        std::vector< postprocess_step_type* > postprocess_steps = {
            new yavsg::debug_4up_postprocess_step(
                nullptr,
                new yavsg::basic_postprocess_step(
                    "../src/shaders/postprocess/circular_gradient.frag"
                ),
                new yavsg::basic_postprocess_step(
                    "../src/shaders/postprocess/dof.frag"
                ),
                new yavsg::basic_postprocess_step(
                    "../src/shaders/postprocess/buffer_analysis.frag"
                )
            )
        };
        
        fb_type buffer_A(
            gl_tut::window_width,
            gl_tut::window_height
        );
        fb_type buffer_B(
            gl_tut::window_width,
            gl_tut::window_height
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
            
            // Even though only target_buffer needs to be a write-only buffer,
            // these both need to be the same type for std::swap()
            wo_fb_type* source_buffer;
            wo_fb_type* target_buffer;
            
            // Run all scene render steps against the same framebuffer
            if( postprocess_steps.size() )
                target_buffer = &buffer_A;
            else
                target_buffer = &window.default_framebuffer();
            source_buffer = &buffer_B;
            
            target_buffer -> alpha_blending(
                yavsg::gl::alpha_blend_mode::PREMULTIPLIED_DROP_ALPHA
            );
            target_buffer -> bind();
            
            for( auto step : scene_steps )
                step -> run( *target_buffer );
            
            auto postprocess_step_iter = postprocess_steps.begin();
            while( postprocess_step_iter != postprocess_steps.end() )
            {
                std::swap( source_buffer, target_buffer);
                
                bool is_first_step =
                    postprocess_step_iter == postprocess_steps.begin();
                
                // Get the current step before advancing the iterator
                auto step = *postprocess_step_iter;
                // The last postprocess step should target the window's default
                // framebuffer
                if( ++postprocess_step_iter == postprocess_steps.end() )
                    target_buffer = &window.default_framebuffer();
                
                target_buffer -> alpha_blending(
                    yavsg::gl::alpha_blend_mode::DISABLED
                );
                
                target_buffer -> bind();
                
                step -> run(
                    *static_cast< fb_type* >( source_buffer ),
                    *                         target_buffer
                );
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
                    std::cerr << std::to_string( code );
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
