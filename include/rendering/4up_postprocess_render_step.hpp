#pragma once
#ifndef YAVSG_RENDERING_4UP_POSTPROCESS_RENDER_STEP_HPP
#define YAVSG_RENDERING_4UP_POSTPROCESS_RENDER_STEP_HPP


#include "render_step.hpp"
#include "../gl/shader_program.hpp"
#include "../gl/framebuffer.hpp"
#include "../gl/attribute_buffer.hpp"
#include "../math/vector.hpp"

#include <chrono>
#include <string>


namespace yavsg
{
    class debug_4up_postprocess_render_step : public postprocess_step< 1 >
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
        
        program_type          postprocess_program;
        attribute_buffer_type vertices;
        
        postprocess_step< 1 >* top_left;
        postprocess_step< 1 >* top_right;
        postprocess_step< 1 >* bottom_left;
        postprocess_step< 1 >* bottom_right;
        
        yavsg::gl::index_buffer top_left_indices;
        yavsg::gl::index_buffer top_right_indices;
        yavsg::gl::index_buffer bottom_left_indices;
        yavsg::gl::index_buffer bottom_right_indices;
        
        debug_4up_postprocess_render_step(
            postprocess_step< 1 >* top_left,
            postprocess_step< 1 >* top_right,
            postprocess_step< 1 >* bottom_left,
            postprocess_step< 1 >* bottom_right
        );
        ~debug_4up_postprocess_render_step();
        virtual void run( framebuffer_type& source );
    };
}


#endif
