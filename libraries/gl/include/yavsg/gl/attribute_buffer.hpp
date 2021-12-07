#pragma once


#include <yavsg/gl_wrap.hpp>

#include "error.hpp"

#include <doctest/doctest.h>    // REQUIRE

#include <array>
#include <cstdint>  // uintptr_t
#include <limits>
#include <tuple>
#include <vector>


namespace JadeMatrix::yavsg::gl
{
    template< typename... Attributes > class attribute_buffer
    {
        // friend class shader_program< Attributes... >;
        
    public:
        using tuple_type = std::tuple< Attributes... >;
        
        attribute_buffer( std::vector< tuple_type > const& vertices );
        // template< std::size_t N > attribute_buffer(
        //     std::array< tuple_type, N > const& vertices
        // );
        attribute_buffer( attribute_buffer&& o );
        
        ~attribute_buffer();
        
        void upload_data( std::vector< tuple_type > const& vertices );
        // template< std::size_t N > void upload_data(
        //     std::array< tuple_type, N > const& vertices
        // );
        
        std::size_t size() const;
        bool empty() const;
        // TODO: Determine if there's a more opaque way to access this
        GLuint gl_buffer_id() const;
        
    protected:
        GLuint      gl_id_ = 0;
        std::size_t vertex_count_;
    };
    
    class index_buffer
    {
        // template< typename... Attributes > friend class shader_program;
        
    public:
        index_buffer( std::vector< GLuint > const& indices );
        index_buffer( index_buffer&& o );
        
        ~index_buffer();
        
        void upload_data( std::vector< GLuint > const& indices );
        
        std::size_t size() const;
        bool empty() const;
        // TODO: Determine if there's a more opaque way to access this
        GLuint gl_buffer_id() const;
        
    protected:
        GLuint      gl_id_ = 0;
        std::size_t index_count_;
    };
}


// Vertex buffer implementation ////////////////////////////////////////////////

template< typename... Attributes >
JadeMatrix::yavsg::gl::attribute_buffer< Attributes... >::attribute_buffer(
    std::vector< tuple_type > const& vertices
)
{
    using namespace std::string_literals;
    
    glGenBuffers( 1, &gl_id_ );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't generate buffers for yavsg::gl::attribute_buffer"s
    );
    upload_data( vertices );
}

template< typename... Attributes >
JadeMatrix::yavsg::gl::attribute_buffer< Attributes... >::attribute_buffer(
    attribute_buffer&& o
)
{
    std::swap( gl_id_, o.gl_id_ );
    std::swap( vertex_count_, o.vertex_count_ );
}

template< typename... Attributes >
JadeMatrix::yavsg::gl::attribute_buffer< Attributes... >::~attribute_buffer()
{
    if( gl_id_ != 0 )
    {
        glDeleteBuffers( 1, &gl_id_ );
    }
}

template< typename... Attributes >
void JadeMatrix::yavsg::gl::attribute_buffer< Attributes... >::upload_data(
    std::vector< tuple_type > const& vertices
)
{
    using namespace std::string_literals;
    
    glBindBuffer( GL_ARRAY_BUFFER, gl_id_ );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't bind buffer "s
        + std::to_string( gl_id_ )
        + " for yavsg::gl::attribute_buffer::upload_data()"s
    );
    
    auto const vertices_len = vertices.size() * sizeof( tuple_type );
    REQUIRE( vertices_len <= std::numeric_limits< GLsizeiptr >::max() );
    auto const vertices_len_glsip = static_cast< GLsizeiptr >( vertices_len );
    
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices_len_glsip,
        vertices.data(),
        GL_STATIC_DRAW // TODO: GL_DYNAMIC_DRAW, GL_STREAM_DRAW
    );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't upload "s
        + std::to_string( vertices.size() )
        + " buffer elements at "s
        + std::to_string(
            reinterpret_cast< std::uintptr_t >( vertices.data() )
        )
        + " for yavsg::gl::attribute_buffer::upload_data()"s
    );
    
    vertex_count_ = vertices.size();
}

template< typename... Attributes >
std::size_t JadeMatrix::yavsg::gl::attribute_buffer<
    Attributes...
>::size() const
{
    return vertex_count_;
}

template< typename... Attributes >
bool JadeMatrix::yavsg::gl::attribute_buffer< Attributes... >::empty() const
{
    return vertex_count_ == 0ul;
}

template< typename... Attributes >
GLuint JadeMatrix::yavsg::gl::attribute_buffer<
    Attributes...
>::gl_buffer_id() const
{
    return gl_id_;
}


// Index buffer implementation /////////////////////////////////////////////////
// TODO: Move this to a .cpp file

inline JadeMatrix::yavsg::gl::index_buffer::index_buffer(
    std::vector< GLuint > const& indices
)
{
    using namespace std::string_literals;
    
    glGenBuffers( 1, &gl_id_ );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't generate buffers for yavsg::gl::index_buffer"s
    );
    upload_data( indices );
}

inline JadeMatrix::yavsg::gl::index_buffer::index_buffer( index_buffer&& o )
{
    std::swap( gl_id_, o.gl_id_ );
    std::swap( index_count_, o.index_count_ );
}

inline JadeMatrix::yavsg::gl::index_buffer::~index_buffer()
{
    if( gl_id_ != 0 )
    {
        glDeleteBuffers( 1, &gl_id_ );
    }
}

inline void JadeMatrix::yavsg::gl::index_buffer::upload_data(
    const std::vector< GLuint >& indices
)
{
    using namespace std::string_literals;
    
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gl_id_ );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't bind buffer "s
        + std::to_string( gl_id_ )
        + " for yavsg::gl::index_buffer::upload_data()"s
    );
    
    auto const buffer_len = indices.size() * sizeof( GLuint );
    REQUIRE( buffer_len <= std::numeric_limits< GLsizeiptr >::max() );
    auto const buffer_len_glsip = static_cast< GLsizeiptr >( buffer_len );
    
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        buffer_len_glsip,
        indices.data(),
        GL_STATIC_DRAW // TODO: GL_DYNAMIC_DRAW, GL_STREAM_DRAW
    );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't upload "s
        + std::to_string( indices.size() )
        + " buffer elements at "s
        + std::to_string(
            reinterpret_cast< std::uintptr_t >( indices.data() )
        )
        + " for yavsg::gl::index_buffer::upload_data()"s
    );
    
    index_count_ = indices.size();
}

inline std::size_t JadeMatrix::yavsg::gl::index_buffer::size() const
{
    return index_count_;
}

inline bool JadeMatrix::yavsg::gl::index_buffer::empty() const
{
    return index_count_ == 0ul;
}

inline GLuint JadeMatrix::yavsg::gl::index_buffer::gl_buffer_id() const
{
    return gl_id_;
}
