#include "../../include/engine/frame.hpp"

#include "../../include/engine/4up_postprocess_step.hpp"
#include "../../include/engine/basic_postprocess_step.hpp"
#include "../../include/engine/dof_postprocess_step.hpp"
#include "../../include/engine/multi_postprocess_step.hpp"
#include "../../include/gl/framebuffer.hpp"
#include "../../include/tasking/tasking.hpp"
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
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE // | SDL_WINDOW_FULLSCREEN
        },
        camera_look_listener{ [ this ]( const SDL_MouseMotionEvent& e ){
            this -> ors -> main_camera.increment_pitch_yaw(
                static_cast< float >( e.yrel ) / -4.0f,
                static_cast< float >( e.xrel ) / -4.0f
            );
        } },
        focal_adjust_listener{ [ this ]( const SDL_MouseWheelEvent& e ){
            auto new_focal_point = (
                static_cast< float >( e.y )
                / 4.0f
                + this -> ors -> main_camera.focal_point()
            );
            auto near = this -> ors -> main_camera.near_point();
            auto  far = this -> ors -> main_camera. far_point();
            if( new_focal_point < near )
                new_focal_point = near + 0.1f;
            else if( new_focal_point > far )
                new_focal_point = far - 0.1f;
            this -> ors -> main_camera.focal_point( new_focal_point );
        } }
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
            new debug_4up_postprocess_step(
                nullptr,
                std::make_unique< multi_postprocess_step >(
                    std::vector< std::string >{
                        "linear_to_sRGB"
                    }
                ),
                std::make_unique< dof_postprocess_step >(
                    ors -> main_camera
                ),
                std::make_unique< multi_postprocess_step >(
                    std::vector< std::string >{
                        "buffer_analysis"
                    }
                )
            ),
            // std::make_unique< dof_postprocess_step >(
            //     ors -> main_camera
            // ),
            // std::make_unique< multi_postprocess_step >(
            //     std::vector< std::string >{
            //         "linear_to_sRGB"
            //     }
            // )
        };
        
        start_time = std::chrono::high_resolution_clock::now();
        previous_time = start_time;
    }
    
    frame_task::~frame_task()
    {
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
        auto current_time = std::chrono::high_resolution_clock::now();
        
        // TODO: Just use window dimensions
        auto& window_buffer = window.default_framebuffer();
        
        // Initialize buffers or update dimensions; only checks one because both
        // A and B's states are locked
        if(
            !buffer_A
            || buffer_A ->  width() != window_buffer. width()
            || buffer_A -> height() != window_buffer.height()
        )
        {
            buffer_A = std::make_unique< fb_type >(
                window_buffer. width(),
                window_buffer.height()
            );
            buffer_B = std::make_unique< fb_type >(
                window_buffer. width(),
                window_buffer.height()
            );
        }
        
        // Even though only target_buffer needs to be a write-only
        // buffer, these both need to be the same type for std::swap()
        wo_fb_type* source_buffer;
        wo_fb_type* target_buffer;
        
        if( postprocess_steps.size() )
            target_buffer = buffer_A.get();
        else
            target_buffer = &window.default_framebuffer();
        source_buffer = buffer_B.get();
        
        target_buffer -> alpha_blending(
            gl::alpha_blend_mode::PREMULTIPLIED_DROP_ALPHA
        );
        target_buffer -> bind();
        
        // Run all scene render steps against the same framebuffer
        for( auto step : scene_steps )
        {
            // Reset viewport to default before running step
            glViewport(
                0,
                0,
                target_buffer -> width(),
                target_buffer -> height()
            );
            
            step -> run( *target_buffer );
        }
        
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
                target_buffer = &window_buffer;
            
            target_buffer -> alpha_blending(
                gl::alpha_blend_mode::DISABLED
            );
            
            target_buffer -> bind();
            
            // Reset viewport to default before running step
            glViewport(
                0,
                0,
                target_buffer -> width(),
                target_buffer -> height()
            );
            
            step -> run(
                *static_cast< fb_type* >( source_buffer ),
                *                         target_buffer
            );
        }
        
        SDL_GL_SwapWindow( window.sdl_window );
        previous_time = current_time;
        
        // TODO: Frame timer for window should submit a new frame task
        return true;
    }
}