#pragma once
#ifndef YAVSG_ENGINE_BASIC_POSTPROCESS_STEP_HPP
#define YAVSG_ENGINE_BASIC_POSTPROCESS_STEP_HPP


#include "../gl/attribute_buffer.hpp"
#include "../gl/framebuffer.hpp"
#include "../gl/shader_program.hpp"
#include "../gl/texture.hpp"
#include "../math/vector.hpp"
#include "../rendering/render_step.hpp"

#include <string>


namespace yavsg
{
    class basic_postprocess_step : public yavsg::postprocess_step<
        yavsg::gl::texture< GLfloat, 3 >
    >
    {
    public:
        using attribute_buffer_type = yavsg::gl::attribute_buffer<
            yavsg::vector< GLfloat, 2 >,
            yavsg::vector< GLfloat, 2 >
        >;
        using source_type = yavsg::gl::framebuffer<
            yavsg::gl::texture< GLfloat, 3 >
        >;
        using program_type = yavsg::gl::shader_program<
            attribute_buffer_type,
            source_type
        >;
        
        program_type            postprocess_program;
        attribute_buffer_type   vertices;
        yavsg::gl::index_buffer indices;
        
        basic_postprocess_step(
            const std::string& fragment_shader_filename
        );
        virtual void run(
            const source_type         & source,
            gl::write_only_framebuffer& target
        );
    };
}


#endif
