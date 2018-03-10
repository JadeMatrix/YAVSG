#pragma once
#ifndef YAVSG_RENDERING_OBJ_RENDER_STEP_HPP
#define YAVSG_RENDERING_OBJ_RENDER_STEP_HPP


#include "render_step.hpp"
#include "render_object_manager.hpp"
#include "material.hpp"
#include "../gl/attribute_buffer.hpp"
#include "../gl/shader_program.hpp"
#include "../gl/texture.hpp"
#include "../math/vector.hpp"

#include <chrono>
#include <string>
#include <vector>


namespace yavsg
{
    // TODO: std::optional<gl::texture>
    using material_description_base = material<
        gl::texture<>*,
        gl::texture<>*,
        gl::texture<>*
    >;
    class material_description : public material_description_base
    {
    public:
              gl::texture<>*    color_map()       { return std::get< 0 >( values ); }
              gl::texture<>*   normal_map()       { return std::get< 1 >( values ); }
              gl::texture<>* specular_map()       { return std::get< 2 >( values ); }
        const gl::texture<>*    color_map() const { return std::get< 0 >( values ); }
        const gl::texture<>*   normal_map() const { return std::get< 1 >( values ); }
        const gl::texture<>* specular_map() const { return std::get< 2 >( values ); }
        
        using material_description_base::material;
        
        material_description() : material_description_base(
            nullptr,
            nullptr,
            nullptr
        ) {}
        material_description( const material_description& o ) = delete;
        material_description( material_description&& o ) :
            material_description_base(
                o.   color_map(),
                o.  normal_map(),
                o.specular_map()
            )
        {
            o.values = { nullptr, nullptr, nullptr };
        }
        
        ~material_description()
        {
            if(    color_map() ) delete    color_map();
            if(   normal_map() ) delete   normal_map();
            if( specular_map() ) delete specular_map();
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
        using framebuffer_type = yavsg::gl::framebuffer<
            gl::texture< GLfloat, 3 >
        >;
        using program_type = yavsg::gl::shader_program<
            attribute_buffer_type,
            framebuffer_type
        >;
        using vertex_type = attribute_buffer_type::tuple_type;
        using render_object_manager_type = render_object_manager<
            attribute_buffer_type,
            material_description
        >;
        
        render_object_manager_type object_manager;
        
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point previous_time;
        program_type scene_program;
        
        obj_render_step(
            const std::string& obj_filename,
            const std::string& obj_mtl_directory
        );
        // ~obj_render_step();
        virtual void run();
    };
}


#endif
