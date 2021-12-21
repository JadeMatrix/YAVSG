#pragma once


#include <yavsg/gl_wrap.hpp>

#include <yavsg/gl/error.hpp>
#include "attribute_buffer.hpp"
// #include "../rendering/shader_variable_names.hpp"

#include <yavsg/math/matrix.hpp>
#include <yavsg/math/vector.hpp>

#include <doctest/doctest.h>    // REQUIRE

#include <exception>
#include <limits>
#include <string>
#include <tuple>
#include <type_traits>  // enable_if_t


namespace JadeMatrix::yavsg::gl // Shader program //////////////////////////////
{
    // Helper function to allow partial specialization of
    // shader_program<>::set_uniform<>()
    template< typename T > void set_current_program_uniform(
        GLint              uniform_location,
        T           const& uniform_value,
        // For error handling:
        std::string const& uniform_name,
        GLuint           & gl_program_id
    );
    
    template< class AttributeBuffer, class Framebuffer > class shader_program
    {
    public:
        using attribute_buffer_type = AttributeBuffer;
        using            tuple_type = typename attribute_buffer_type::tuple_type;
        using      framebuffer_type = Framebuffer;
        
        shader_program( std::vector< GLuint > const& shaders );
        ~shader_program();
        
        void run( attribute_buffer_type const& buffer );
        void run(
            attribute_buffer_type const& buffer,
            std::size_t start,
            std::size_t count
        );
        void run(
            attribute_buffer_type const& buffer,
            index_buffer          const& indices
        );
        void run(
            attribute_buffer_type const& buffer,
            index_buffer          const& indices,
            std::size_t                  start,
            std::size_t                  count
        );
        
        bool has_attribute( std::string const& name                   );
        bool has_attribute( std::string const& name, GLuint& location );
        bool has_uniform  ( std::string const& name                   );
        bool has_uniform  ( std::string const& name, GLint & location );
        
        // Return true if the variable exists & could be linked, false if it
        // does not exist, and throws `yavsg::gl::summary_error` if the variable
        // exists but could not be linked.
        template< std::size_t Nth > bool link_attribute(
            std::string           const& attribute_name,
            attribute_buffer_type const& dummy_attributes
        );
        template< std::size_t Nth, typename StrID > bool link_attribute(
            StrID                 const& attribute_name_id,
            attribute_buffer_type const& dummy_attributes
        );
        template< typename T > bool set_uniform(
            std::string const& uniform_name,
            T           const& uniform_value
        );
        template< typename T, typename StrID > bool set_uniform(
            StrID const& uniform_name_id,
            T     const& uniform_value
        );
        
        // Note: use glDrawBuffers when rendering to multiple buffers, because
        // only the first output will be enabled by default.
        template<
            std::size_t Nth,
            typename    = std::enable_if_t<
                ( Nth < framebuffer_type::num_color_targets )
            >
        > void bind_target(
            std::string const& target_name
        );
        template<
            std::size_t Nth,
            typename    StrID,
            typename    = std::enable_if_t<
                ( Nth < framebuffer_type::num_color_targets )
            >
        > void bind_target(
            StrID const& target_name_id
        );
        
    protected:
        GLuint gl_program_id_;
        GLuint gl_vao_id_;
        
        // OpenGL commands wrapped with error-checking
        void use_program();
        void bind_vao();
    };
}


namespace JadeMatrix::yavsg::gl // Attribute traits ////////////////////////////
{
    template< typename T > struct attribute_traits {};
    
    template<> struct attribute_traits< GLfloat >
    {
        static constexpr GLint  components_per_element = 1;
        static constexpr GLenum component_type         = GL_FLOAT;
    };
    
    template<> struct attribute_traits< GLint >
    {
        static constexpr GLint  components_per_element = 1;
        static constexpr GLenum component_type         = GL_INT;
    };
    
    // TODO: more basic types, see
    //      https://www.khronos.org/opengl/wiki/OpenGL_Type
    //      http://docs.gl/gl3/glVertexAttribPointer
    
    template< typename T, unsigned int D >
    struct attribute_traits< vector< T, D > >
    {
        static constexpr GLint components_per_element
            = attribute_traits< T >::components_per_element * D;
        static constexpr GLenum component_type
            = attribute_traits< T >::component_type;
    };
}


// Shader program implementations //////////////////////////////////////////////

template< class AttributeBuffer, class Framebuffer >
void JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::use_program()
{
    gl::UseProgram( gl_program_id_ );
}

template< class AttributeBuffer, class Framebuffer >
void JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::bind_vao()
{
    gl::BindVertexArray( gl_vao_id_ );
}

