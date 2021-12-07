#pragma once


#include "material.hpp"
#include "camera.hpp"
#include "render_object_manager.hpp"
#include "texture_reference.hpp"

#include <utility>  // size_t
#include <tuple>    // tuple_size_v


namespace JadeMatrix::yavsg
{
    class scene
    {
    public:
        
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
            material_texture_type      &    color_map()       { return std::get< 0 >( values ); }
            material_texture_type      &   normal_map()       { return std::get< 1 >( values ); }
            material_texture_type      & specular_map()       { return std::get< 2 >( values ); }
            material_texture_type const&    color_map() const { return std::get< 0 >( values ); }
            material_texture_type const&   normal_map() const { return std::get< 1 >( values ); }
            material_texture_type const& specular_map() const { return std::get< 2 >( values ); }
            
            using material_description_base::material;
            
            material_description() : material_description_base( {}, {}, {} ) {}
            material_description(
                material_texture_type& c,
                material_texture_type& n,
                material_texture_type& s
            ) : material_description_base( c, n, s ) {}
            material_description( material_description const& o ) = delete;
            material_description( material_description&& o ) :
                material_description_base(
                    std::move( std::get< 0 >( o.values ) ),
                    std::move( std::get< 1 >( o.values ) ),
                    std::move( std::get< 2 >( o.values ) )
                )
            {}
        };
        
        static constexpr auto material_texture_count = std::tuple_size_v<
            material_description::tuple_type
        >;
        
        // Vertex structures ///////////////////////////////////////////////////
        
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
        
        scene();
    };
}
