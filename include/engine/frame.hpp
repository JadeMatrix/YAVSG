#pragma once
#ifndef YAVSG_ENGINE_FRAME_HPP
#define YAVSG_ENGINE_FRAME_HPP


#include "../rendering/render_step.hpp"
#include "../engine/obj_render_step.hpp"
#include "../sdl/sdl_utils.hpp"
#include "../tasking/tasking.hpp"

#include <chrono>
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
        bool running;
        
        // TODO: OpenGL context should not be controlled by the frame task;
        // instead the frame task should be submitted by a window
        SDL_manager sdl;
        SDL_window_manager window;
        
        obj_render_step* ors;
        
        std::vector<      render_step     * > scene_steps;
        std::vector< postprocess_step_type* > postprocess_steps;
        
        std::chrono::high_resolution_clock::time_point    start_time;
        std::chrono::high_resolution_clock::time_point previous_time;
        
        fb_type* buffer_A;
        fb_type* buffer_B;
        
    public:
        frame_task();
        ~frame_task();
        task_flags_type flags() const;
        bool operator()();
    };
}


#endif
