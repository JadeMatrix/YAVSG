#pragma once
#ifndef YAVSG_ENGINE_OBJ_RENDER_STEP_HPP
#define YAVSG_ENGINE_OBJ_RENDER_STEP_HPP


#include "../engine/scene.hpp"
#include "../gl/attribute_buffer.hpp"
#include "../gl/shader_program.hpp"
#include "../gl/texture.hpp"
#include "../math/vector.hpp"
#include "../rendering/render_step.hpp"

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
        
        // TODO: Move
        scene obj_scene;
        
        obj_render_step();
        virtual void run( gl::write_only_framebuffer& target );
    };
}


#endif
