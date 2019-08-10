#pragma once
#ifndef YAVSG_WINDOWSYS_FRAME_HPP
#define YAVSG_WINDOWSYS_FRAME_HPP


#include "window.hpp"

#include <yavsg/events/event_listener.hpp>
#include <yavsg/rendering/render_step.hpp>
#include <yavsg/tasking/task.hpp>

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
        
        std::vector< std::unique_ptr<      render_step      > > scene_steps;
        std::vector< std::unique_ptr< postprocess_step_type > > postprocess_steps;
        
        std::chrono::high_resolution_clock::time_point    start_time;
        std::chrono::high_resolution_clock::time_point previous_time;
        
        std::unique_ptr< fb_type > buffer_A;
        std::unique_ptr< fb_type > buffer_B;
        
        std::vector< std::chrono::milliseconds >            frame_times;
        std::vector< std::chrono::milliseconds >::size_type current_frame_time;
        std::chrono::milliseconds                           latest_frame_time;
        float                                               latest_frame_rate;
        
    public:
        frame_task( std::shared_ptr< window_reference > );
        ~frame_task();
        
        task_flags_type flags() const;
        bool operator()();
    };
}


#endif
