#pragma once
#ifndef GL_GL_UTILS_HPP
#define GL_GL_UTILS_HPP


// See https://gist.github.com/cbmeeks/5587a11e7856baf819b7
#ifdef __APPLE__
    #include <OpenGL/gl3.h>
    #include <OpenGL/gl3ext.h>
#else
    #include <GL/glew.h>
#endif

#include "../../include/math/matrix.hpp"
#include "../../include/math/vector.hpp"

#include <exception>
#include <map>
#include <string>
#include <vector>


namespace gl // Shaders ////////////////////////////////////////////////////////
{
    class shader
    {
    public:
        GLuint id;
        
        shader( GLenum shader_type, const std::string& source );
        ~shader();
        
        static shader from_file(
            GLenum shader_type,
            const std::string& filename
        );
    };
}


namespace gl // Shader programs ////////////////////////////////////////////////
{
    class shader_program
    {
    public:
        class no_such_variable : public std::runtime_error
        {
            using runtime_error::runtime_error;
        };
        class wrong_variable_type : public std::runtime_error
        {
            using runtime_error::runtime_error;
        };
        
        GLuint id;
        GLuint vao_id;
        
        shader_program( const std::vector< GLuint >& shaders );
        ~shader_program();
        
        void use();
        
        // TODO: has_attribute() and has_uniform()
        
        GLint attribute( const std::string& attribute_name );
        GLint   uniform( const std::string&   uniform_name );
        
        template< typename T > void set_uniform(
            const std::string& uniform_name,
            const T& value
        );
        template< typename T > bool try_set_uniform(
            const std::string& uniform_name,
            const T& value
        )
        {
            // TODO: Make this not rely on exceptions?
            try
            {
                set_uniform( uniform_name, value );
                return true;
            }
            catch( const no_such_variable& e )
            {
                return false;
            }
        }
    };
    
    class framebuffer
    {
    public:
        GLuint id;
        GLuint color_buffer;
        GLuint depth_stencil_buffer;
        
        framebuffer(
            GLsizei width,
            GLsizei height
        );
        ~framebuffer();
    };
    
    class render_step
    {
    public:
        virtual ~render_step() {};
        virtual void run( framebuffer& ) = 0;
    };
    
    void load_bound_texture( const std::string& filename );
}


namespace gl // shader_program::set_uniform<>() specializations ////////////////
{
    #define SHADER_PROGRAM_DEFINE_UNIFORM_SPECIALIZATION( TYPE, OPERATION ) \
    template<> inline void shader_program::set_uniform< TYPE >( \
        const std::string& uniform_name, \
        const TYPE& value \
    ) \
    { \
        auto uniform_id = uniform( uniform_name ); \
        OPERATION; \
        if( glGetError() != GL_NO_ERROR ) \
            throw wrong_variable_type( \
                "unable to set " #TYPE " uniform \"" \
                + uniform_name \
                + "\" of program " \
                + std::to_string( id ) \
            ); \
    }
    
    SHADER_PROGRAM_DEFINE_UNIFORM_SPECIALIZATION(
        float,
        glUniform1f( uniform_id, value )
    )
    
    SHADER_PROGRAM_DEFINE_UNIFORM_SPECIALIZATION(
        int,
        glUniform1i( uniform_id, value )
    )
    
    #undef SHADER_PROGRAM_DEFINE_UNIFORM_SPECIALIZATION
    
    #define SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_VECTOR_SPECIALIZATION( \
        DIMENSIONS, \
        TYPE, \
        TYPECHAR \
    ) \
    template<> inline \
    void shader_program::set_uniform< yavsg::vector< TYPE, DIMENSIONS > >( \
        const std::string& uniform_name, \
        const yavsg::vector< TYPE, DIMENSIONS >& value \
    ) \
    { \
        auto uniform_id = uniform( uniform_name ); \
        glUniform##DIMENSIONS##TYPECHAR##v( \
            uniform_id, \
            1, \
            value.data() \
        ); \
        if( glGetError() != GL_NO_ERROR ) \
            throw wrong_variable_type( \
                "unable to set yavsg::vector<" \
                #TYPE "," #DIMENSIONS "> uniform \"" \
                + uniform_name \
                + "\" of program " \
                + std::to_string( id ) \
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
    template<> inline \
    void shader_program::set_uniform< yavsg::square_matrix< TYPE, DIMENSIONS > >( \
        const std::string& uniform_name, \
        const yavsg::square_matrix< TYPE, DIMENSIONS >& value \
    ) \
    { \
        auto uniform_id = uniform( uniform_name ); \
        glUniformMatrix##DIMENSIONS##TYPECHAR##v( \
            uniform_id, \
            1, \
            false, \
            value.data() \
        ); \
        if( glGetError() != GL_NO_ERROR ) \
            throw wrong_variable_type( \
                "unable to set yavsg::matrix<" \
                #TYPE "," #DIMENSIONS "," #DIMENSIONS "> uniform \"" \
                + uniform_name \
                + "\" of program " \
                + std::to_string( id ) \
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
    template<> inline \
    void shader_program::set_uniform< yavsg::matrix< TYPE, ROWS, COLS > >( \
        const std::string& uniform_name, \
        const yavsg::matrix< TYPE, ROWS, COLS >& value \
    ) \
    { \
        auto uniform_id = uniform( uniform_name ); \
        glUniformMatrix##ROWS##x##COLS##TYPECHAR##v( \
            uniform_id, \
            1, \
            false, \
            value.data() \
        ); \
        if( glGetError() != GL_NO_ERROR ) \
            throw wrong_variable_type( \
                "unable to set yavsg::matrix<" \
                #TYPE "," #ROWS "," #COLS "> uniform \"" \
                + uniform_name \
                + "\" of program " \
                + std::to_string( id ) \
            ); \
    }
    
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( 2, 3, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( 3, 2, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( 2, 4, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( 4, 2, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( 3, 4, GLfloat, f )
    SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION( 4, 3, GLfloat, f )
    
    #undef SHADER_PROGRAM_DEFINE_UNIFORM_YAVSG_NONSQUARE_MATRIX_SPECIALIZATION
}


#endif

