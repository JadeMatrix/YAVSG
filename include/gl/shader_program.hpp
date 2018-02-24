#pragma once
#ifndef YAVSG_GL_SHADER_PROGRAM_HPP
#define YAVSG_GL_SHADER_PROGRAM_HPP


#include "_gl_base.hpp"
#include "buffer_types.hpp"
#include "error.hpp"
#include "attribute_buffer.hpp"
#include "../math/matrix.hpp"
#include "../math/vector.hpp"

#include <tuple>
#include <exception>


namespace yavsg { namespace gl // Shader program ///////////////////////////////
{
    // Helper function to allow partial specialization of
    // shader_program<>::set_uniform<>()
    template< typename T > void set_current_program_uniform(
        GLuint uniform_location,
        const T& uniform_value,
        const std::string& uniform_name,    // Passed along for error handling
        GLuint& gl_program_id               // Passed along for error handling
    );
    
    // TODO: template< typename Buffer, unsigned int Targets > class shader_program
    template< typename... Attributes > class shader_program
    {
    public:
        using attribute_buffer_type = attribute_buffer< Attributes... >;
        using            tuple_type =       std::tuple< Attributes... >;
        
    protected:
        GLuint gl_program_id;
        GLuint gl_vao_id;
        
        // OpenGL commands wrapped with error-checking
        void use_program();
        void bind_vao();
        
    public:
        shader_program( const std::vector< GLuint >& shaders );
        ~shader_program();
        
        void run( const attribute_buffer_type& buffer );
        void run(
            const attribute_buffer_type& buffer,
            std::size_t start,
            std::size_t count
        );
        void run(
            const attribute_buffer_type& buffer,
            const index_buffer& indices
        );
        void run(
            const attribute_buffer_type& buffer,
            const index_buffer& indices,
            std::size_t start,
            std::size_t count
        );
        
        bool has_attribute( const std::string& name                  );
        bool has_attribute( const std::string& name, GLint& location );
        bool has_uniform  ( const std::string& name                  );
        bool has_uniform  ( const std::string& name, GLint& location );
        
        // Return true if the variable exists & could be linked, false if it
        // does not exist, and throws `yavsg::gl::summary_error` if the variable
        // exists but could not be linked.
        template< unsigned int Nth > bool link_attribute(
            const std::string& attribute_name
        );
        template< typename T > bool set_uniform(
            const std::string& uniform_name,
            const T& uniform_value
        );
    };
} }


namespace yavsg { namespace gl // Shader program implementations ///////////////
{
    template< typename... Attributes >
    void shader_program< Attributes... >::use_program()
    {
        glUseProgram( gl_program_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't use program "
            + std::to_string( gl_program_id )
            + " for yavsg::gl::shader_program::use_program()"
        );
    }
    
    template< typename... Attributes >
    void shader_program< Attributes... >::bind_vao()
    {
        glBindVertexArray( gl_vao_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind vertex array "
            + std::to_string( gl_vao_id )
            + " for program "
            + std::to_string( gl_program_id )
            + " for yavsg::gl::shader_program::bind_vao()"
        );
    }
    
    template< typename... Attributes >
    shader_program< Attributes... >::shader_program(
        const std::vector< GLuint >& shaders
    )
    {
        glGenVertexArrays( 1, &gl_vao_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't generate vertex array for yavsg::gl::shader_program"
        );
        glBindVertexArray( gl_vao_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind vertex array for yavsg::gl::shader_program"
        );
        
        gl_program_id = glCreateProgram();
        /* if( gl_program_id == 0 ) */ YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't create program for yavsg::gl::shader_program"
        );
        for( auto& shader_id : shaders )
        {
            glAttachShader( gl_program_id, shader_id );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't attach shader "
                + std::to_string( shader_id )
                + " to program "
                + std::to_string( gl_program_id )
                + " for yavsg::gl::shader_program"
            );
        }
        
        glLinkProgram( gl_program_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't link program "
            + std::to_string( gl_program_id )
            + " for yavsg::gl::shader_program"
        );
        
