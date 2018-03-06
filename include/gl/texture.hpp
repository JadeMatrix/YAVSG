#pragma once
#ifndef YAVSG_GL_TEXTURE_HPP
#define YAVSG_GL_TEXTURE_HPP


#include "_gl_base.hpp"
#include "error.hpp"
#include "../sdl/_sdl_base.hpp"

#include <array>
#include <string>
#include <type_traits>  // std::enable_if
#include <utility>      // std::size_t


namespace yavsg { namespace gl
{
    template< typename DataType, std::size_t Channels >
    struct texture_format_traits {};
    
    #define DECLARE_TEXTURE_FORMAT_TRAITS_SINGLE( \
        TYPE, \
        CHANNELS, \
        PREFIX, \
        TYPESEQ \
    ) \
    template<> struct texture_format_traits< TYPE, CHANNELS > \
    { \
        static const GLint gl_format = PREFIX##TYPESEQ; \
    };
    
    #define DECLARE_TEXTURE_FORMAT_TRAITS( TYPE, TYPESEQ ) \
    DECLARE_TEXTURE_FORMAT_TRAITS_SINGLE( TYPE, 1, GL_R   , TYPESEQ ) \
    DECLARE_TEXTURE_FORMAT_TRAITS_SINGLE( TYPE, 2, GL_RG  , TYPESEQ ) \
    DECLARE_TEXTURE_FORMAT_TRAITS_SINGLE( TYPE, 3, GL_RGB , TYPESEQ ) \
    DECLARE_TEXTURE_FORMAT_TRAITS_SINGLE( TYPE, 4, GL_RGBA, TYPESEQ )

    DECLARE_TEXTURE_FORMAT_TRAITS( GLbyte  ,  8I  )
    DECLARE_TEXTURE_FORMAT_TRAITS( GLubyte ,  8UI )
    DECLARE_TEXTURE_FORMAT_TRAITS( GLshort , 16I  )
    DECLARE_TEXTURE_FORMAT_TRAITS( GLushort, 16UI )
    DECLARE_TEXTURE_FORMAT_TRAITS( GLint   , 32I  )
    DECLARE_TEXTURE_FORMAT_TRAITS( GLuint  , 32UI )
    // DECLARE_TEXTURE_FORMAT_TRAITS( GLhalf  , 16F  )
    DECLARE_TEXTURE_FORMAT_TRAITS( GLfloat , 32F  )
    // DECLARE_TEXTURE_FORMAT_TRAITS( GLdouble, 64F  )
    
    #undef DECLARE_TEXTURE_FORMAT_TRAITS
    #undef DECLARE_TEXTURE_FORMAT_TRAITS_SINGLE
} }


namespace yavsg { namespace gl
{
    static const GLuint default_texture_gl_id = 0x00;
    
    // A base class to hold all the stuff that doesn't need to be templated
    class _texture_general
    {
    protected:
        GLuint gl_id = default_texture_gl_id;
        
        struct filter_settings
        {
            enum class magnify_mode
            {
                NEAREST, LINEAR
            } magnify;
            enum class minify_mode
            {
                NEAREST, LINEAR
            } minify;
            enum class mipmap_type
            {
                NEAREST, LINEAR, NONE
            } mipmaps;
        };
        
        _texture_general();
        _texture_general(
            SDL_Surface*, // Not `const` due to the SDL API
            const filter_settings&,
            GLint         // Target OpenGL format after upload
        );
        
    public:
        // TODO: wrap settings
        void filtering( const filter_settings& );
        
        // TODO: Determine if there's a more opaque way to access this
        GLuint gl_texture_id();
    };
    
    /*template<
        typename    DataType = GLfloat,
        std::size_t Channels = 4
    >*/ class texture : public _texture_general
    {
    protected:
        texture();
        
    public:
        using _texture_general::filter_settings;
        
        texture(
            SDL_Surface*, // Not `const` due to the SDL API
            const filter_settings&
        );
        texture( texture&& );
        
        // Disable copy & assignment
        texture( const texture& ) = delete;
        texture& operator=( const texture& ) = delete;
        
        static texture from_file(
            const std::string& filename,
            const filter_settings& settings
        );
        
        // static texture rgba8_from_bytes(
        //     unsigned int width,
        //     unsigned int height,
        //     const std::array< DataType, Channels >* data,
        //     const filter_settings& settings
        // );
        
        // For use by stuff like framebuffers that need to control texture
        // allocation
        static texture make_empty();
        
        ~texture();
        
        template<
            std::size_t ActiveTexture,
            typename std::enable_if<
                ( ActiveTexture < GL_MAX_TEXTURE_UNITS ),
                int
            >::type = 0
        > void bind_as() const
        {
            glActiveTexture( GL_TEXTURE0 + ActiveTexture );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't activate GL_TEXTURE"
                + std::to_string( ActiveTexture )
                + " to bind texture "
                + std::to_string( gl_id )
                + " for yavsg::gl::texture::bind_as<>()"
            );
            
            glBindTexture( GL_TEXTURE_2D, gl_id );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't bind texture "
                + std::to_string( gl_id )
                + " as GL_TEXTURE"
                + std::to_string( ActiveTexture )
                + " for yavsg::gl::texture::bind_as<>()"
            );
        }
    };
    
    template<
        std::size_t ActiveTexture,
        typename std::enable_if<
            ( ActiveTexture < GL_MAX_TEXTURE_UNITS ),
            int
        >::type = 0
    > void unbind_texture()
    {
        glActiveTexture( GL_TEXTURE0 + ActiveTexture );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't activate GL_TEXTURE"
            + std::to_string( ActiveTexture )
            + " to bind default texture for yavsg::gl::unbind_texture<>()"
        );
        
        glBindTexture( GL_TEXTURE_2D, 0 );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind default texture as GL_TEXTURE"
            + std::to_string( ActiveTexture )
            + " for yavsg::gl::unbind_texture<>()"
        );
    }
} }


#endif
