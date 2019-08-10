#pragma once
#ifndef YAVSG_RENDERING_OBJ_RENDER_STEP_HPP
#define YAVSG_RENDERING_OBJ_RENDER_STEP_HPP


#include "render_step.hpp"
#include "scene.hpp"

#include <yavsg/gl/attribute_buffer.hpp>
#include <yavsg/gl/shader_program.hpp>
#include <yavsg/gl/texture.hpp>
#include <yavsg/math/vector.hpp>

#include <string>


namespace yavsg
{
    class obj_render_step : public render_step
    {
    protected:
        // TODO: get rid of thise once shader programs & VAOs are separate
        bool first_run;
        
    public:
        using program_type = gl::shader_program<
            scene::attribute_buffer_type,
            gl::framebuffer< gl::texture< GLfloat, 3 > >
        >;
        
        program_type scene_program;
        
        obj_render_step();
        virtual void run( const scene&, gl::write_only_framebuffer& );
    };
}


#endif
