#pragma once
#ifndef YAVSG_ENGINE_FRAME_HPP
#define YAVSG_ENGINE_FRAME_HPP


#include "../events/event_listener.hpp"
#include "../rendering/render_step.hpp"
#include "../tasking/task.hpp"
#include "../windowsys/window.hpp"

#include <chrono>
#include <memory>   // std::unique_ptr, std::shared_ptr
#include <vector>


namespace yavsg
{
    class frame_task : public task
    {
    public:
        using postprocess_step_type = postprocess_step<
            gl::texture< GLfloat, 3 >
        >;
        using fb_type = gl::framebuffer<
            gl::texture< GLfloat, 3 >
        >;
        using wo_fb_type = gl::write_only_framebuffer;
        
    protected:
        std::shared_ptr< window_reference > window_ref;
        
        std::vector<      render_step     * > scene_steps;
        std::vector< postprocess_step_type* > postprocess_steps;
        
        std::chrono::high_resolution_clock::time_point    start_time;
        std::chrono::high_resolution_clock::time_point previous_time;
        
        std::unique_ptr< fb_type > buffer_A;
        std::unique_ptr< fb_type > buffer_B;
        
    public:
        frame_task( std::shared_ptr< window_reference > );
        ~frame_task();
        
        task_flags_type flags() const;
        bool operator()();
    };
}


#endif
