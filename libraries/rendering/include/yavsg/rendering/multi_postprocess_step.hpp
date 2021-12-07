#pragma once


#include "render_step.hpp"

#include <yavsg/gl/attribute_buffer.hpp>
#include <yavsg/gl/framebuffer.hpp>
#include <yavsg/gl/shader_program.hpp>
#include <yavsg/gl/shader.hpp>
#include <yavsg/gl/texture.hpp>
#include <yavsg/math/vector.hpp>

#include <string>
#include <vector>


namespace JadeMatrix::yavsg
{
    class multi_postprocess_step : public postprocess_step<
        gl::texture< GLfloat, 3 >
    >
    {
    public:
        multi_postprocess_step(
            std::vector< std::string > const& function_names
        );
        
        void run(
            gl::framebuffer< gl::texture< GLfloat, 3 > > const& source,
            gl::write_only_framebuffer                        & target
        ) override;
        
    protected:
        using attribute_buffer_type = gl::attribute_buffer<
            vector< GLfloat, 2 >,
            vector< GLfloat, 2 >
        >;
        
        attribute_buffer_type vertices;
        gl::index_buffer      indices;
        
        gl::shader_program<
            attribute_buffer_type,
            gl::framebuffer< gl::texture< GLfloat, 3 > >
        > multi_program;
        
        gl::shader generate_fragment_shader(
            std::vector< std::string > const& function_names
        );
    };
}
