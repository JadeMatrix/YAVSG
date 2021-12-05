#pragma once


#include <yavsg/gl_wrap.hpp>
#include <yavsg/sdl/sdl.hpp>

#include <cstddef>  // size_t
#include <string>
#include <memory>   // unique_ptr


namespace JadeMatrix::yavsg // Class prototypes ////////////////////////////////
{
    namespace gl
    {
        template<
            typename    DataType,
            std::size_t Channels
        > class texture_reference;
    }
    
    template<
        typename    DataType,
        std::size_t Channels
    >class load_texture_file_task;
    
    template<
        typename    DataType,
        std::size_t Channels
    >class upload_texture_data_task;
    
    template<
        typename    DataType,
        std::size_t Channels
    >class destroy_texture_data_task;
}


namespace JadeMatrix::yavsg::gl // Texture format traits ///////////////////////
{
    template<
        typename    DataType,
        std::size_t Channels
    > struct texture_format_traits;
    
    #define DEFINE_TEXTURE_FORMAT_TRAITS_SINGLE( \
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
    
    #define DEFINE_TEXTURE_FORMAT_TRAITS( TYPE, TYPESEQ, TYPENAME ) \
    DEFINE_TEXTURE_FORMAT_TRAITS_SINGLE( TYPE, 1, GL_R   , TYPESEQ, TYPENAME, GL_RED  ) \
    DEFINE_TEXTURE_FORMAT_TRAITS_SINGLE( TYPE, 2, GL_RG  , TYPESEQ, TYPENAME, GL_RG   ) \
    DEFINE_TEXTURE_FORMAT_TRAITS_SINGLE( TYPE, 3, GL_RGB , TYPESEQ, TYPENAME, GL_RGB  ) \
    DEFINE_TEXTURE_FORMAT_TRAITS_SINGLE( TYPE, 4, GL_RGBA, TYPESEQ, TYPENAME, GL_RGBA )

    DEFINE_TEXTURE_FORMAT_TRAITS( GLbyte  ,  8I , GL_BYTE           )
    DEFINE_TEXTURE_FORMAT_TRAITS( GLubyte ,  8UI, GL_UNSIGNED_BYTE  )
    DEFINE_TEXTURE_FORMAT_TRAITS( GLshort , 16I , GL_SHORT          )
    DEFINE_TEXTURE_FORMAT_TRAITS( GLushort, 16UI, GL_UNSIGNED_SHORT )
    DEFINE_TEXTURE_FORMAT_TRAITS( GLint   , 32I , GL_INT            )
    DEFINE_TEXTURE_FORMAT_TRAITS( GLuint  , 32UI, GL_UNSIGNED_INT   )
    // DEFINE_TEXTURE_FORMAT_TRAITS( GLhalf  , 16F , GL_HALF_FLOAT     )
    DEFINE_TEXTURE_FORMAT_TRAITS( GLfloat , 32F , GL_FLOAT          )
    // DEFINE_TEXTURE_FORMAT_TRAITS( GLdouble, 64F , GL_DOUBLE         )
    
    #undef DEFINE_TEXTURE_FORMAT_TRAITS
    #undef DEFINE_TEXTURE_FORMAT_TRAITS_SINGLE
}


namespace JadeMatrix::yavsg::gl // Texture configuration types /////////////////
{
    constexpr GLuint default_texture_gl_id = 0x00;
    
    struct texture_filter_settings
    {
        enum class magnify_mode
        {
            nearest, linear
        } magnify;
        enum class minify_mode
        {
            nearest, linear
        } minify;
        enum class mipmap_type
        {
            nearest, linear, none
        } mipmaps;
    };
    
    using texture_flags_type = std::size_t;
    namespace texture_flag
    {
        static const texture_flags_type                        none = 0x00;
        
        // If alpha channel exists, don't premultiply alpha
        static const texture_flags_type disable_premultiplied_alpha = 0x01 << 0;
        
        // Ignore any input data and just tell OpenGL to allocate texture space
        static const texture_flags_type               allocate_only = 0x01 << 1;
        
        // Skip conversion to sRGB for gamma-correctness
        static const texture_flags_type                linear_input = 0x01 << 2;
        
        // Don't drop channels that have only default values (only affects alpha
        // for now)
        static const texture_flags_type        keep_unused_channels = 0x01 << 3;
    }
}


namespace JadeMatrix::yavsg::gl // Upload utilities ////////////////////////////
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
        SDL_Surface      * sdl_surface,
        texture_flags_type flags,
        GLint              gl_internal_format
    );
    
    void upload_texture_data(
        GLuint                         gl_id,
        texture_upload_data            upload_data,
        texture_filter_settings const& settings
    );
    
    void set_bound_texture_filtering( texture_filter_settings const& settings );
}
