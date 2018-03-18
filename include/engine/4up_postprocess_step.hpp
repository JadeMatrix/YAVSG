#pragma once
#ifndef YAVSG_RENDERING_4UP_POSTPROCESS_STEP_HPP
#define YAVSG_RENDERING_4UP_POSTPROCESS_STEP_HPP


#include "../gl/attribute_buffer.hpp"
#include "../gl/framebuffer.hpp"
#include "../gl/shader_program.hpp"
#include "../gl/texture.hpp"
#include "../math/vector.hpp"
#include "../rendering/render_step.hpp"

#include <chrono>
#include <string>


namespace yavsg
{
    class debug_4up_postprocess_step : public postprocess_step<
        gl::texture< GLfloat, 3 >
    >
    {
    public:
        using attribute_buffer_type = gl::attribute_buffer<
            yavsg::vector< GLfloat, 2 >,
            yavsg::vector< GLfloat, 2 >
        >;
        using source_type = gl::framebuffer< gl::texture< GLfloat, 3 > >;
        using program_type = gl::shader_program<
            attribute_buffer_type,
            source_type
        >;
        
        using child_type = postprocess_step<
            gl::texture< GLfloat, 3 >
        >;
        
        program_type          postprocess_program;
        attribute_buffer_type vertices;
        
        child_type* top_left;
        child_type* top_right;
        child_type* bottom_left;
        child_type* bottom_right;
        
        gl::index_buffer top_left_indices;
        gl::index_buffer top_right_indices;
        gl::index_buffer bottom_left_indices;
        gl::index_buffer bottom_right_indices;
        
        source_type* sub_buffer;
        
        debug_4up_postprocess_step(
            child_type* top_left,
            child_type* top_right,
            child_type* bottom_left,
            child_type* bottom_right
        );
        ~debug_4up_postprocess_step();
        virtual void run(
            source_type               & source,
            gl::write_only_framebuffer& target
        );
    };
}


#endif
