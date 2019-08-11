#pragma once
#ifndef YAVSG_GL_TEXTURE_UTILITIES_HPP
#define YAVSG_GL_TEXTURE_UTILITIES_HPP


#include <yavsg/gl_wrap.hpp>
#include <yavsg/sdl/sdl.hpp>

#include <string>
#include <utility>  // std::size_t
#include <memory>   // std::unique_ptr


namespace yavsg // Class prototypes ////////////////////////////////////////////
{
    namespace gl
    {
        template<
            typename DataType,
            std::size_t Channels
        > class texture_reference;
    }
    
    template<
        typename DataType,
        std::size_t Channels
    >class load_texture_file_task;
    
    template<
        typename DataType,
        std::size_t Channels
    >class upload_texture_data_task;
    
    template<
        typename DataType,
        std::size_t Channels
    >class destroy_texture_data_task;
}


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
        static const GLenum gl_incoming_format = FORMATNAME; \
        static const GLenum gl_incoming_type   = TYPENAME; \
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
    namespace texture_flag
    {
        static const texture_flags_type                        NONE = 0x00;
        
        // If alpha channel exists, don't premultiply alpha
        static const texture_flags_type DISABLE_PREMULTIPLIED_ALPHA = 0x01 << 0;
        
        // Ignore any input data and just tell OpenGL to allocate texture space
        static const texture_flags_type               ALLOCATE_ONLY = 0x01 << 1;
        
        // Skip conversion to sRGB for gamma-correctness
        static const texture_flags_type                LINEAR_INPUT = 0x01 << 2;
        
        // Don't drop channels that have only default values (only affects alpha
        // for now)
        static const texture_flags_type        KEEP_UNUSED_CHANNELS = 0x01 << 3;
    }
} }


namespace yavsg { namespace gl // Upload utilities /////////////////////////////
{
    struct texture_upload_data
    {
        GLint                     gl_internal_format;
        std::size_t               width;
        std::size_t               height;
        GLenum                    gl_incoming_format;
        GLenum                    gl_incoming_type;
        std::unique_ptr< char[] > data;
    };
    
    texture_upload_data process_texture_data(
        texture_upload_data upload_data,
        texture_flags_type  flags
    );
    texture_upload_data process_texture_data(
        // Not `const` due to the SDL API
        SDL_Surface*       sdl_surface,
        texture_flags_type flags,
        GLint              gl_internal_format
    );
    
    void upload_texture_data(
        GLuint                         gl_id,
        texture_upload_data            upload_data,
        const texture_filter_settings& settings
    );
    
    void set_bound_texture_filtering(
        const texture_filter_settings& settings
    );
} }


#endif
