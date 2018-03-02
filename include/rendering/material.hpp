#pragma once
#ifndef YAVSG_RENDERING_MATERIAL_HPP
#define YAVSG_RENDERING_MATERIAL_HPP


#include "../gl/shader_program.hpp"
#include "../gl/texture.hpp"

#include <array>
#include <string>
#include <tuple>
#include <utility>  // std::size_t


// When setting up materials for a render pass, it's (purposefully) not possible
// to loop over active-texture IDs with texture::bind_as<>(), as this function
// checks active-texture ID bounds at compile-time.  This material descriptor
// metaclass provides a bind() method that takes an array of names to use for
// binding material variables, chosing IDs at compile time as they aren't 
// meaningful in themselves anyways.


namespace yavsg // Binding attributes //////////////////////////////////////////
{
    template< typename T, class AttributeBuffer, class Framebuffer >
    struct bind_attributes
    {
        static const std::size_t increment_active_texture = 0;
        
        template< std::size_t ActiveTexture >
        static void asdfasdfasdf(
            gl::shader_program< AttributeBuffer, Framebuffer >& program,
            const std::string& name,
            const T& value
        )
        {
            program.template set_uniform< T >( name, value );
        }
    };
    
    // TODO: std::optional< gl::texture >, or just texture ref type
    template< class AttributeBuffer, class Framebuffer >
    struct bind_attributes< gl::texture, AttributeBuffer, Framebuffer >
    {
        static const std::size_t increment_active_texture = 1;
        
        template< std::size_t ActiveTexture >
        static void asdfasdfasdf(
            gl::shader_program< AttributeBuffer, Framebuffer >& program,
            const std::string& name,
            const gl::texture& texture
        )
        {
            texture.bind_as< ActiveTexture >();
            program.template set_uniform< GLint >( name, ActiveTexture );
        }
    };
    
    template< class AttributeBuffer, class Framebuffer >
    struct bind_attributes< gl::texture*, AttributeBuffer, Framebuffer >
    {
        static const std::size_t increment_active_texture = 1;
        
        template< std::size_t ActiveTexture >
        static void asdfasdfasdf(
            gl::shader_program< AttributeBuffer, Framebuffer >& program,
            const std::string& name,
            const gl::texture* texture
        )
        {
            if( texture )
            {
                texture -> bind_as< ActiveTexture >();
                program.template set_uniform< GLint >( name, ActiveTexture );
            }
            else
                gl::unbind_texture< ActiveTexture >();
        }
    };
    
    template<
        std::size_t FirstActiveTexture,
        std::size_t TupleIndex,
        class TupleType,
        class AttributeBuffer,
        class Framebuffer
    >
    struct bind_material_values
    {
        static void bind(
            gl::shader_program< AttributeBuffer, Framebuffer >& program,
            const std::array<
                std::string,
                std::tuple_size< TupleType >::value
            >& names,
            const TupleType& values
        )
        {
            using value_type = typename std::tuple_element<
                TupleIndex - 1,
                TupleType
            >::type;
            using bind_attributes_type = bind_attributes<
                value_type,
                AttributeBuffer,
                Framebuffer
            >;
            
            bind_material_values<
                (
                    FirstActiveTexture
                    + bind_attributes_type::increment_active_texture
                ),
                TupleIndex - 1,
                TupleType,
                AttributeBuffer,
                Framebuffer
            >::bind( program, names, values );
            
            bind_attributes_type::template asdfasdfasdf< FirstActiveTexture >(
                program,
                std::get< TupleIndex - 1 >( names ),
                std::get< TupleIndex - 1 >( values )
            );
        }
    };
    
    template<
        std::size_t FirstActiveTexture,
        class TupleType,
        class AttributeBuffer,
        class Framebuffer
    >
    struct bind_material_values<
        FirstActiveTexture,
        0,
        TupleType,
        AttributeBuffer,
        Framebuffer
    >
    {
        static void bind(
            gl::shader_program< AttributeBuffer, Framebuffer >& program,
            const std::array<
                std::string,
                std::tuple_size< TupleType >::value
            >& names,
            const TupleType& values
        ) {}
    };
}


namespace yavsg // Material descriptor /////////////////////////////////////////
{
    template< typename... Attributes > class material
    {
    public:
        using tuple_type = std::tuple< Attributes... >;
        
    protected:
        tuple_type values;
        
    public:
        
        material( Attributes... args ) : values( args... ) {}
        material( material&& o ) : values( std::move( o.values ) ) {}
        
        virtual ~material() {}
        
        // TODO: "starting at ID" parameter so more than one material can be
        // bound at once
        template< class AttributeBuffer, class Framebuffer >
        void bind(
            gl::shader_program< AttributeBuffer, Framebuffer >& program,
            const std::array< std::string, sizeof...( Attributes ) >& names
        ) const
        {
            bind_material_values<
                0,
                sizeof...( Attributes ),
                tuple_type,
                AttributeBuffer,
                Framebuffer
            >::bind( program, names, values );
        }
    };
}


#endif
