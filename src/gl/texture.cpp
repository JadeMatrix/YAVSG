#include "../../include/gl/texture.hpp"

#include "../../include/gl/_gl_base.hpp"
#include "../../include/gl/error.hpp"

#include <exception>
#include <limits>
#include <utility>      // std::swap()

// DEBUG:
#include <iostream>


namespace // Anisotropic filtering support /////////////////////////////////////
{
    enum
    {
        NEED_CHECK,
        SUPPORTED,
        UNSUPPORTED
    } anisotropic_filtering_supported = NEED_CHECK;
}


namespace // Alpha premultiply functions ///////////////////////////////////////
{
    template< typename T >
    void premultiply(
        const  void* data,
               void* premultiplied_data,
        std::size_t  sample_count,
        
        typename std::enable_if<
            std::is_integral< T >::value,
            T
        >::type* = nullptr
    )
    {
        auto  in_data = static_cast< const T* >(               data );
        auto out_data = static_cast<       T* >( premultiplied_data );
        
        auto max = static_cast< double >( std::numeric_limits< T >::max() );
        
        for( std::size_t i = 0; i < sample_count; ++i )
        {
            std::size_t channels = 4;
            
            double alpha = static_cast< double >( in_data[ channels * i + 3 ] );
            
            std::size_t j;
            for( j = 0; j < channels - 1; ++j )
                out_data[ channels * i + j ] = static_cast< T >(
                    (
                        static_cast< double >( in_data[ channels * i + j ] )
                        * alpha
                    ) / max
                );
            out_data[ channels * i + j ] = in_data[ channels * i + j ];
        }
    }
    
    template< typename T >
    void premultiply(
        const  void* data,
               void* premultiplied_data,
        std::size_t  sample_count,
        
        typename std::enable_if<
            std::is_floating_point< T >::value,
            T
        >::type* = nullptr
    )
    {
        auto  in_data = static_cast< const T* >(               data );
        auto out_data = static_cast<       T* >( premultiplied_data );
        
        for( std::size_t i = 0; i < sample_count; ++i )
        {
            std::size_t channels = 4;
            
            auto alpha = in_data[ channels * i + 3 ];
            
            std::size_t j;
            for( j = 0; j < channels - 1; ++j )
                out_data[ channels * i + j ] = (
                    in_data[ channels * i + j ]
                    * alpha
                );
            out_data[ channels * i + j ] = in_data[ channels * i + j ];
        }
    }
}


namespace yavsg { namespace gl // Texture base class implementation ////////////
{
    _texture_general::_texture_general()
    {
        glGenTextures( 1, &gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't generate texture for yavsg::gl::texture"
        );
    }
    
    _texture_general::~_texture_general()
    {
        if( gl_id != default_texture_gl_id )
            glDeleteTextures( 1, &gl_id );
    }
    
