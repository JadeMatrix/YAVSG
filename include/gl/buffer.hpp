#pragma once
#ifndef YAVSG_GL_BUFFER_HPP
#define YAVSG_GL_BUFFER_HPP


#include "_gl_base.hpp"
#include "error.hpp"
#include "../math/vector.hpp"


namespace yavsg { namespace gl
{
    template< typename Attributes... > class attribute_buffer
    {
    public:
        using tuple_type = std::tuple< Attributes... >;
        
    protected:
        GLuint gl_array_buffer_id;
        
        // template<
        //     unsigned int Nth,
        //     typename OfTraitsFirst,
        //     typename OfTraits...
        // >
        // void set_attribute(
        //     shader_program& program,
        //     const std::string& attribute_name,
        //     size_t stride,
        //     size_t offset
        // )
        // {
        //     set_attribute< Nth - 1, Traits... > set_attribute(
        //         program,
        //         attribute_name,
        //         stride,
        //         offset
        //     );
        // }
        
        // template<
        //     typename OfTraitsFirst,
        //     typename OfTraits...
        // >
        // void set_attribute< 0, OfTraitsFirst, OfTraits... >(
        //     shader_program& program,
        //     const std::string& attribute_name,
        //     size_t stride,
        //     size_t offset
        // )
        // {
        //     // GLint attribute_id = program.attribute( attribute_name );
        //     // glEnableVertexAttribArray( attribute_id );
        //     // glVertexAttribPointer(
        //     //     attribute_id,        // Data source
        //     //     3,                   // Components per element
        //     //     GL_FLOAT,            // Component type
        //     //     GL_FALSE,            // Components should be normalized
        //     //     stride,              // Component stride in bytes (0 = packed)
        //     //     offset               // Component offset within stride
        //     // );
        //     program.set_attribute< OfTraitsFirst >(
        //         attribute_name,
        //         stride,
        //         offset
        //     )
        // }
    
    public:
        attribute_buffer( tuple_type* element_array, size_t element_count )
        {
            glGenBuffers( 1, &gl_array_buffer_id );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't generate buffers for yavsg::gl::array_buffer"
            );
            set_data( element_array, element_count );
        }
        
        ~attribute_buffer()
        {
            glDeleteBuffers( 1, &gl_array_buffer_id );
        }
        
        void set_data( tuple_type* element_array, size_t element_count )
        {
            glBindBuffer( GL_ARRAY_BUFFER, gl_array_buffer_id );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't bind buffer "
                + std::to_string( gl_array_buffer_id )
                + " for yavsg::gl::array_buffer::set_data()"
            );
            glBufferData(
                GL_ARRAY_BUFFER,
                element_count * sizeof( tuple_type ),
                ( void* )element_array,
                GL_STATIC_DRAY // TODO: GL_DYNAMIC_DRAW, GL_STREAM_DRAW
            );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't upload "
                + std::to_string( element_count )
                + " buffer elements at "
                + std::to_string( ( unsigned long )element_array )
                + " for yavsg::gl::array_buffer::set_data()"
            );
        }
        
        // template< typename OfAttributesFirst, typename OfAttributes... >
        // constexpr
        // size_t total_size_of()
        // {
        //     return (
        //         sizeof( OfAttributesFirst )
        //         + total_size_of< OfAttributes... >()
        //     );
        // }
        
        // template< typename OfAttributesFirst, typename OfAttributesLast >
        // constexpr
        // size_t total_size_of< OfAttributesFirst, OfAttributesLast >()
        // {
        //     return sizeof( OfAttributesFirst ) + sizeof( OfAttributesLast );
        // }
        
        template<
            unsigned int Nth,
            typename OfAttributesFirst,
            typename OfAttributes...
        >
        static constexpr
        size_t offset_of()
        {
            return (
                offset_of< Nth - 1, OfAttributes... >()
                + sizeof( OfAttributesFirst )
            );
        }
        
        template< typename OfAttributes... >
        static constexpr
        size_t offset_of< 0, OfAttributes... >()
        {
            return 0;
        }
        
        // template< unsigned int Nth > static void set_program_attribute(
        //     shader_program< array_buffer< Attributes... > >& program,
        //     const std::string& attribute_name
        // )
        // {
        //     set_attribute< Nth, Attributes... > set_attribute(
        //         program,
        //         attribute_name,
        //         // total_size_of< Attributes... >(),
        //         sizeof( tuple_type ),
        //         ( void* )offset_of< Nth, Attributes... >()
        //     );
        // }
    }
    
    
    
    
    
    
    // template< class Traits > class array_buffer
    // {
    // public:
    //     using traits_type = Traits;
        
    // protected:
    //     GLuint gl_array_buffer_id;
        
    // public:
    //     array_buffer( traits_type* element_array, size_t element_count )
    //     {
    //         glGenBuffers( 1, &gl_array_buffer_id );
    //         YAVSG_GL_THROW_FOR_ERRORS(
    //             "couldn't generate buffers for yavsg::gl::array_buffer"
    //         );
    //         gset_data( element_array, element_count );
    //     }
        
    //     void set_data( traits_type* element_array, size_t element_count )
    //     {
    //         glBindBuffer( GL_ARRAY_BUFFER, gl_array_buffer_id );
    //         YAVSG_GL_THROW_FOR_ERRORS(
    //             "couldn't bind buffer "
    //             + std::to_string( gl_array_buffer_id )
    //             + " for yavsg::gl::array_buffer::set_data()"
    //         );
    //         glBufferData(
    //             GL_ARRAY_BUFFER,
    //             element_count * sizeof( traits_type ),
    //             ( void* )element_array,
    //             GL_STATIC_DRAY // TODO: GL_DYNAMIC_DRAW, GL_STREAM_DRAW
    //         );
    //         YAVSG_GL_THROW_FOR_ERRORS(
    //             "couldn't upload "
    //             + std::to_string( element_count )
    //             + " buffer elements at "
    //             + std::to_string( ( unsigned long )element_array )
    //             + " for yavsg::gl::array_buffer::set_data()"
    //         );
    //     }
    // };
    
    // template<
    //     class BufferType,
    //     typename T
    // > void set_buffer
} }


#endif
