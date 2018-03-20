#pragma once
#ifndef YAVSG_ENGINE_4UP_POSTPROCESS_STEP_HPP
#define YAVSG_ENGINE_4UP_POSTPROCESS_STEP_HPP


#include "../gl/attribute_buffer.hpp"
#include "../gl/framebuffer.hpp"
#include "../gl/shader_program.hpp"
#include "../gl/texture.hpp"
#include "../math/vector.hpp"
#include "../rendering/render_step.hpp"

#include <chrono>
#include <memory>   // std::unique_ptr
#include <string>


namespace yavsg
{
    class debug_4up_postprocess_step : public postprocess_step<
        gl::texture< GLfloat, 3 >
    >
    {
    public:
        using source_type = gl::framebuffer< gl::texture< GLfloat, 3 > >;
        
        using child_type = postprocess_step<
            gl::texture< GLfloat, 3 >
        >;
        
        std::unique_ptr< child_type > top_left;
        std::unique_ptr< child_type > top_right;
        std::unique_ptr< child_type > bottom_left;
        std::unique_ptr< child_type > bottom_right;
        
        debug_4up_postprocess_step(
            std::unique_ptr< child_type > top_left,
            std::unique_ptr< child_type > top_right,
            std::unique_ptr< child_type > bottom_left,
            std::unique_ptr< child_type > bottom_right
        );
        
        virtual void run(
            source_type               & source,
            gl::write_only_framebuffer& target
        );
    };
}


#endif
