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
#include "../include/rendering/dof_postprocess_step.hpp"

#include <chrono>
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
        SDL_SetRelativeMouseMode( SDL_TRUE );
        
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
        
        auto ors = new yavsg::obj_render_step(
            "../local/Crytek Sponza Atrium/sponza.obj",
            "../local/Crytek Sponza Atrium/"
        );
        std::vector< yavsg::render_step* > scene_steps = {
            ors
        };
        std::vector< postprocess_step_type* > postprocess_steps = {
            new yavsg::debug_4up_postprocess_step(
                nullptr,
                new yavsg::basic_postprocess_step(
                    "../src/shaders/postprocess/linear_to_sRGB.frag"
                ),
                new yavsg::dof_postprocess_step(
                    ors -> main_camera
                ),
                new yavsg::basic_postprocess_step(
                    "../src/shaders/postprocess/buffer_analysis.frag"
                )
            )
        };
        
        auto    start_time = std::chrono::high_resolution_clock::now();
        auto previous_time = start_time;
        
        fb_type buffer_A(
            gl_tut::window_width,
            gl_tut::window_height
        );
        fb_type buffer_B(
            gl_tut::window_width,
            gl_tut::window_height
        );
        
        bool running = true;
        SDL_Event window_event;
        while( running )
        {
            while( SDL_PollEvent( &window_event ) )
            {
                switch( window_event.type )
                {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    {
                        switch( window_event.key.keysym.sym )
                        {
                        case SDLK_ESCAPE:
                            if( SDL_GetWindowFlags( window.sdl_window ) & (
                                  SDL_WINDOW_FULLSCREEN
                                | SDL_WINDOW_FULLSCREEN_DESKTOP
                            ) )
                                running = false;
                            break;
                        default:
                            break;
                        }
                    }
                    break;
                case SDL_MOUSEMOTION:
                    ors -> main_camera.increment_pitch_yaw(
                        static_cast< float >( window_event.motion.yrel ) / -2.0f,
                        static_cast< float >( window_event.motion.xrel ) / -2.0f
                    );
                    break;
                case SDL_MOUSEWHEEL:
                    {
                        auto new_focal_point = (
                            static_cast< float >( window_event.wheel.y )
                            / 4.0f
                            + ors -> main_camera.focal_point()
                        );
                        auto near = ors -> main_camera.near_point();
                        auto  far = ors -> main_camera. far_point();
                        if( new_focal_point < near )
                            new_focal_point = near + 0.1f;
                        else if( new_focal_point > far )
                            new_focal_point = far - 0.1f;
                        ors -> main_camera.focal_point( new_focal_point );
                    }
                    break;
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
                }
            }
            
            if( !running )
                std::cout << "quitting..." << std::endl;
            
            auto current_time = std::chrono::high_resolution_clock::now();
            
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
            previous_time = current_time;
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
