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

#include <SDL2/SDL_image.h>

#include <exception>


namespace yavsg { namespace gl // Texture format traits ////////////////////////
{
    template< typename DataType, std::size_t Channels >
    struct texture_format_traits {};
    
    #define DECLARE_TEXTURE_FORMAT_TRAITS_SINGLE( \
        TYPE, \
        CHANNELS, \
        PREFIX, \
        TYPESEQ, \
        TYPENAME, \
        FORMATNAME \
    ) \
    template<> struct texture_format_traits< TYPE, CHANNELS > \
    { \
        static const GLint  gl_internal_format = PREFIX##TYPESEQ; \
        static const GLenum gl_format          = FORMATNAME; \
        static const GLenum gl_type            = TYPENAME; \
    };
    
    #define DECLARE_TEXTURE_FORMAT_TRAITS( TYPE, TYPESEQ, TYPENAME ) \
    DECLARE_TEXTURE_FORMAT_TRAITS_SINGLE( TYPE, 1, GL_R   , TYPESEQ, TYPENAME, GL_RED  ) \
    DECLARE_TEXTURE_FORMAT_TRAITS_SINGLE( TYPE, 2, GL_RG  , TYPESEQ, TYPENAME, GL_RG   ) \
    DECLARE_TEXTURE_FORMAT_TRAITS_SINGLE( TYPE, 3, GL_RGB , TYPESEQ, TYPENAME, GL_RGB  ) \
    DECLARE_TEXTURE_FORMAT_TRAITS_SINGLE( TYPE, 4, GL_RGBA, TYPESEQ, TYPENAME, GL_RGBA )

    DECLARE_TEXTURE_FORMAT_TRAITS( GLbyte  ,  8I , GL_BYTE           )
    DECLARE_TEXTURE_FORMAT_TRAITS( GLubyte ,  8UI, GL_UNSIGNED_BYTE  )
    DECLARE_TEXTURE_FORMAT_TRAITS( GLshort , 16I , GL_SHORT          )
    DECLARE_TEXTURE_FORMAT_TRAITS( GLushort, 16UI, GL_UNSIGNED_SHORT )
    DECLARE_TEXTURE_FORMAT_TRAITS( GLint   , 32I , GL_INT            )
    DECLARE_TEXTURE_FORMAT_TRAITS( GLuint  , 32UI, GL_UNSIGNED_INT   )
    // DECLARE_TEXTURE_FORMAT_TRAITS( GLhalf  , 16F , GL_HALF_FLOAT     )
    DECLARE_TEXTURE_FORMAT_TRAITS( GLfloat , 32F , GL_FLOAT          )
    // DECLARE_TEXTURE_FORMAT_TRAITS( GLdouble, 64F , GL_DOUBLE         )
    
    #undef DECLARE_TEXTURE_FORMAT_TRAITS
    #undef DECLARE_TEXTURE_FORMAT_TRAITS_SINGLE
} }


namespace yavsg { namespace gl // Texture configuration types //////////////////
{
    static const GLuint default_texture_gl_id = 0x00;
    
    struct texture_filter_settings
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
    
    using texture_flags_type = std::size_t;
    enum texture_flags
    {
        NONE                        = 0x00,
        // If alpha channel exists, don't premultiply alpha
        DISABLE_PREMULTIPLIED_ALPHA = 0x01 << 0,
        // Ignore any input data and just tell OpenGL to allocate texture space
        ALLOCATE_ONLY               = 0x01 << 1,
        // Skip conversion to sRGB for gamma-correctness
        LINEAR_INPUT                = 0x01 << 2
    };
} }


namespace yavsg { namespace gl
{
    // A base class to hold all the stuff that doesn't need to be templated
    class _texture_general
    {
    protected:
        GLuint gl_id = default_texture_gl_id;
        
        _texture_general();
        ~_texture_general();
        
        void upload(
            std::size_t width,
            std::size_t height,
            const void* data,
            const texture_filter_settings& settings,
            texture_flags_type flags,
            GLint gl_internal_format, // Target OpenGL format after upload
            GLenum gl_format,         // Incoming data format
            GLenum gl_type            // Incoming data type
        );
        void upload(
            SDL_Surface* sdl_surface, // Not `const` due to the SDL API
            const texture_filter_settings& settings,
            texture_flags_type flags,
            GLint gl_internal_format  // Target OpenGL format after upload
        );
        
