#pragma once
#ifndef GL_RENDER_STEPS_HPP
#define GL_RENDER_STEPS_HPP


#include "gl_utils.hpp"

#include <chrono>


namespace gl
{
    class scene_render_step : public render_step
    {
    public:
        // shader_program* scene_program;
        // GLuint triangle_vbo;
        
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point previous_time;
        shader_program* scene_program;
        GLuint triangle_vbo;
        GLuint textures[ 2 ];
        
        scene_render_step();
        ~scene_render_step();
        void run( framebuffer& previous_framebuffer );
    };
    
    class postprocess_render_step : public render_step
    {
    public:
        shader_program* postprocess_program;
        GLuint triangle_vbo;
        GLuint triangle_ebo;
        
        postprocess_render_step();
        ~postprocess_render_step();
        void run( framebuffer& previous_framebuffer );
    };
    
    class compute_render_step : public render_step
    {
    public:
        shader_program* compute_program;
        GLuint data_vbo;
        GLuint result_vbo;
        GLuint query;
        
        compute_render_step();
        ~compute_render_step();
        void run( framebuffer& previous_framebuffer );
    };
}


#endif
