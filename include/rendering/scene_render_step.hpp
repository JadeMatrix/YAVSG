#pragma once
#ifndef YAVSG_RENDERING_SCENE_RENDER_STEP_HPP
#define YAVSG_RENDERING_SCENE_RENDER_STEP_HPP


#include "render_step.hpp"
#include "../gl/shader_program.hpp"
#include "../gl/attribute_buffer.hpp"
#include "../gl/texture.hpp"
#include "../math/vector.hpp"
#include "../math/quaternion.hpp"

#include <string>
#include <vector>


namespace yavsg
{
    class scene_render_step : public render_step
    {
    public:
        using attribute_buffer_type = yavsg::gl::attribute_buffer<
            vector< GLfloat, 3 >,   // position XYZ
            vector< GLfloat, 3 >,   // normal   XYZ
            vector< GLfloat, 3 >,   // tangent  XYZ
            vector< GLfloat, 3 >,   // color    RGB
            vector< GLfloat, 2 >    // texture  UV
        >;
        using framebuffer_type = yavsg::gl::framebuffer< 1 >;
        using program_type = yavsg::gl::shader_program<
            attribute_buffer_type,
            framebuffer_type
        >;
        using vertex_type = attribute_buffer_type::tuple_type;
        
        struct render_group
        {
            struct material
            {
                // TODO: std::optional< gl::texture >
                gl::texture* color_map;
                gl::texture* normal_map;
                gl::texture* specular_map;
                // gl::texture* alpha_map;
            } material;
            gl::index_buffer vertex_indices;
        };
        
        struct object
        {
            program_type::attribute_buffer_type* vertices;
            std::vector< render_group > render_groups;
            
            struct
            {
                vector< GLfloat, 3 > max_corner;
                vector< GLfloat, 3 > min_corner;
            } bounding_box;
            
            // Basic transforms; applied scale then rotation then position
            vector< GLfloat, 3 > position;
            vector< GLfloat, 3 > scale;
            versor< GLfloat > rotation;
        };
        
        program_type scene_program;
        std::vector< object > objects;
        
        scene_render_step(
            const std::string&   vertex_shader_filename,
            const std::string& fragment_shader_filename
        );
        virtual void update() = 0;
        virtual void run();
    };
}


#endif