template< class AttributeBuffer, class Framebuffer >
JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::shader_program(
    std::vector< GLuint > const& shaders
)
{
    gl::GenVertexArrays( 1, &gl_vao_id_ );
    gl::BindVertexArray( gl_vao_id_ );
    
    gl_program_id_ = gl::CreateProgram();
    REQUIRE( gl_program_id_ );
    
    try
    {
        for( auto& shader_id : shaders )
        {
            gl::AttachShader( gl_program_id_, shader_id );
        }
        
        gl::LinkProgram( gl_program_id_ );
        
        GLint link_status;
        gl::GetProgramiv( gl_program_id_, GL_LINK_STATUS, &link_status );
        if( link_status != GL_TRUE )
        {
            constexpr GLsizei log_buffer_length = 1024;
            char log_buffer[ log_buffer_length ];
            GLsizei got_buffer_length;
            std::string log;
            
            while( true )
            {
                gl::GetProgramInfoLog(
                    gl_program_id_,
                    log_buffer_length,
                    &got_buffer_length,
                    log_buffer
                );
                if( got_buffer_length <= 0 )
                {
                    break;
                }
                log += std::string(
                    log_buffer,
                    static_cast< std::size_t >( got_buffer_length )
                );
            }
            
            using namespace std::string_literals;
            throw std::runtime_error{
                "failed to link shader program:\n"s + log
            };
        }
    }
    catch( ... )
    {
        gl::DeleteProgram( gl_program_id_ );
        throw;
    }
}

template< class AttributeBuffer, class Framebuffer >
JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::~shader_program()
{
    gl::DeleteProgram( gl_program_id_ );
    gl::DeleteVertexArrays( 1, &gl_vao_id_ );
}

template< class AttributeBuffer, class Framebuffer >
void JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::run(
    attribute_buffer_type const& buffer
)
{
    run( buffer, 0, buffer.size() );
}

template< class AttributeBuffer, class Framebuffer >
void JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::run(
    attribute_buffer_type const& buffer,
    std::size_t                  start,
    std::size_t                  count
)
{
    use_program();
    bind_vao();
    
    // Const cast as the OpenGL object ID from this is only used in a const-
    // like way
    attribute_buffer_type const& buffer_ref =
        const_cast< attribute_buffer_type& >(  buffer );
    
    gl::BindBuffer( GL_ARRAY_BUFFER, buffer_ref.gl_buffer_id() );
    
    if( count + start > buffer.size() )
    {
        count = buffer.size() - start;
    }
    
    // TODO: other primitive types
    gl::DrawArrays( GL_TRIANGLES, start, count );
}

template< class AttributeBuffer, class Framebuffer >
void JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::run(
    attribute_buffer_type const& buffer,
    index_buffer          const& indices
)
{
    run( buffer, indices, 0, indices.size() );
}

template< class AttributeBuffer, class Framebuffer >
void JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::run(
    attribute_buffer_type const& buffer,
    index_buffer          const& indices,
    std::size_t                  start,
    std::size_t                  count
)
{
    use_program();
    bind_vao();
    
    // Const cast as the OpenGL object IDs from these are only used in a
    // const-like way
    attribute_buffer_type&  buffer_ref = const_cast< attribute_buffer_type& >(  buffer );
    index_buffer         & indices_ref = const_cast< index_buffer         & >( indices );
    
    gl::BindBuffer( GL_ARRAY_BUFFER, buffer_ref.gl_buffer_id() );
    
    gl::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, indices_ref.gl_buffer_id() );
    
    if( count + start > indices.size() )
    {
        count = indices.size() - start;
    }
    REQUIRE( count <= std::numeric_limits< GLsizei >::max() );
    
    gl::DrawElements(
        GL_TRIANGLES, // TODO: other primitive types
        static_cast< GLsizei >( count ),
        GL_UNSIGNED_INT,
        reinterpret_cast< void* >( start * sizeof( GLuint ) )
    );
}

template< class AttributeBuffer, class Framebuffer >
bool JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::has_attribute(
    std::string const& name
)
{
    GLuint location;
    return has_attribute( name, location );
}

template< class AttributeBuffer, class Framebuffer >
bool JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::has_attribute(
    std::string const& name,
    GLuint           & location
)
{
    using namespace std::string_literals;
    
    auto const result = gl::GetAttribLocation( gl_program_id_, name.c_str() );
    if( result == -1 )
    {
        return false;
    }
    else
    {
        location = static_cast< GLuint >( result );
        return true;
    }
}

template< class AttributeBuffer, class Framebuffer >
bool JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::has_uniform(
    std::string const& name
)
{
    GLint location;
    return has_uniform( name, location );
}

