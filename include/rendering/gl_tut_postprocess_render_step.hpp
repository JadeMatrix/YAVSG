#pragma once
#ifndef YAVSG_RENDERING_GL_TUT_POSTPROCESS_RENDER_STEP_HPP
#define YAVSG_RENDERING_GL_TUT_POSTPROCESS_RENDER_STEP_HPP


#include "render_step.hpp"
#include "../gl/shader_program.hpp"
#include "../math/vector.hpp"
#include "../gl/framebuffer.hpp"

#include <chrono>
#include <string>


namespace gl_tut
{
    class postprocess_render_step : public yavsg::postprocess_render_step
    {
    public:
        using program_type = yavsg::gl::shader_program<
            yavsg::vector< GLfloat, 2 >,
            yavsg::vector< GLfloat, 2 >
        >;
        
        program_type* postprocess_program;
        GLuint triangle_ebo;
        
        postprocess_render_step(
            const std::string& fragment_shader_filename
        );
        ~postprocess_render_step();
        virtual void run( yavsg::gl::framebuffer& source );
    };
}


#endif