    public:
        // TODO: wrap settings
        void filtering( const texture_filter_settings& );
        
        // TODO: Determine if there's a more opaque way to access this
        GLuint gl_texture_id();
    };
    
    template<
        typename DataType,
        std::size_t Channels
    > class texture : public _texture_general
    {
    protected:
        texture();
        
    public:
        using format_traits = texture_format_traits< DataType, Channels >;
        
        using             sample_type = DataType;
        static const auto channels    = Channels;
        
        template<
            typename    InDataType = DataType,
            std::size_t InChannels = Channels
        > texture(
            std::size_t width,
            std::size_t height,
            const std::array< DataType, Channels >* data,
            const texture_filter_settings& settings,
            texture_flags_type flags = texture_flags::NONE
        );
        texture(
            SDL_Surface* sdl_surface, // Not `const` due to the SDL API
            const texture_filter_settings& settings,
            texture_flags_type flags = texture_flags::NONE
        );
        texture( texture&& );
        
        // Disable copy & assignment
        texture( const texture& ) = delete;
        texture& operator=( const texture& ) = delete;
        
        static texture from_file(
            const std::string& filename,
            const texture_filter_settings& settings,
            texture_flags_type flags = texture_flags::NONE
        );
        
        // For use by stuff like framebuffers that need to control texture
        // allocation
        static texture make_empty();
        
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
            + " to bind default texture for yavsg::gl::unbind_texture()"
        );
        
        glBindTexture( GL_TEXTURE_2D, 0 );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind default texture as GL_TEXTURE"
            + std::to_string( ActiveTexture )
            + " for yavsg::gl::unbind_texture()"
        );
    }
} }


namespace yavsg { namespace gl // Texture class implementation /////////////////
{
    template< typename DataType, std::size_t Channels >
    texture< DataType, Channels >::texture() : _texture_general()
    {}
    
    template< typename   DataType, std::size_t   Channels >
    template< typename InDataType, std::size_t InChannels >
    texture< DataType, Channels >::texture(
        std::size_t width,
        std::size_t height,
        const std::array< DataType, Channels >* data,
        const texture_filter_settings& settings,
        texture_flags_type flags
    ) : _texture_general()
    {
        using in_format_traits = texture_format_traits<
            InDataType,
            InChannels
        >;
        upload(
            width,
            height,
            data,
            settings,
            flags,
               format_traits::gl_internal_format,
            in_format_traits::gl_format,
            in_format_traits::gl_type
        );
    }
    
    template< typename DataType, std::size_t Channels >
    texture< DataType, Channels >::texture(
        SDL_Surface* sdl_surface,
        const texture_filter_settings& settings,
        texture_flags_type flags
    ) : _texture_general()
    {
        upload(
            sdl_surface,
            settings,
            flags,
            format_traits::gl_internal_format
        );
    }
    
    template< typename DataType, std::size_t Channels >
    texture< DataType, Channels >::texture( texture< DataType, Channels >&& o )
    {
        std::swap( gl_id, o.gl_id );
    }
} }


namespace yavsg { namespace gl // Texture static method implementations ////////
{
    template< typename DataType, std::size_t Channels >
    texture< DataType, Channels > texture< DataType, Channels >::from_file(
        const std::string& filename,
        const texture_filter_settings& settings,
        texture_flags_type flags
    )
    {
        SDL_Surface* sdl_surface = IMG_Load(
            filename.c_str()
        );
        if( !sdl_surface )
            throw std::runtime_error(
                "failed to load texture \""
                + filename
                + "\": "
                + IMG_GetError()
            );
        
        try
        {
            texture created_texture(
                sdl_surface,
                settings,
                flags
            );
            
            SDL_FreeSurface( sdl_surface );
            
            return created_texture;
        }
        catch( ... )
        {
            SDL_FreeSurface( sdl_surface );
            throw;
        }
    }
    
    template< typename DataType, std::size_t Channels >
    texture< DataType, Channels > texture< DataType, Channels >::make_empty()
    {
        return texture();
    }
} }


#endif
