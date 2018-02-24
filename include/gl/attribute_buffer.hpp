#pragma once
#ifndef YAVSG_GL_ATTRIBUTE_BUFFER_HPP
#define YAVSG_GL_ATTRIBUTE_BUFFER_HPP


#include "buffer_types.hpp"
#include "shader_program.hpp"

#include <tuple>
#include <vector>
#include <array>


namespace yavsg { namespace gl
{
    template< typename... Attributes > class attribute_buffer
    {
        friend class shader_program< Attributes... >;
        
    protected:
        GLuint gl_id;
        std::size_t vertex_count;
        
    public:
        using          tuple_type =     std::tuple< Attributes... >;
        using shader_program_type = shader_program< Attributes... >;
        
        attribute_buffer( const std::vector< tuple_type >& vertices );
        // template< std::size_t N > attribute_buffer(
        //     const std::array< tuple_type, N >& vertices
        // );
        
        ~attribute_buffer();
        
        void upload_data( const std::vector< tuple_type >& vertices );
        // template< std::size_t N > void upload_data(
        //     const std::array< tuple_type, N >& vertices
        // );
        
        std::size_t size() const;
    };
    
    class index_buffer
    {
        template< typename... Attributes > friend class shader_program;
        
    protected:
        GLuint gl_id;
        std::size_t index_count;
        
    public:
        index_buffer( const std::vector< GLuint >& indices );
        
        ~index_buffer();
        
        void upload_data( const std::vector< GLuint >& indices );
        
        std::size_t size() const;
    };
} }


namespace yavsg { namespace gl // Vertex buffer implementation /////////////////
{
    template< typename... Attributes >
    attribute_buffer< Attributes... >::attribute_buffer(
        const std::vector< tuple_type >& vertices
    )
    {
        glGenBuffers( 1, &gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't generate buffers for yavsg::gl::attribute_buffer"
        );
        upload_data( vertices );
    }
    
    template< typename... Attributes >
    attribute_buffer< Attributes... >::~attribute_buffer()
    {
        glDeleteBuffers( 1, &gl_id );
    }
    
    template< typename... Attributes >
    void attribute_buffer< Attributes... >::upload_data(
        const std::vector< tuple_type >& vertices
    )
    {
        glBindBuffer( GL_ARRAY_BUFFER, gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind buffer "
            + std::to_string( gl_id )
            + " for yavsg::gl::attribute_buffer::upload_data()"
        );
        
        void* elements_ptr = ( void* )&vertices[ 0 ];
        
        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof( tuple_type ),
            elements_ptr,
            GL_STATIC_DRAW // TODO: GL_DYNAMIC_DRAW, GL_STREAM_DRAW
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't upload "
            + std::to_string( vertices.size() )
            + " buffer elements at "
            + std::to_string( ( unsigned long )elements_ptr )
            + " for yavsg::gl::attribute_buffer::upload_data()"
        );
        
        vertex_count = vertices.size();
    }
    
    template< typename... Attributes >
    std::size_t attribute_buffer< Attributes... >::size() const
    {
        return vertex_count;
    }
} }


// TODO: in CPP
namespace yavsg { namespace gl // Index buffer implementation //////////////////
{
    inline index_buffer::index_buffer( const std::vector< GLuint >& indices )
    {
        glGenBuffers( 1, &gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't generate buffers for yavsg::gl::index_buffer"
        );
        upload_data( indices );
    }
    
    inline index_buffer::~index_buffer()
    {
        glDeleteBuffers( 1, &gl_id );
    }
    
    inline void index_buffer::upload_data( const std::vector< GLuint >& indices )
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind buffer "
            + std::to_string( gl_id )
            + " for yavsg::gl::index_buffer::upload_data()"
        );
        
        void* elements_ptr = ( void* )&indices[ 0 ];
        
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof( GLuint ),
            elements_ptr,
            GL_STATIC_DRAW // TODO: GL_DYNAMIC_DRAW, GL_STREAM_DRAW
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't upload "
            + std::to_string( indices.size() )
            + " buffer elements at "
            + std::to_string( ( unsigned long )elements_ptr )
            + " for yavsg::gl::index_buffer::upload_data()"
        );
        
        index_count = indices.size();
    }
    
    inline std::size_t index_buffer::size() const
    {
        return index_count;
    }
} }


#endif
