#pragma once


#include "render_step.hpp"

#include <yavsg/gl/attribute_buffer.hpp>
#include <yavsg/gl/framebuffer.hpp>
#include <yavsg/gl/shader_program.hpp>
#include <yavsg/gl/texture.hpp>
#include <yavsg/math/vector.hpp>

#include <chrono>
#include <memory>   // std::unique_ptr
#include <string>


namespace JadeMatrix::yavsg
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
            const source_type         & source,
            gl::write_only_framebuffer& target
        );
    };
}
