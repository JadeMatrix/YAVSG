#pragma once
#ifndef YAVSG_RENDERING_OBJ_RENDER_STEP_HPP
#define YAVSG_RENDERING_OBJ_RENDER_STEP_HPP


#include "../gl/attribute_buffer.hpp"
#include "../gl/shader_program.hpp"
#include "../gl/texture.hpp"
#include "../math/vector.hpp"
#include "../rendering/camera.hpp"
#include "../rendering/material.hpp"
#include "../rendering/render_object_manager.hpp"
#include "../rendering/render_step.hpp"
#include "../rendering/texture_reference.hpp"

#include <string>
#include <vector>


namespace yavsg
{
    using material_texture_type = texture_reference< GLfloat, 4 >;
    
    using material_description_base = material<
        material_texture_type,
        material_texture_type,
        material_texture_type
    >;
    
    class material_description : public material_description_base
    {
    public:
              material_texture_type&    color_map()       { return std::get< 0 >( values ); }
              material_texture_type&   normal_map()       { return std::get< 1 >( values ); }
              material_texture_type& specular_map()       { return std::get< 2 >( values ); }
        const material_texture_type&    color_map() const { return std::get< 0 >( values ); }
        const material_texture_type&   normal_map() const { return std::get< 1 >( values ); }
        const material_texture_type& specular_map() const { return std::get< 2 >( values ); }
        
        using material_description_base::material;
        
        material_description() : material_description_base( {}, {}, {} ) {}
        material_description(
            material_texture_type& c,
            material_texture_type& n,
            material_texture_type& s
        ) : material_description_base( c, n, s ) {}
        material_description( const material_description& o ) = delete;
        material_description( material_description&& o ) :
            material_description_base(
                std::move( std::get< 0 >( o.values ) ),
                std::move( std::get< 1 >( o.values ) ),
                std::move( std::get< 2 >( o.values ) )
            )
        {}
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
        
        program_type scene_program;
        
        camera main_camera;
        
        obj_render_step(
            const std::string& obj_filename,
            const std::string& obj_mtl_directory
        );
        // ~obj_render_step();
        virtual void run( gl::write_only_framebuffer& target );
    };
}


#endif
