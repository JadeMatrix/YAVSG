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


namespace // Alpha detection functions /////////////////////////////////////////
{
    template< typename T >
    bool has_varying_alpha(
        const  void* data,
        std::size_t  sample_count,
        typename std::enable_if<
            std::is_integral< T >::value,
            T
        >::type* = nullptr
    )
    {
        auto in_data = static_cast< const T* >( data );
        auto max = std::numeric_limits< T >::max();
        
        for( std::size_t i = 0; i < sample_count; ++i )
            if( in_data[ 4 * i + 3 ] != max )
                return true;
        
        return false;
    }
    
    template< typename T >
    bool has_varying_alpha(
        const  void* data,
        std::size_t  sample_count,
        typename std::enable_if<
            std::is_floating_point< T >::value,
            T
        >::type* = nullptr
    )
    {
        auto in_data = static_cast< const T* >( data );
        
        for( std::size_t i = 0; i < sample_count; ++i )
            if( in_data[ 4 * i + 3 ] != 1.0f )
                return true;
        
        return false;
    }
}


namespace // Alpha premultiply functions ///////////////////////////////////////
{
    template< typename T >
    void premultiply(
        const  void* data,
               void* preprocessed_data,
        std::size_t  sample_count,
        
        typename std::enable_if<
            std::is_integral< T >::value,
            T
        >::type* = nullptr
    )
    {
        auto  in_data = static_cast< const T* >(              data );
        auto out_data = static_cast<       T* >( preprocessed_data );
        
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
               void* preprocessed_data,
        std::size_t  sample_count,
        
        typename std::enable_if<
            std::is_floating_point< T >::value,
            T
        >::type* = nullptr
    )
    {
        auto  in_data = static_cast< const T* >(              data );
        auto out_data = static_cast<       T* >( preprocessed_data );
        
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
        char* preprocessed_data = nullptr;
        
        try
        {
            if( flags & texture_flags::ALLOCATE_ONLY )
                data = nullptr;
            else if( !data )
                throw std::runtime_error(
                    "null data do _texture_general but allocate-only flag not "
                    "set"
                );
            
            // We can skip a whole bunch of potential preprocessing if we're
            // just going to end up discarding the alpha anyways
            bool internal_has_alpha;
            switch( gl_internal_format )
            {
            case GL_RGBA:
            case GL_RGBA8I:
            case GL_RGBA8UI:
            case GL_RGBA16I:
            case GL_RGBA16UI:
            case GL_RGBA32I:
            case GL_RGBA32UI:
            case GL_RGBA16F:
            case GL_RGBA32F:
                internal_has_alpha = true;
            default:
                internal_has_alpha = false;
            }
            
            auto sample_count = width * height;
            
            // Don't bother storing alpha channel in OpenGL memory if it's all
            // 100%
            // TODO: Make this abstract to G = 0%, B = 0%, A = 100%
            if( data && internal_has_alpha && gl_format == GL_RGBA )
            {
                switch( gl_type )
                {
                case GL_BYTE:
                    internal_has_alpha = has_varying_alpha< GLbyte >(
                        data,
                        sample_count
                    );
                    break;
                case GL_UNSIGNED_BYTE:
                    internal_has_alpha = has_varying_alpha< GLubyte >(
                        data,
                        sample_count
                    );
                    break;
                case GL_SHORT:
                    internal_has_alpha = has_varying_alpha< GLshort >(
                        data,
                        sample_count
                    );
                    break;
                case GL_UNSIGNED_SHORT:
                    internal_has_alpha = has_varying_alpha< GLushort >(
                        data,
                        sample_count
                    );
                    break;
                case GL_INT:
                    internal_has_alpha = has_varying_alpha< GLint >(
                        data,
                        sample_count
                    );
                    break;
                case GL_UNSIGNED_INT:
                    internal_has_alpha = has_varying_alpha< GLuint >(
                        data,
                        sample_count
                    );
                    break;
                case GL_HALF_FLOAT:
                    internal_has_alpha = has_varying_alpha< GLhalf >(
                        data,
                        sample_count
                    );
                    break;
                case GL_FLOAT:
                    internal_has_alpha = has_varying_alpha< GLfloat >(
                        data,
                        sample_count
                    );
                    break;
                case GL_DOUBLE:
                    internal_has_alpha = has_varying_alpha< GLdouble >(
                        data,
                        sample_count
                    );
                    break;
                default:
                    throw std::runtime_error(
                        "uknown/unsupported OpenGL type "
                        + std::to_string( gl_type )
                        + " for yavsg::gl::_texture_general"
                    );
                }
                
                if( !internal_has_alpha )
                    switch( gl_internal_format )
                    {
                    case GL_RGBA:
                        gl_internal_format = GL_RGB;
                        break;
                    case GL_RGBA8I:
                        gl_internal_format = GL_RGB8I;
                        break;
                    case GL_RGBA8UI:
                        gl_internal_format = GL_RGB8UI;
                        break;
                    case GL_RGBA16I:
                        gl_internal_format = GL_RGB16I;
                        break;
                    case GL_RGBA16UI:
                        gl_internal_format = GL_RGB16UI;
                        break;
                    case GL_RGBA32I:
                        gl_internal_format = GL_RGB32I;
                        break;
                    case GL_RGBA32UI:
                        gl_internal_format = GL_RGB32UI;
                        break;
                    case GL_RGBA16F:
                        gl_internal_format = GL_RGB16F;
                        break;
                    case GL_RGBA32F:
                        gl_internal_format = GL_RGB32F;
                        break;
                    }
            }
            
            // Premultiply colors by alpha
            if(
                data
                && internal_has_alpha
                && gl_format == GL_RGBA
                && !( flags & texture_flags::DISABLE_PREMULTIPLIED_ALPHA )
            )
            {
                // Any format that requires multiplying will have 4 channels
                std::size_t channels = 4;
                
                switch( gl_type )
                {
                case GL_BYTE:
                    preprocessed_data = new char[
                        sample_count * channels * sizeof( GLbyte )
                    ];
                    premultiply< GLbyte >(
                        data,
                        preprocessed_data,
                        sample_count
                    );
                    break;
                case GL_UNSIGNED_BYTE:
                    preprocessed_data = new char[
                        sample_count * channels * sizeof( GLubyte )
                    ];
                    premultiply< GLubyte >(
                        data,
                        preprocessed_data,
                        sample_count
                    );
                    break;
                case GL_SHORT:
                    preprocessed_data = new char[
                        sample_count * channels * sizeof( GLshort )
                    ];
                    premultiply< GLshort >(
                        data,
                        preprocessed_data,
                        sample_count
                    );
                    break;
                case GL_UNSIGNED_SHORT:
                    preprocessed_data = new char[
                        sample_count * channels * sizeof( GLushort )
                    ];
                    premultiply< GLushort >(
                        data,
                        preprocessed_data,
                        sample_count
                    );
                    break;
                case GL_INT:
                    preprocessed_data = new char[
                        sample_count * channels * sizeof( GLint )
                    ];
                    premultiply< GLint >(
                        data,
                        preprocessed_data,
                        sample_count
                    );
                    break;
                case GL_UNSIGNED_INT:
                    preprocessed_data = new char[
                        sample_count * channels * sizeof( GLuint )
                    ];
                    premultiply< GLuint >(
                        data,
                        preprocessed_data,
                        sample_count
                    );
                    break;
                // case GL_HALF_FLOAT:
                //     preprocessed_data = new char[
                //         sample_count * channels * sizeof( GLhalf )
                //     ];
                //     premultiply< GLhalf >(
                //         data,
                //         preprocessed_data,
                //         sample_count
                //     );
                //     break;
                case GL_FLOAT:
                    preprocessed_data = new char[
                        sample_count * channels * sizeof( GLfloat )
                    ];
                    premultiply< GLfloat >(
                        data,
                        preprocessed_data,
                        sample_count
                    );
                    break;
                // case GL_DOUBLE:
                //     preprocessed_data = new char[
                //         sample_count * channels * sizeof( GLdouble )
                //     ];
                //     premultiply< GLdouble >(
                //         data,
                //         preprocessed_data,
                //         sample_count
                //     );
                //     break;
                default:
                    throw std::runtime_error(
                        "uknown/unsupported OpenGL type "
                        + std::to_string( gl_type )
                        + " for yavsg::gl::_texture_general"
                    );
                }
                
                data = preprocessed_data;
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
                + ( data ? "" : " from null data" )
                + " for yavsg::gl::_texture_general"
            );
            
            filtering( settings );
            
            if( preprocessed_data )
                delete[] preprocessed_data;
        }
        catch( ... )
        {
            if( preprocessed_data )
                delete[] preprocessed_data;
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