template< class AttributeBuffer, class Framebuffer >
bool JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::has_uniform(
    std::string const& name,
    GLint            & location
)
{
    auto const result = gl::GetUniformLocation( gl_program_id_, name.c_str() );
    if( result == -1 )
    {
        return false;
    }
    else
    {
        location = result;
        return true;
    }
}

template< class AttributeBuffer, class Framebuffer >
template< std::size_t Nth >
bool JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::link_attribute(
    std::string           const& attribute_name,
    attribute_buffer_type const& dummy_attributes
)
{
    GLuint attribute_location;
    if( !has_attribute( attribute_name, attribute_location ) )
    {
        return false;
    }
    
    bind_vao();
    
    attribute_buffer_type& buffer_ref = const_cast<
        attribute_buffer_type&
    >( dummy_attributes );
    gl::BindBuffer( GL_ARRAY_BUFFER, buffer_ref.gl_buffer_id() );
    
    gl::EnableVertexAttribArray( attribute_location );
    
    using attribute_type = typename std::tuple_element< Nth, tuple_type >::type;
    
    // I've tried everything I can think of, this seems to be the only way
    // to """safely""" get the byte offset of a tuple member.
    // FIXME: I've learned a lot more, it's not.
    const tuple_type& fake_tuple = *reinterpret_cast< tuple_type* >( 0x01 );
    const auto offset_of_attribute = (
          reinterpret_cast< const char* >( &std::get< Nth >( fake_tuple ) )
        - reinterpret_cast< const char* >( &fake_tuple                    )
    );
    
    gl::VertexAttribPointer(
        attribute_location,
        attribute_traits< attribute_type >::components_per_element,
        attribute_traits< attribute_type >::component_type,
        GL_FALSE,
        sizeof( tuple_type ),
        reinterpret_cast< const void* >( offset_of_attribute )
    );
    
    return true;
}

template< class AttributeBuffer, class Framebuffer >
template< std::size_t Nth, typename StrID >
bool JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::link_attribute(
    StrID                 const& attribute_name_id,
    attribute_buffer_type const& dummy_attributes
)
{
    return link_attribute< Nth >(
        shader_string( attribute_name_id ),
        dummy_attributes
    );
}

template< class AttributeBuffer, class Framebuffer >
template< typename T >
bool JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::set_uniform(
    std::string const& uniform_name,
    T           const& uniform_value
)
{
    GLint uniform_location;
    if( !has_uniform( uniform_name, uniform_location ) )
    {
        return false;
    }
    use_program();
    set_current_program_uniform< T >(
        uniform_location,
        uniform_value,
        uniform_name,
        gl_program_id_
    );
    return true;
}

template< class AttributeBuffer, class Framebuffer >
template< typename T, typename StrID >
bool JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::set_uniform(
    StrID const& uniform_name_id,
    T     const& uniform_value
)
{
    return set_uniform( shader_string( uniform_name_id ), uniform_value );
}

template< class AttributeBuffer, class Framebuffer >
template<
    std::size_t Nth,
    typename    /* enable */
>
void JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::bind_target( const std::string& target_name )
{
    gl::BindFragDataLocation( gl_program_id_, Nth, target_name.c_str() );
}

template< class AttributeBuffer, class Framebuffer >
template<
    std::size_t Nth,
    typename    StrID,
    typename    /* enable */
>
void JadeMatrix::yavsg::gl::shader_program<
    AttributeBuffer,
    Framebuffer
>::bind_target( StrID const& target_name_id )
{
    return bind_target< Nth >( shader_string( target_name_id ) );
}


// Uniform-set specializations /////////////////////////////////////////////////

#define DEFINE_BASIC_SHADER_PROGRAM_UNIFORM_SPECIALIZATION( \
    TYPE, \
    OPERATION \
) \
    template<> inline void \
    JadeMatrix::yavsg::gl::set_current_program_uniform< TYPE >( \
        GLint              uniform_location, \
        TYPE        const& uniform_value, \
        std::string const& /* uniform_name */, \
        GLuint           & /* gl_program_id */ \
    ) \
    { \
        OPERATION; \
    }

DEFINE_BASIC_SHADER_PROGRAM_UNIFORM_SPECIALIZATION(
    GLfloat,
    gl::Uniform1f( uniform_location, uniform_value )
)
DEFINE_BASIC_SHADER_PROGRAM_UNIFORM_SPECIALIZATION(
    GLint,
    gl::Uniform1i( uniform_location, uniform_value )
)
DEFINE_BASIC_SHADER_PROGRAM_UNIFORM_SPECIALIZATION(
    GLuint,
    gl::Uniform1ui( uniform_location, uniform_value )
)
// There is no glUniform1b, so just use glUniform1ui()
DEFINE_BASIC_SHADER_PROGRAM_UNIFORM_SPECIALIZATION(
    GLboolean,
    gl::Uniform1ui( uniform_location, uniform_value )
)

