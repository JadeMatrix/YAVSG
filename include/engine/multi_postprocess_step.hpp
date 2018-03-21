#pragma once
#ifndef YAVSG_ENGINE_MULTI_POSTPROCESS_STEP_HPP
#define YAVSG_ENGINE_MULTI_POSTPROCESS_STEP_HPP


#include "../gl/attribute_buffer.hpp"
#include "../gl/framebuffer.hpp"
#include "../gl/shader_program.hpp"
#include "../gl/shader.hpp"
#include "../gl/texture.hpp"
#include "../math/vector.hpp"
#include "../rendering/render_step.hpp"

#include <string>
#include <vector>


namespace yavsg
{
    class multi_postprocess_step : public postprocess_step<
        gl::texture< GLfloat, 3 >
    >
    {
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
            const std::vector< std::string > function_names
        );
        
    public:
        multi_postprocess_step(
            const std::vector< std::string > function_names
        );
        
        virtual void run(
            const gl::framebuffer< gl::texture< GLfloat, 3 > >& source,
                  gl::write_only_framebuffer                  & target
        );
    };
}


#endif
