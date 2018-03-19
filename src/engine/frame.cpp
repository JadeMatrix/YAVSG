#include "../../include/engine/frame.hpp"

#include "../../include/engine/4up_postprocess_step.hpp"
#include "../../include/engine/basic_postprocess_step.hpp"
#include "../../include/engine/dof_postprocess_step.hpp"
#include "../../include/gl/framebuffer.hpp"
#include "../../include/tasking/utility_tasks.hpp"

#include <iostream>


namespace
{
    const int window_width  = 1280;
    const int window_height = 720;
}


namespace yavsg
{
    frame_task::frame_task() :
        window{
            "YAVSG",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            window_width,
            window_height,
            SDL_WINDOW_OPENGL // | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN
        },
        running{ true }
    {
        SDL_SetRelativeMouseMode( SDL_TRUE );
        
        // Run GLEW stuff _after_ creating SDL/GL context
    #ifndef __APPLE__
        glewExperimental = GL_TRUE;
        if( glewInit() != GLEW_OK )
            throw std::runtime_error( "failed to initialize GLEW" );
    #endif
        
        ors = new obj_render_step(
            "../local/Crytek Sponza Atrium/sponza.obj",
            "../local/Crytek Sponza Atrium/"
        );
        
        scene_steps = std::vector< render_step* >{
            ors
        };
        postprocess_steps = std::vector< postprocess_step_type* >{
            // new debug_4up_postprocess_step(
            //     nullptr,
            //     new basic_postprocess_step(
            //         "../src/shaders/postprocess/linear_to_sRGB.frag"
            //     ),
            //     new dof_postprocess_step(
            //         ors -> main_camera
            //     ),
            //     new basic_postprocess_step(
            //         "../src/shaders/postprocess/buffer_analysis.frag"
            //     )
            // )
            new dof_postprocess_step(
                ors -> main_camera
            ),
            new basic_postprocess_step(
                "../src/shaders/postprocess/linear_to_sRGB.frag"
            )
        };
        
        start_time = std::chrono::high_resolution_clock::now();
        previous_time = start_time;
        
        // TODO: Get dimensions from window
        buffer_A = new fb_type(
            window_width,
            window_height
        );
        buffer_B = new fb_type(
            window_width,
            window_height
        );
    }
    
    frame_task::~frame_task()
    {
        delete buffer_A;
        delete buffer_B;
        
        for( auto step : postprocess_steps )
            delete step;
        for( auto step : scene_steps )
            delete step;
    }
    
    task_flags_type frame_task::flags() const
    {
        return task_flag::MAIN_THREAD | task_flag::GPU_THREAD;
    }
    
    bool frame_task::operator()()
    {
        SDL_Event window_event;
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
                    static_cast< float >( window_event.motion.yrel ) / -4.0f,
                    static_cast< float >( window_event.motion.xrel ) / -4.0f
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
        {
            std::cout << "quitting..." << std::endl;
            submit_task(
                std::make_unique< stop_task_system_task >()
            );
        }
        else
        {
            auto current_time = std::chrono::high_resolution_clock::now();
            
            // Even though only target_buffer needs to be a write-only
            // buffer, these both need to be the same type for std::swap()
            wo_fb_type* source_buffer;
            wo_fb_type* target_buffer;
            
            if( postprocess_steps.size() )
                target_buffer = buffer_A;
            else
                target_buffer = &window.default_framebuffer();
            source_buffer = buffer_B;
            
            target_buffer -> alpha_blending(
                gl::alpha_blend_mode::PREMULTIPLIED_DROP_ALPHA
            );
            target_buffer -> bind();
            
            // Run all scene render steps against the same framebuffer
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
                // The last postprocess step should target the window's
                // default framebuffer
                if( ++postprocess_step_iter == postprocess_steps.end() )
                    target_buffer = &window.default_framebuffer();
                
                target_buffer -> alpha_blending(
                    gl::alpha_blend_mode::DISABLED
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
        
        return running;
    }
}