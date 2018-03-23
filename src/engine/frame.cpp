#include "../../include/engine/frame.hpp"

#include "../../include/engine/4up_postprocess_step.hpp"
#include "../../include/engine/basic_postprocess_step.hpp"
#include "../../include/engine/dof_postprocess_step.hpp"
#include "../../include/engine/multi_postprocess_step.hpp"
#include "../../include/engine/obj_render_step.hpp"
#include "../../include/gl/framebuffer.hpp"
#include "../../include/tasking/tasking.hpp"
#include "../../include/tasking/utility_tasks.hpp"

#include <iostream> // std::cerr


namespace yavsg
{
    frame_task::frame_task( std::shared_ptr< window_reference > wr ) :
        window_ref{ wr },
        camera_look_listener{
            [ window_ref = this -> window_ref ](
                const SDL_MouseMotionEvent& e
            ){
                std::unique_lock< std::mutex > window_reference_lock(
                    window_ref -> reference_mutex
                );
                if( !window_ref -> window )
                    return;
                std::unique_lock< std::mutex > window_lock(
                    window_ref -> window -> state_mutex
                );
                
                window_ref -> window -> main_scene.main_camera.increment_pitch_yaw(
                    static_cast< float >( e.yrel ) / -4.0f,
                    static_cast< float >( e.xrel ) / -4.0f
                );
            }
        },
        focal_adjust_listener{
            [ window_ref = this -> window_ref ](
                const SDL_MouseWheelEvent& e
            ){
                std::unique_lock< std::mutex > window_reference_lock(
                    window_ref -> reference_mutex
                );
                if( !window_ref -> window )
                    return;
                std::unique_lock< std::mutex > window_lock(
                    window_ref -> window -> state_mutex
                );
                
                auto new_focal_point = (
                    static_cast< float >( e.y )
                    / 4.0f
                    + window_ref -> window -> main_scene.main_camera.focal_point()
                );
                auto near = window_ref -> window -> main_scene.main_camera.near_point();
                auto  far = window_ref -> window -> main_scene.main_camera. far_point();
                if( new_focal_point < near )
                    new_focal_point = near + 0.1f;
                else if( new_focal_point > far )
                    new_focal_point = far - 0.1f;
                window_ref -> window -> main_scene.main_camera.focal_point(
                    new_focal_point
                );
            }
        }
    {
        std::unique_lock< std::mutex > window_reference_lock(
            window_ref -> reference_mutex
        );
        
        if( window_ref -> window )
        {
            std::unique_lock< std::mutex > window_lock(
                window_ref -> window -> state_mutex
            );
            
            scene_steps = std::vector< render_step* >{
                new obj_render_step()
            };
            postprocess_steps = std::vector< postprocess_step_type* >{
                new debug_4up_postprocess_step(
                    nullptr,
                    std::make_unique< multi_postprocess_step >(
                        std::vector< std::string >{
                            "linear_to_sRGB",
                            "circular_gradient"
                        }
                    ),
                    std::make_unique< dof_postprocess_step >(
                        // FIXME: unsafe
                        window_ref -> window -> main_scene.main_camera
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
        return task_flag::GPU_THREAD;
    }
    
    bool frame_task::operator()()
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        
        std::unique_lock< std::mutex > window_reference_lock(
            window_ref -> reference_mutex
        );
        
        if( !window_ref -> window )
            return false;
        
        std::unique_lock< std::mutex > window_lock(
            window_ref -> window -> state_mutex
        );
        
        // Window is already locked, so no need to use `window::state()`
        auto window_state = window_ref -> window -> current_state;
        
        if( window_state.arranged == window_arrange_state::INVALID )
        {
            std::cerr << (
                "skipping frame because window isn't fully initialized\n"
            );
            // Requeue & check again later
            return true;
        }
        
        // Initialize buffers or update dimensions; only checks one because both
        // A and B's states are locked
        auto pixel_width  = static_cast< std::size_t >( to_pixels( window_state. width, window_state ) );
        auto pixel_height = static_cast< std::size_t >( to_pixels( window_state.height, window_state ) );
        if(
            !buffer_A
            || buffer_A ->  width() != pixel_width
            || buffer_A -> height() != pixel_height
        )
        {
            buffer_A = std::make_unique< fb_type >(
                pixel_width,
                pixel_height
            );
            buffer_B = std::make_unique< fb_type >(
                pixel_width,
                pixel_height
            );
        }
        
        // Even though only target_buffer needs to be a write-only
        // buffer, these both need to be the same type for std::swap()
        wo_fb_type* source_buffer;
        wo_fb_type* target_buffer;
        
        auto& window_buffer = *( window_ref -> window -> _default_framebuffer );
        
        if( postprocess_steps.size() )
            target_buffer = buffer_A.get();
        else
            target_buffer = &window_buffer;
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
            
            step -> run( window_ref -> window -> main_scene, *target_buffer );
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
        
        SDL_GL_SwapWindow( window_ref -> window -> sdl_window );
        previous_time = current_time;
        
        // TODO: Frame timer for window should submit a new frame task
        return true;
    }
}