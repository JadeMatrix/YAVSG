#pragma once
#ifndef YAVSG_RENDERING_OBJ_RENDER_STEP_HPP
#define YAVSG_RENDERING_OBJ_RENDER_STEP_HPP


#include "render_step.hpp"
#include "render_object_manager.hpp"
#include "../gl/shader_program.hpp"
#include "../gl/attribute_buffer.hpp"
#include "../gl/texture.hpp"
#include "../math/vector.hpp"

#include <chrono>
#include <string>
#include <vector>


namespace yavsg
{
    class material_description
    {
    public:
        // TODO: std::optional< gl::texture >
        gl::texture*    color_map = nullptr;
        gl::texture*   normal_map = nullptr;
        gl::texture* specular_map = nullptr;
        
        material_description() {}
        material_description( const material_description& o ) = delete;
        material_description( material_description&& o )
        {
            std::swap(    color_map, o.   color_map );
            std::swap(   normal_map, o.  normal_map );
            std::swap( specular_map, o.specular_map );
        }
        
        ~material_description()
        {
            // Won't be necessary using std::optional
            if(    color_map ) delete    color_map;
            if(   normal_map ) delete   normal_map;
            if( specular_map ) delete specular_map;
        }
    };
    
    class obj_render_step : public render_step
    {
    public:
        using attribute_buffer_type = yavsg::gl::attribute_buffer<
            yavsg::vector< GLfloat, 3 >,    // position
            yavsg::vector< GLfloat, 3 >,    // normal
            yavsg::vector< GLfloat, 3 >,    // color
            yavsg::vector< GLfloat, 2 >     // texture
        >;
        using framebuffer_type = yavsg::gl::framebuffer< 1 >;
        using program_type = yavsg::gl::shader_program<
            attribute_buffer_type,
            framebuffer_type
        >;
        using vertex_type = attribute_buffer_type::tuple_type;
        
        render_object_manager<
            attribute_buffer_type,
            material_description
        > object_manager;
        
        struct render_group
        {
            // TODO: std::optional< gl::texture >
            gl::texture* color_map;
            gl::texture* normal_map;
            gl::texture* specular_map;
            gl::texture* mask_map;
            
            std::vector< GLuint > temp_index_storage;
            gl::index_buffer* vertex_indices;
        };
        
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point previous_time;
        program_type scene_program;
        square_matrix< GLfloat, 4 > model_scale_transform;
        
        obj_render_step(
            const std::string& obj_filename,
            const std::string& obj_mtl_directory
        );
        // ~obj_render_step();
        virtual void run();
    };
}


#endif
