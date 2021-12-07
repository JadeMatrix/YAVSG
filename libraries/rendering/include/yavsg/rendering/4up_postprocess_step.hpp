#pragma once


#include "render_step.hpp"

#include <yavsg/gl/attribute_buffer.hpp>
#include <yavsg/gl/framebuffer.hpp>
#include <yavsg/gl/shader_program.hpp>
#include <yavsg/gl/texture.hpp>
#include <yavsg/math/vector.hpp>

#include <chrono>
#include <memory>   // unique_ptr


namespace JadeMatrix::yavsg
{
    class debug_4up_postprocess_step : public postprocess_step<
        gl::texture< GLfloat, 3 >
    >
    {
    public:
        using source_type = gl::framebuffer< gl::texture< GLfloat, 3 > >;
        
        using child_type = postprocess_step< gl::texture< GLfloat, 3 > >;
        
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
        
        void run(
            source_type          const& source,
            gl::write_only_framebuffer& target
        ) override;
    };
}