        // TODO: Find a way to specify this and multi-target re:
        // Note: use glDrawBuffers when rendering to multiple buffers,
        // because only the first output will be enabled by default.
        glBindFragDataLocation( gl_program_id, 0, "color_out" );
    }
    
    template< typename... Attributes >
    shader_program< Attributes... >::~shader_program()
    {
        glDeleteProgram( gl_program_id );
        glDeleteVertexArrays( 1, &gl_vao_id );
    }
    
    template< typename... Attributes >
    void shader_program< Attributes... >::run(
        const attribute_buffer_type& buffer
    )
    {
        run( buffer, 0, buffer.size() );
    }
    
    template< typename... Attributes >
    void shader_program< Attributes... >::run(
        const attribute_buffer_type& buffer,
        std::size_t start,
        std::size_t count
    )
    {
        use_program();
        bind_vao();
        
        glBindBuffer( GL_ARRAY_BUFFER, buffer.gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind vertex array buffer "
            + std::to_string( buffer.gl_id )
            + " for program "
            + std::to_string( gl_program_id )
            + " for yavsg::gl::shader_program::run()"
        );
        
        if( count + start > buffer.size() )
            count = buffer.size() - start;
        
        glDrawArrays(
            GL_TRIANGLES, // TODO: other primitive types
            start,
            count
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't draw triangles from vertex array buffer "
            + std::to_string( buffer.gl_id )
            + " for program "
            + std::to_string( gl_program_id )
            + " for yavsg::gl::shader_program::run()"
        );
    }
    
    template< typename... Attributes >
    void shader_program< Attributes... >::run(
        const attribute_buffer_type& buffer,
        const index_buffer& indices
    )
    {
        run( buffer, indices, 0, indices.size() );
    }
    
    template< typename... Attributes >
    void shader_program< Attributes... >::run(
        const attribute_buffer_type& buffer,
        const index_buffer& indices,
        std::size_t start,
        std::size_t count
    )
    {
        use_program();
        bind_vao();
        
        glBindBuffer( GL_ARRAY_BUFFER, buffer.gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind vertex array buffer "
            + std::to_string( buffer.gl_id )
            + " for program "
            + std::to_string( gl_program_id )
            + " for yavsg::gl::shader_program::run()"
        );
        
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indices.gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind element array buffer "
            + std::to_string( indices.gl_id )
            + " for program "
            + std::to_string( gl_program_id )
            + " for yavsg::gl::shader_program::run()"
        );
        
        if( count + start > indices.size() )
            count = indices.size() - start;
        
        glDrawElements(
            GL_TRIANGLES, // TODO: other primitive types
            count,
            GL_UNSIGNED_INT,
            ( void* )( start * sizeof( GLuint ) )
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't draw triangles from vertex array buffer "
            + std::to_string( buffer.gl_id )
            + " with element array buffer "
            + std::to_string( indices.gl_id )
            + " for program "
            + std::to_string( gl_program_id )
            + " for yavsg::gl::shader_program::run()"
        );
    }
    
    template< typename... Attributes >
    bool shader_program< Attributes... >::has_attribute(
        const std::string& name
    )
    {
        GLint location;
        return has_attribute( name, location );
    }
    
    template< typename... Attributes >
    bool shader_program< Attributes... >::has_attribute(
        const std::string& name,
        GLint& location
    )
    {
        GLint result = glGetAttribLocation( gl_program_id, name.c_str() );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't get attribute \""
            + name
            + "\" of program "
            + std::to_string( gl_program_id )
            + " for yavsg::gl::shader_program::has_attribute()"
        );
        if( result == -1 )
            return false;
        else
        {
            location = result;
            return true;
        }
    }
    
    template< typename... Attributes >
    bool shader_program< Attributes... >::has_uniform(
        const std::string& name
    )
    {
        GLint location;
        return has_uniform( name, location );
    }
    
    template< typename... Attributes >
    bool shader_program< Attributes... >::has_uniform(
        const std::string& name,
        GLint& location
    )
    {
        GLint result = glGetUniformLocation( gl_program_id, name.c_str() );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't get uniform \""
            + name
            + "\" of program "
            + std::to_string( gl_program_id )
            + " for yavsg::gl::shader_program::has_uniform()"
        );
        if( result == -1 )
            return false;
        else
        {
            location = result;
            return true;
        }
    }
    
    template< typename... Attributes >
    template< unsigned int Nth >
    bool shader_program< Attributes... >::link_attribute(
        const std::string& attribute_name
    )
    {
        GLint attribute_location;
        if( !has_attribute( attribute_name, attribute_location ) )
            return false;
        
        bind_vao();
        
        glEnableVertexAttribArray( attribute_location );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't enable attribute \""
            + attribute_name
            + "\" of program "
            + std::to_string( gl_program_id )
            + " for yavsg::gl::shader_program::link_attribute()"
        );
        
        using attribute_type = typename std::tuple_element< Nth, tuple_type >::type;
        // using attribute_type = typename tuple_type::type< Nth >;
        std::size_t offset_of_attribute = offset_of<
            Nth,
            Attributes...
        >::bytes;
        
        glVertexAttribPointer(
            attribute_location,
            attribute_traits< attribute_type >::components_per_element,
            attribute_traits< attribute_type >::component_type,
            GL_FALSE,
            sizeof( tuple_type ),
            ( void* )offset_of_attribute
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't link attribute \""
            + attribute_name
            + "\" of program "
            + std::to_string( gl_program_id )
            + " for yavsg::gl::shader_program::link_attribute()"
        );
        return true;
    }
    
    template< typename... Attributes >
    template< typename T >
    bool shader_program< Attributes... >::set_uniform(
        const std::string& uniform_name,
        const T& uniform_value
    )
    {
        GLint uniform_location;
        if( !has_uniform( uniform_name, uniform_location ) )
            return false;
        use_program();
        set_current_program_uniform< T >(
            uniform_location,
            uniform_value,
            uniform_name,
            gl_program_id
        );
        return true;
    }
} }


