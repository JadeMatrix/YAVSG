#pragma once
#ifndef YAVSG_RENDERING_GL_TUT_SCENE_RENDER_STEP_HPP
#define YAVSG_RENDERING_GL_TUT_SCENE_RENDER_STEP_HPP


#include "render_step.hpp"
#include "../gl/shader_program.hpp"
#include "../math/vector.hpp"

#include <chrono>


namespace gl_tut
{
    class scene_render_step : public yavsg::scene_render_step
    {
    public:
        using program_type = yavsg::gl::shader_program<
            yavsg::vector< GLfloat, 3 >,
            yavsg::vector< GLfloat, 3 >,
            yavsg::vector< GLfloat, 2 >
        >;
        
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point previous_time;
        program_type* scene_program;
        GLuint textures[ 2 ];
        
        scene_render_step();
        ~scene_render_step();
        virtual void run();
    };
}


#endif