    void _texture_general::upload(
        std::size_t                    width,
        std::size_t                    height,
        const void                   * data,
        const texture_filter_settings& settings,
        texture_flags                  flags,
        GLint                          gl_internal_format,
        GLenum                         gl_format,
        GLenum                         gl_type
    )
    {
        char* premultiplied_data = nullptr;
        
        try
        {
            if( flags & texture_flags::ALLOCATE_ONLY )
                data = nullptr;
            else if( !data )
                throw std::runtime_error(
                    "null data do _texture_general but allocate-only flag not "
                    "set"
                );
            
            if(
                data
                && gl_format == GL_RGBA
                && !( flags & texture_flags::DISABLE_PREMULTIPLIED_ALPHA )
            )
            {
                // Any format that requires multiplying will have 4 channels
                std::size_t channels = 4;
                
                switch( gl_type )
                {
                case GL_BYTE:
                    premultiplied_data = new char[
                        width * height * channels * sizeof( GLbyte )
                    ];
                    premultiply< GLbyte >(
                        data,
                        premultiplied_data,
                        width * height
                    );
                    break;
                case GL_UNSIGNED_BYTE:
                    premultiplied_data = new char[
                        width * height * channels * sizeof( GLubyte )
                    ];
                    premultiply< GLubyte >(
                        data,
                        premultiplied_data,
                        width * height
                    );
                    break;
                case GL_SHORT:
                    premultiplied_data = new char[
                        width * height * channels * sizeof( GLshort )
                    ];
                    premultiply< GLshort >(
                        data,
                        premultiplied_data,
                        width * height
                    );
                    break;
                case GL_UNSIGNED_SHORT:
                    premultiplied_data = new char[
                        width * height * channels * sizeof( GLushort )
                    ];
                    premultiply< GLushort >(
                        data,
                        premultiplied_data,
                        width * height
                    );
                    break;
                case GL_INT:
                    premultiplied_data = new char[
                        width * height * channels * sizeof( GLint )
                    ];
                    premultiply< GLint >(
                        data,
                        premultiplied_data,
                        width * height
                    );
                    break;
                case GL_UNSIGNED_INT:
                    premultiplied_data = new char[
                        width * height * channels * sizeof( GLuint )
                    ];
                    premultiply< GLuint >(
                        data,
                        premultiplied_data,
                        width * height
                    );
                    break;
                // case GL_HALF_FLOAT:
                //     premultiplied_data = new char[
                //         width * height * channels * sizeof( GLhalf )
                //     ];
                //     premultiply< GLhalf >(
                //         data,
                //         premultiplied_data,
                //         width * height
                //     );
                //     break;
                case GL_FLOAT:
                    premultiplied_data = new char[
                        width * height * channels * sizeof( GLfloat )
                    ];
                    premultiply< GLfloat >(
                        data,
                        premultiplied_data,
                        width * height
                    );
                    break;
                // case GL_DOUBLE:
                //     premultiplied_data = new char[
                //         width * height * channels * sizeof( GLdouble )
                //     ];
                //     premultiply< GLdouble >(
                //         data,
                //         premultiplied_data,
                //         width * height
                //     );
                //     break;
                default:
                    throw std::runtime_error(
                        "uknown/unsupported OpenGL type "
                        + std::to_string( gl_type )
                        + " for yavsg::gl::_texture_general"
                    );
                }
                
                data = premultiplied_data;
            }
            
            glBindTexture( GL_TEXTURE_2D, gl_id );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't bind texture "
                + std::to_string( gl_id )
                + " for yavsg::gl::_texture_general"
            );
            
            glTexImage2D(   // Loads starting at 0,0 as bottom left
                GL_TEXTURE_2D,
                0,                  // LoD, 0 = base
                gl_internal_format, // Internal format
                width,              // Width
                height,             // Height
                0,                  // Border; must be 0
                gl_format,          // Incoming format
                gl_type,            // Pixel type
                data                // Pixel data
            );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't allocate "
                + std::to_string( width )
                + "x"
                + std::to_string( height )
                + " texture "
                + std::to_string( gl_id )
                + ( data ? "" : " from null data " )
                + " for yavsg::gl::_texture_general"
            );
            
            filtering( settings );
            
