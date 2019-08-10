#include <yavsg/windowsys/frame.hpp>

#include <yavsg/gl/framebuffer.hpp>
#include <yavsg/rendering/4up_postprocess_step.hpp>
#include <yavsg/rendering/basic_postprocess_step.hpp>
#include <yavsg/rendering/dof_postprocess_step.hpp>
#include <yavsg/rendering/multi_postprocess_step.hpp>
#include <yavsg/rendering/obj_render_step.hpp>
#include <yavsg/tasking/tasking.hpp>
#include <yavsg/tasking/utility_tasks.hpp>

#include <iostream> // std::cerr
#include <numeric>  // std::accumulate


namespace yavsg
{
    frame_task::frame_task( std::shared_ptr< window_reference > wr ) :
        window_ref{ wr },
        frame_times{ 10, std::chrono::milliseconds{ 0 } },
        current_frame_time{ frame_times.size() }
    {
        std::unique_lock< std::mutex > window_reference_lock(
            window_ref -> reference_mutex
        );
        
        if( window_ref -> window )
        {
            std::unique_lock< std::mutex > window_lock(
                window_ref -> window -> state_mutex
            );
            
            scene_steps.push_back( std::make_unique< obj_render_step >() );
            postprocess_steps.push_back(
                std::make_unique< debug_4up_postprocess_step >(
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
                )
            );
            // postprocess_steps.push_back(
            //     std::make_unique< dof_postprocess_step >(
            //         // FIXME: unsafe
            //         window_ref -> window -> main_scene.main_camera
            //     )
            // );
            // postprocess_steps.push_back(
            //     std::make_unique< multi_postprocess_step >(
            //         std::vector< std::string >{ "linear_to_sRGB" }
            //     )
            // );
            
            start_time = std::chrono::high_resolution_clock::now();
            previous_time = start_time;
        }
    }
    
    frame_task::~frame_task()
    {
        
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
        for( auto& step : scene_steps )
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
            auto& step = *( postprocess_step_iter -> get() );
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
            
            step.run(
                *static_cast< fb_type* >( source_buffer ),
                *                         target_buffer
            );
        }
        
        SDL_GL_SwapWindow( window_ref -> window -> sdl_window );
        
        if( ++current_frame_time >= frame_times.size() )
            current_frame_time = 0;
        frame_times[ current_frame_time ] = std::chrono::duration_cast<
            std::chrono::milliseconds
        >( current_time - previous_time );
        latest_frame_time = std::accumulate(
            frame_times.begin(),
            frame_times.  end(),
            std::chrono::milliseconds{ 0 }
        ) / frame_times.size();
        latest_frame_rate = 1000.0f / latest_frame_time.count();
        
        // DEBUG:
        if( current_frame_time == 0 )
            std::cout << (
                "current framerate: "
                + std::to_string( latest_frame_rate )
                + "\n"
            );
        
        previous_time = current_time;
        
        // TODO: Frame timer for window should submit a new frame task
        return true;
    }
}