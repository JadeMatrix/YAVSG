#pragma once
#ifndef YAVSG_RENDERING_DOF_POSTPROCESS_STEP_HPP
#define YAVSG_RENDERING_DOF_POSTPROCESS_STEP_HPP


#include "render_step.hpp"
#include "camera.hpp"
#include "../gl/attribute_buffer.hpp"
#include "../gl/framebuffer.hpp"
#include "../gl/shader_program.hpp"
#include "../gl/texture.hpp"
#include "../math/vector.hpp"


namespace yavsg
{
    class dof_postprocess_step : public postprocess_step<
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
        yavsg::gl::index_buffer indices;
        
        const camera& scene_camera;
        
        dof_postprocess_step( const camera& );
        virtual void run(
            source_type               & source,
            gl::write_only_framebuffer& target
        );
    };
}


#endif
