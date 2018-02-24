#pragma once
#ifndef YAVSG_RENDERING_OBJ_RENDER_STEP_HPP
#define YAVSG_RENDERING_OBJ_RENDER_STEP_HPP


#include "render_step.hpp"
#include "../gl/shader_program.hpp"
#include "../math/vector.hpp"

#include <chrono>
#include <string>
#include <vector>


namespace yavsg
{
    class obj_render_step : public scene_render_step
    {
    public:
        using program_type = yavsg::gl::shader_program<
            yavsg::vector< GLfloat, 3 >,    // position
            yavsg::vector< GLfloat, 3 >,    // color
            yavsg::vector< GLfloat, 2 >     // texture
        >;
        using vertex_type = program_type::tuple_type;
        
        struct render_group
        {
            bool has_color_map; // TODO: std::optional< GLuint >
            GLuint color_map;
            // GLuint normal_map;
            // GLuint specular_map;
            // GLuint mask_map;
            
            std::vector< vertex_type > vertices;
        };
        
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point previous_time;
        std::vector< render_group > render_groups;
        program_type* scene_program;
        GLfloat obj_max_x = 0.0f;
        GLfloat obj_min_x = 0.0f;
        GLfloat obj_max_y = 0.0f;
        GLfloat obj_min_y = 0.0f;
        GLfloat obj_max_z = 0.0f;
        GLfloat obj_min_z = 0.0f;
        square_matrix< GLfloat, 4 > model_scale_transform;
        
        obj_render_step(
            const std::string& obj_filename,
            const std::string& obj_mtl_directory
        );
        ~obj_render_step();
        virtual void run();
    };
}


#endif
