#pragma once
#ifndef YAVSG_ENGINE_SCENE_HPP
#define YAVSG_ENGINE_SCENE_HPP


#include "../rendering/material.hpp"
#include "../rendering/camera.hpp"
#include "../rendering/render_object_manager.hpp"
#include "../rendering/texture_reference.hpp"

#include <utility>  // std::size_t
#include <tuple>    // std::tuple_size


namespace yavsg
{
    class scene
    {
    protected:
        
        // Material structures /////////////////////////////////////////////////
        
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
        
    public:
        
        static const std::size_t material_texture_count = std::tuple_size<
            material_description::tuple_type
        >::value;
        
        using attribute_buffer_type = gl::attribute_buffer<
            vector< GLfloat, 3 >,    // position
            vector< GLfloat, 3 >,    // normal
            vector< GLfloat, 3 >,    // color
            vector< GLfloat, 2 >     // texture
        >;
        using vertex_type = attribute_buffer_type::tuple_type;
        using render_object_manager_type = render_object_manager<
            attribute_buffer_type,
            material_description
        >;
        
        // Scene description objects ///////////////////////////////////////////
        
        // These are thread-safe, so okay to expose them publicly
        
        render_object_manager_type object_manager;
        camera                     main_camera;
        
    public:
        scene();
        
        // DEVEL:
        void insert_model(
            const std::string& obj_filename,
            const std::string& obj_mtl_directory
        );
    };
}


#endif
