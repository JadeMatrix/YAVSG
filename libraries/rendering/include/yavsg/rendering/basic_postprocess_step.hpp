#pragma once


#include "render_step.hpp"

#include <yavsg/gl/attribute_buffer.hpp>
#include <yavsg/gl/framebuffer.hpp>
#include <yavsg/gl/shader_program.hpp>
#include <yavsg/gl/texture.hpp>
#include <yavsg/math/vector.hpp>

#include <string>


namespace JadeMatrix::yavsg
{
    class basic_postprocess_step : public postprocess_step<
        gl::texture< GLfloat, 3 >
    >
    {
    public:
        using attribute_buffer_type = gl::attribute_buffer<
            vector< GLfloat, 2 >,
            vector< GLfloat, 2 >
        >;
        using source_type = gl::framebuffer< gl::texture< GLfloat, 3 > >;
        using program_type = gl::shader_program<
            attribute_buffer_type,
            source_type
        >;
        
        program_type            postprocess_program;
        attribute_buffer_type   vertices;
        gl::index_buffer indices;
        
        basic_postprocess_step(
            const std::string& fragment_shader_filename
        );
        virtual void run(
            const source_type         & source,
            gl::write_only_framebuffer& target
        );
    };
}