namespace yavsg { namespace gl // Uniform-set specializations //////////////////
{
    #define SHADER_PROGRAM_DEFINE_BASIC_UNIFORM_SPECIALIZATION( \
        TYPE, \
        OPERATION \
    ) \
    template<> inline void set_current_program_uniform< TYPE >( \
        GLuint uniform_location, \
        const TYPE& uniform_value, \
        const std::string& uniform_name, \
        GLuint& gl_program_id \
    ) \
    { \
        OPERATION; \
        YAVSG_GL_THROW_FOR_ERRORS( \
            "unable to set " #TYPE " uniform \"" \
            + uniform_name \
            + "\" of program " \
            + std::to_string( gl_program_id ) \
        ); \
    }
    
    SHADER_PROGRAM_DEFINE_BASIC_UNIFORM_SPECIALIZATION(
        GLfloat,
        glUniform1f( uniform_location, uniform_value )
    )
    SHADER_PROGRAM_DEFINE_BASIC_UNIFORM_SPECIALIZATION(
        GLint,
        glUniform1i( uniform_location, uniform_value )
    )
    SHADER_PROGRAM_DEFINE_BASIC_UNIFORM_SPECIALIZATION(
        GLuint,
        glUniform1ui( uniform_location, uniform_value )
    )
    
    #undef SHADER_PROGRAM_DEFINE_BASIC_UNIFORM_SPECIALIZATION
    
    
    #define SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( \
        DIMENSIONS, \
        TYPE, \
        TYPECHAR \
    ) \
    template<> inline void set_current_program_uniform< \
        yavsg::vector< TYPE, DIMENSIONS > \
    >( \
        GLuint uniform_location, \
        const yavsg::vector< TYPE, DIMENSIONS >& uniform_value, \
        const std::string& uniform_name, \
        GLuint& gl_program_id \
    ) \
    { \
        glUniform##DIMENSIONS##TYPECHAR##v( \
            uniform_location, \
            1, \
            uniform_value.data() \
        ); \
        YAVSG_GL_THROW_FOR_ERRORS( \
            "unable to set yavsg::vector<" \
            #TYPE "," #DIMENSIONS "> uniform \"" \
            + uniform_name \
            + "\" of program " \
            + std::to_string( gl_program_id ) \
        ); \
    }
    
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( 1, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( 2, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( 3, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( 4, GLfloat, f )
    
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( 1, GLint, i )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( 2, GLint, i )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( 3, GLint, i )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( 4, GLint, i )
    
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( 1, GLuint, ui )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( 2, GLuint, ui )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( 3, GLuint, ui )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( 4, GLuint, ui )
    
    #undef SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION
    
    
    #define SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_SQUARE_MATRIX_SPECIALIZATION( \
        DIMENSIONS, \
        TYPE, \
        TYPECHAR \
    ) \
    template<> inline void set_current_program_uniform< \
        yavsg::square_matrix< TYPE, DIMENSIONS > \
    >( \
        GLuint uniform_location, \
        const yavsg::square_matrix< TYPE, DIMENSIONS >& uniform_value, \
        const std::string& uniform_name, \
        GLuint& gl_program_id \
    ) \
    { \
        glUniformMatrix##DIMENSIONS##TYPECHAR##v( \
            uniform_location, \
            1, \
            false, \
            uniform_value.data() \
        ); \
        YAVSG_GL_THROW_FOR_ERRORS( \
            "unable to set yavsg::matrix<" \
            #TYPE "," #DIMENSIONS "," #DIMENSIONS "> uniform \"" \
            + uniform_name \
            + "\" of program " \
            + std::to_string( gl_program_id ) \
        ); \
    }
    
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_SQUARE_MATRIX_SPECIALIZATION( 2, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_SQUARE_MATRIX_SPECIALIZATION( 3, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_SQUARE_MATRIX_SPECIALIZATION( 4, GLfloat, f )
    
    #undef SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_SQUARE_MATRIX_SPECIALIZATION
    
    
    #define SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( \
        ROWS, \
        COLS, \
        TYPE, \
        TYPECHAR \
    ) \
    template<> inline void set_current_program_uniform< \
        yavsg::matrix< TYPE, ROWS, COLS > \
    >( \
        GLuint uniform_location, \
        const yavsg::matrix< TYPE, ROWS, COLS >& uniform_value, \
        const std::string& uniform_name, \
        GLuint& gl_program_id \
    ) \
    { \
        glUniformMatrix##ROWS##x##COLS##TYPECHAR##v( \
            uniform_location, \
            1, \
            false, \
            uniform_value.data() \
        ); \
        YAVSG_GL_THROW_FOR_ERRORS( \
            "unable to set yavsg::matrix<" \
            #TYPE "," #ROWS "," #COLS "> uniform \"" \
            + uniform_name \
            + "\" of program " \
            + std::to_string( gl_program_id ) \
        ); \
    }
    
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( 2, 3, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( 3, 2, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( 2, 4, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( 4, 2, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( 3, 4, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( 4, 3, GLfloat, f )
    
    #undef SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION
} }


#endif