#undef DEFINE_BASIC_SHADER_PROGRAM_UNIFORM_SPECIALIZATION


#define DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( \
    DIMENSIONS, \
    TYPE, \
    TYPECHAR \
) \
    template<> inline void JadeMatrix::yavsg::gl::set_current_program_uniform< \
        JadeMatrix::yavsg::vector< TYPE, DIMENSIONS > \
    >( \
        GLint                                    uniform_location, \
        yavsg::vector< TYPE, DIMENSIONS > const& uniform_value, \
        std::string                       const& /* uniform_name */, \
        GLuint                                 & /* gl_program_id */ \
    ) \
    { \
        gl::Uniform##DIMENSIONS##TYPECHAR##v( \
            uniform_location, \
            1, \
            uniform_value.data() \
        ); \
    }

DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( 1, GLfloat, f )
DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( 2, GLfloat, f )
DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( 3, GLfloat, f )
DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( 4, GLfloat, f )

DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( 1, GLint, i )
DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( 2, GLint, i )
DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( 3, GLint, i )
DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( 4, GLint, i )

DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( 1, GLuint, ui )
DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( 2, GLuint, ui )
DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( 3, GLuint, ui )
DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION( 4, GLuint, ui )

#undef DEFINE_SHADER_PROGRAM_UNIFORM_VECTOR_SPECIALIZATION


#define DEFINE_SHADER_PROGRAM_UNIFORM_SQUARE_MATRIX_SPECIALIZATION( \
    DIMENSIONS, \
    TYPE, \
    TYPECHAR \
) \
    template<> inline void JadeMatrix::yavsg::gl::set_current_program_uniform< \
        JadeMatrix::yavsg::square_matrix< TYPE, DIMENSIONS > \
    >( \
        GLint                                           uniform_location, \
        yavsg::square_matrix< TYPE, DIMENSIONS > const& uniform_value, \
        std::string                              const& /* uniform_name */, \
        GLuint                                        & /* gl_program_id */ \
    ) \
    { \
        gl::UniformMatrix##DIMENSIONS##TYPECHAR##v( \
            uniform_location, \
            1, \
            false, \
            uniform_value.data() \
        ); \
    }

DEFINE_SHADER_PROGRAM_UNIFORM_SQUARE_MATRIX_SPECIALIZATION( 2, GLfloat, f )
DEFINE_SHADER_PROGRAM_UNIFORM_SQUARE_MATRIX_SPECIALIZATION( 3, GLfloat, f )
DEFINE_SHADER_PROGRAM_UNIFORM_SQUARE_MATRIX_SPECIALIZATION( 4, GLfloat, f )

#undef DEFINE_SHADER_PROGRAM_UNIFORM_SQUARE_MATRIX_SPECIALIZATION


#define DEFINE_SHADER_PROGRAM_UNIFORM_NONSQUARE_MATRIX_SPECIALIZATION( \
    ROWS, \
    COLS, \
    TYPE, \
    TYPECHAR \
) \
    template<> inline void JadeMatrix::yavsg::gl::set_current_program_uniform< \
        JadeMatrix::yavsg::matrix< TYPE, ROWS, COLS > \
    >( \
        GLint                                    uniform_location, \
        yavsg::matrix< TYPE, ROWS, COLS > const& uniform_value, \
        std::string                       const& /* uniform_name */, \
        GLuint                                 & /* gl_program_id */ \
    ) \
    { \
        gl::UniformMatrix##ROWS##x##COLS##TYPECHAR##v( \
            uniform_location, \
            1, \
            false, \
            uniform_value.data() \
        ); \
    }

DEFINE_SHADER_PROGRAM_UNIFORM_NONSQUARE_MATRIX_SPECIALIZATION( 2, 3, GLfloat, f )
DEFINE_SHADER_PROGRAM_UNIFORM_NONSQUARE_MATRIX_SPECIALIZATION( 3, 2, GLfloat, f )
DEFINE_SHADER_PROGRAM_UNIFORM_NONSQUARE_MATRIX_SPECIALIZATION( 2, 4, GLfloat, f )
DEFINE_SHADER_PROGRAM_UNIFORM_NONSQUARE_MATRIX_SPECIALIZATION( 4, 2, GLfloat, f )
DEFINE_SHADER_PROGRAM_UNIFORM_NONSQUARE_MATRIX_SPECIALIZATION( 3, 4, GLfloat, f )
DEFINE_SHADER_PROGRAM_UNIFORM_NONSQUARE_MATRIX_SPECIALIZATION( 4, 3, GLfloat, f )

#undef DEFINE_SHADER_PROGRAM_UNIFORM_NONSQUARE_MATRIX_SPECIALIZATION
