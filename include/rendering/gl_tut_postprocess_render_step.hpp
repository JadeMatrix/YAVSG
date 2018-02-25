#pragma once
#ifndef YAVSG_RENDERING_GL_TUT_POSTPROCESS_RENDER_STEP_HPP
#define YAVSG_RENDERING_GL_TUT_POSTPROCESS_RENDER_STEP_HPP


#include "render_step.hpp"
#include "../gl/shader_program.hpp"
#include "../gl/framebuffer.hpp"
#include "../gl/attribute_buffer.hpp"
#include "../math/vector.hpp"

#include <chrono>
#include <string>


namespace gl_tut
{
    class postprocess_render_step : public yavsg::postprocess_render_step< 1 >
    {
    public:
        using attribute_buffer_type = yavsg::gl::attribute_buffer<
            yavsg::vector< GLfloat, 2 >,
            yavsg::vector< GLfloat, 2 >
        >;
        using framebuffer_type = yavsg::gl::framebuffer< 1 >;
        using program_type = yavsg::gl::shader_program<
            attribute_buffer_type,
            framebuffer_type
        >;
        
        program_type            postprocess_program;
        attribute_buffer_type   vertices;
        yavsg::gl::index_buffer indices;
        
        postprocess_render_step(
            const std::string& fragment_shader_filename
        );
        // ~postprocess_render_step();
        virtual void run( framebuffer_type& source );
    };
}


#endif
