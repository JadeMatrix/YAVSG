#pragma once
#ifndef YAVSG_GL_BUFFER_TYPES_HPP
#define YAVSG_GL_BUFFER_TYPES_HPP


#include "_gl_base.hpp"
#include "../math/vector.hpp"


namespace yavsg { namespace gl
{
    template< typename... Attributes > class shader_program;
    template< typename... Attributes > class vertex_buffer;
} }


namespace yavsg { namespace gl // Attribute traits /////////////////////////////
{
    template< typename T > struct attribute_traits {};
    
    template<> struct attribute_traits< GLfloat >
    {
        static const GLint  components_per_element = 1;
        static const GLenum component_type         = GL_FLOAT;
    };
    
    template<> struct attribute_traits< GLint >
    {
        static const GLint  components_per_element = 1;
        static const GLenum component_type         = GL_INT;
    };
    
    // TODO: more basic types, see
    //      https://www.khronos.org/opengl/wiki/OpenGL_Type
    //      http://docs.gl/gl3/glVertexAttribPointer
    
    template< typename T, unsigned int D >
    struct attribute_traits< vector< T, D > >
    {
        static const GLint components_per_element
            = attribute_traits< T >::components_per_element * D;
        static const GLenum component_type
            = attribute_traits< T >::component_type;
    };
} }


namespace yavsg { namespace gl // Helpers to make templating simpler ///////////
{
    // A recent version of Clang apparently has a bug dealing with template
    // specializations, so disable this check; see
    // https://github.com/ryanrhymes/eigen/issues/2
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Winvalid-partial-specialization"
    
    template<
        unsigned int Nth,
        typename OfAttributesFirst,
        typename... OfAttributes
    > struct offset_of
    {
        static const std::size_t bytes = (
            sizeof( OfAttributesFirst )
            + offset_of< Nth - 1, OfAttributes... >::bytes
        );
    };
    
    template< typename... OfAttributes >
    struct offset_of< 0, OfAttributes... >
    {
        static const std::size_t bytes = 0;
    };
    
    #pragma clang diagnostic pop
} }


#endif