            if( premultiplied_data )
                delete[] premultiplied_data;
        }
        catch( ... )
        {
            if( premultiplied_data )
                delete[] premultiplied_data;
            throw;
        }
    }
    
    void _texture_general::upload(
        SDL_Surface                  * sdl_surface,
        const texture_filter_settings& settings,
        texture_flags                  flags,
        GLint                          gl_internal_format
    )
    {
        // Surface is passed as a pointer for consistency with the SDL API, so
        // we'll want to check if it's NULL
        if( !sdl_surface )
            throw std::runtime_error(
                "null SDL_Surface* for yavsg::gl::texture"
            );
        
        GLenum incoming_format;
        GLenum incoming_type = GL_UNSIGNED_BYTE;
        SDL_Surface* converted_surface = nullptr;
        
        if( sdl_surface -> format -> format == SDL_PIXELFORMAT_RGBA8888 )
            incoming_format = GL_RGBA;
        else if( sdl_surface -> format -> format == SDL_PIXELFORMAT_RGB888 )
            incoming_format = GL_RGB;
        else // Convert pixels
        {
            SDL_PixelFormat* new_format = nullptr;
            
            bool format_has_alpha;
            switch( sdl_surface -> format -> format )
            {
            case SDL_PIXELFORMAT_ARGB4444:
            case SDL_PIXELFORMAT_RGBA4444:
            case SDL_PIXELFORMAT_ABGR4444:
            case SDL_PIXELFORMAT_BGRA4444:
            case SDL_PIXELFORMAT_ARGB1555:
            case SDL_PIXELFORMAT_RGBA5551:
            case SDL_PIXELFORMAT_ABGR1555:
            case SDL_PIXELFORMAT_BGRA5551:
            // case SDL_PIXELFORMAT_RGBX8888:
            // case SDL_PIXELFORMAT_BGRX8888:
            case SDL_PIXELFORMAT_ARGB8888:
            case SDL_PIXELFORMAT_RGBA8888:
            case SDL_PIXELFORMAT_ABGR8888:
            case SDL_PIXELFORMAT_BGRA8888:
            case SDL_PIXELFORMAT_ARGB2101010:
                format_has_alpha = true;
            default:
                format_has_alpha = false;
            }
            
            if( format_has_alpha )
            {
                new_format = SDL_AllocFormat( SDL_PIXELFORMAT_RGBA32 );
                incoming_format = GL_RGBA;
            }
            else
            {
                new_format = SDL_AllocFormat( SDL_PIXELFORMAT_RGB24 );\
                incoming_format = GL_RGB;
            }
            
            if( !new_format )
                throw std::runtime_error(
                    "couldn't allocate SDL format for "
                    "yavsg::gl::_texture_general: "
                    + std::string( SDL_GetError() )
                );
            
            converted_surface = SDL_ConvertSurface(
                sdl_surface,
                new_format,
                0x00 // Flags (unused)
            );
            
            // Free format before possibly throwing an error
            SDL_FreeFormat( new_format );
            
            if( !converted_surface )
                throw std::runtime_error(
                    "couldn't convert SDL surface for "
                    "yavsg::gl::_texture_general: "
                    + std::string( SDL_GetError() )
                );
            
            sdl_surface = converted_surface;
        }
        
        try
        {
            upload(
                sdl_surface -> w,
                sdl_surface -> h,
                sdl_surface -> pixels,
                settings,
                flags,
                gl_internal_format,
                incoming_format,
                incoming_type
            );
            
            if( converted_surface )
                SDL_FreeSurface( converted_surface );
        }
        catch( ... )
        {
            if( converted_surface )
                SDL_FreeSurface( converted_surface );
            throw;
        }
    }
    
    void _texture_general::filtering( const texture_filter_settings& settings )
    {
        using magnify_mode = texture_filter_settings::magnify_mode;
        using  minify_mode = texture_filter_settings::minify_mode;
        using  mipmap_type = texture_filter_settings::mipmap_type;
        
        if( anisotropic_filtering_supported == NEED_CHECK )
        #ifdef __APPLE__
            // For now, assume this won't be compiled on one version of macOS
            // and expected to run on an older one
            anisotropic_filtering_supported = SUPPORTED;
        #else
            anisotropic_filtering_supported = (
                GLEW_EXT_texture_filter_anisotropic
                ? SUPPORTED
                : UNSUPPORTED
            );
        #endif
        
        glBindTexture( GL_TEXTURE_2D, gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind texture "
            + std::to_string( gl_id )
            + " for yavsg::gl::texture::filtering()"
        );
        
        GLint mag_filter;
        GLint min_filter;
        
        switch( settings.magnify )
        {
        case magnify_mode::NEAREST:
            mag_filter = GL_NEAREST;
            break;
        case magnify_mode::LINEAR:
            mag_filter = GL_LINEAR;
            break;
        }
        
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't set mag filter for texture "
            + std::to_string( gl_id )
            + " for yavsg::gl::texture::filtering()"
        );
        
        switch( settings.minify )
        {
        case minify_mode::NEAREST:
            {
                switch( settings.mipmaps )
                {
                case mipmap_type::NEAREST:
                    min_filter = GL_NEAREST_MIPMAP_NEAREST;
                    break;
                case mipmap_type::LINEAR:
                    min_filter = GL_NEAREST_MIPMAP_LINEAR;
                    break;
                case mipmap_type::NONE:
                    min_filter = GL_NEAREST;
                    break;
                }
            }
            break;
        case minify_mode::LINEAR:
            {
                switch( settings.mipmaps )
                {
                case mipmap_type::NEAREST:
                    min_filter = GL_LINEAR_MIPMAP_NEAREST;
                    break;
                case mipmap_type::LINEAR:
                    min_filter = GL_LINEAR_MIPMAP_LINEAR;
                    break;
                case mipmap_type::NONE:
                    min_filter = GL_LINEAR;
                    break;
                }
            }
            break;
        }
        
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't set min filter for texture "
            + std::to_string( gl_id )
            + " for yavsg::gl::texture::filtering()"
        );
        
        if( settings.mipmaps != mipmap_type::NONE )
        {
            glGenerateMipmap( GL_TEXTURE_2D );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't generate mipmaps for texture "
                + std::to_string( gl_id )
                + " for yavsg::gl::texture::filtering()"
            );
            
            if( anisotropic_filtering_supported == SUPPORTED )
            {
                GLfloat max_anisotropic_level;
                glGetFloatv(
                    GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,
                    &max_anisotropic_level
                );
                glTexParameterf(
                    GL_TEXTURE_2D,
                    GL_TEXTURE_MAX_ANISOTROPY_EXT,
                    max_anisotropic_level
                );
            }
        }
    }
    
    GLuint _texture_general::gl_texture_id()
    {
        return gl_id;
    }
} }
