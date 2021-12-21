#include <yavsg/gl/texture_utilities.hpp>

#include <yavsg/gl/error.hpp>
#include <yavsg/math/scalar_operations.hpp> // power

#include <fmt/format.h>
#include <doctest/doctest.h>    // REQUIRE

#include <algorithm>
#include <limits>
#include <mutex>        // mutex, once_flag, call_once
#include <stdexcept>    // runtime_error
#include <type_traits>  // conditional, is_floating_point, enable_if_t
#include <utility>      // swap


namespace
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;
}


namespace // Anisotropic filtering support /////////////////////////////////////
{
    std::once_flag aniso_filter_flag;
    bool           aniso_filter_value;
    
    bool anisotropic_filtering_supported()
    {
        std::call_once(
            aniso_filter_flag,
            [](){
            #ifdef __APPLE__
                // For now, assume this won't be compiled on one version of
                // macOS and expected to run on an older one
                aniso_filter_value = true;
            #else
                aniso_filter_value = static_cast< bool >(
                    GLEW_EXT_texture_filter_anisotropic
                );
            #endif
            }
        );
        
        return aniso_filter_value;
    }
}


namespace // Alpha & gamma preprocess functions ////////////////////////////////
{
    template< typename T > T linearize_sample( T sample )
    {
        using f = typename std::conditional<
            std::is_floating_point< T >::value,
            T,
            double
        >::type;
        auto f_sample = static_cast< f >( sample );
        
        // See http://docs.gl/gl3/glTexImage2D
        if( f_sample <= static_cast< f >( 0.04045 ) )
        {
            return f_sample / static_cast< f >( 12.92 );
        }
        else
        {
            return JadeMatrix::yavsg::power(
                (
                    f_sample + static_cast< f >( 0.055 )
                ) / static_cast< f >( 1.055 ),
                static_cast< f >( 2.4 )
            );
        }
    }
    
    template< typename T > auto premultiply(
        void const* data,
        void      * preprocessed_data,
        std::size_t sample_count,
        std::size_t channels,
        JadeMatrix::yavsg::gl::texture_flags_type flags
    ) -> std::enable_if_t< std::is_integral< T >::value, bool >
    {
        auto has_varying_alpha = false;
        auto const multiply_alpha = !(
            flags
            & JadeMatrix::yavsg::gl::texture_flag::disable_premultiplied_alpha
        );
        auto const linearize = !(
            flags
            & JadeMatrix::yavsg::gl::texture_flag::linear_input
        );
        
        auto  in_data = static_cast< T const* >(              data );
        auto out_data = static_cast< T      * >( preprocessed_data );
        
        auto max = static_cast< double >( std::numeric_limits< T >::max() );
        
        for( std::size_t i = 0; i < sample_count; ++i )
        {
            auto alpha = 1.0;
            if( channels > 3 && (
                in_data[ channels * i + 3 ] != std::numeric_limits< T >::max()
            ) )
            {
                has_varying_alpha = true;
                alpha = (
                    static_cast< double >( in_data[ channels * i + 3 ] ) / max
                );
            }
            
            for( std::size_t j = 0; j < channels; ++j )
            {
                if( j == 3 )
                {
                    // No conversions applied to alpha channel
                    out_data[ channels * i + j ] = in_data[ channels * i + j ];
                }
                else
                {
                    auto in_sample = static_cast< double >(
                        in_data[ channels * i + j ]
                    ) / max;
                    
                    // Alpha premultiplication
                    if( multiply_alpha )
                    {
                        // TODO: Don't perform multiplication if alpha is 100%
                        in_sample *= alpha;
                    }
                    
                    // Color space linearization (gamma de-correction)
                    if( linearize )
                    {
                        in_sample = linearize_sample( in_sample );
                    }
                    
                    out_data[ channels * i + j ] = static_cast< T >(
                        in_sample * max
                    );
                }
            }
        }
        
        return has_varying_alpha;
    }
    
    template< typename T > auto premultiply(
        void const* data,
        void      * preprocessed_data,
        std::size_t sample_count,
        std::size_t channels,
        JadeMatrix::yavsg::gl::texture_flags_type flags
    ) -> std::enable_if_t< std::is_floating_point< T >::value, bool >
    {
        auto has_varying_alpha = false;
        auto const multiply_alpha = !(
            flags
            & JadeMatrix::yavsg::gl::texture_flag::disable_premultiplied_alpha
        );
        auto const linearize = !(
            flags
            & JadeMatrix::yavsg::gl::texture_flag::linear_input
        );
        
        auto  in_data = static_cast< T const* >(              data );
        auto out_data = static_cast< T      * >( preprocessed_data );
        
        for( std::size_t i = 0; i < sample_count; ++i )
        {
            T alpha = 1.0f;
            if( channels > 3 && ( in_data[ channels * i + 3 ] != 1.0f ) )
            {
                has_varying_alpha = true;
                alpha = in_data[ channels * i + 3 ];
            }
            
            for( std::size_t j = 0; j < channels; ++j )
            {
                if( j == 3 )
                {
                    // No conversions applied to alpha channel
                    out_data[ channels * i + j ] = in_data[ channels * i + j ];
                }
                else
                {
                    auto in_sample = in_data[ channels * i + j ];
                    
                    // Alpha premultiplication
                    if( multiply_alpha )
                    {
                        // TODO: Don't perform multiplication if alpha is 100%
                        in_sample *= alpha;
                    }
                    
                    // Color space linearization (gamma de-correction)
                    if( linearize )
                    {
                        in_sample = linearize_sample( in_sample );
                    }
                    
                    out_data[ channels * i + j ] = in_sample;
                }
            }
        }
        
        return has_varying_alpha;
    }
}


namespace JadeMatrix::yavsg::gl // Texture data processing implementation //////
{
    texture_upload_data process_texture_data(
        texture_upload_data upload_data,
        texture_flags_type  flags
    )
    {
        if( flags & texture_flag::allocate_only )
        {
            upload_data.data = nullptr;
        }
        else if( !upload_data.data )
        {
            throw std::runtime_error(
                "null data to yavsg::gl::process_texture_data() but allocate-"
                "only flag not set"s
            );
        }
        
        // We can skip a whole bunch of potential preprocessing if we're just
        // going to end up discarding the alpha anyways
        auto internal_has_alpha = false;
        switch( upload_data.gl_internal_format )
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
        // case GL_RGBA64F:
            internal_has_alpha = true;
            break;
        }
        
        auto sample_count = upload_data.width * upload_data.height;
        
        auto const needs_alpha_pass = (
            internal_has_alpha
            && !( flags & texture_flag::disable_premultiplied_alpha )
        );
        auto const needs_drop_pass = !(
            flags & texture_flag::keep_unused_channels
        );
        auto const needs_gamma_pass = !( flags & texture_flag::linear_input );
        
        // Perform alpha premultiplication & gamma correction
        // TODO: Just use OpenGL sRGB type if internal format is RGB(A)8
        if( upload_data.data && (
               needs_alpha_pass
            || needs_drop_pass
            || needs_gamma_pass
        ) )
        {
            std::size_t channels;
            std::unique_ptr< std::byte[] > preprocessed_data;
            
            switch( upload_data.gl_incoming_format )
            {
            case GL_RED : channels = 1; break;
            case GL_RG  : channels = 2; break;
            case GL_RGB : channels = 3; break;
            case GL_RGBA: channels = 4; break;
            default:
                throw std::runtime_error( fmt::format(
                    "uknown/unsupported OpenGL pixel format {} for "
                        "yavsg::gl::process_texture_data()"sv,
                    upload_data.gl_incoming_type
                ) );
            }
            
            texture_flags_type modified_flags = flags;
            if( upload_data.gl_incoming_format != GL_RGBA )
            {
                modified_flags |= texture_flag::disable_premultiplied_alpha;
            }
            
            std::function< bool(
                void const*,
                void*,
                std::size_t,
                std::size_t,
                yavsg::gl::texture_flags_type
            ) > type_premultiply;
            std::size_t type_size;
            
            switch( upload_data.gl_incoming_type )
            {
            case GL_BYTE:
                type_size = sizeof( GLbyte );
                type_premultiply = premultiply< GLbyte >;
                break;
            case GL_UNSIGNED_BYTE:
                type_size = sizeof( GLubyte );
                type_premultiply = premultiply< GLubyte >;
                break;
            case GL_SHORT:
                type_size = sizeof( GLshort );
                type_premultiply = premultiply< GLshort >;
                break;
            case GL_UNSIGNED_SHORT:
                type_size = sizeof( GLushort );
                type_premultiply = premultiply< GLushort >;
                break;
            case GL_INT:
                type_size = sizeof( GLint );
                type_premultiply = premultiply< GLint >;
                break;
            case GL_UNSIGNED_INT:
                type_size = sizeof( GLuint );
                type_premultiply = premultiply< GLuint >;
                break;
            // case GL_HALF_FLOAT:
            //     type_size = sizeof( GLhalf );
            //     type_premultiply = premultiply< GLhalf >;
            //     break;
            case GL_FLOAT:
                type_size = sizeof( GLfloat );
                type_premultiply = premultiply< GLfloat >;
                break;
            // case GL_DOUBLE:
            //     type_size = sizeof( GLdouble );
            //     type_premultiply = premultiply< GLdouble >;
            //     break;
            default:
                throw std::runtime_error( fmt::format(
                    "uknown/unsupported OpenGL type {} for "
                        "yavsg::gl::process_texture_data()"sv,
                    upload_data.gl_incoming_type
                ) );
            }
            
            preprocessed_data = std::unique_ptr< std::byte[] >( new std::byte[
                sample_count * channels * type_size
            ] );
            internal_has_alpha = type_premultiply(
                upload_data.data.get(),
                preprocessed_data.get(),
                sample_count,
                channels,
                modified_flags
            );
            
            std::swap( upload_data.data, preprocessed_data );
        }
        
        // Don't bother storing alpha channel in OpenGL memory if it's all 100%
        // TODO: Make this abstract to G = 0%, B = 0%, A = 100%
        if( !internal_has_alpha )
        {
            switch( upload_data.gl_internal_format )
            {
            case GL_RGBA    : upload_data.gl_internal_format = GL_RGB    ; break;
            case GL_RGBA8I  : upload_data.gl_internal_format = GL_RGB8I  ; break;
            case GL_RGBA8UI : upload_data.gl_internal_format = GL_RGB8UI ; break;
            case GL_RGBA16I : upload_data.gl_internal_format = GL_RGB16I ; break;
            case GL_RGBA16UI: upload_data.gl_internal_format = GL_RGB16UI; break;
            case GL_RGBA32I : upload_data.gl_internal_format = GL_RGB32I ; break;
            case GL_RGBA32UI: upload_data.gl_internal_format = GL_RGB32UI; break;
            case GL_RGBA16F : upload_data.gl_internal_format = GL_RGB16F ; break;
            case GL_RGBA32F : upload_data.gl_internal_format = GL_RGB32F ; break;
            }
        }
        
        return upload_data;
    }
    
    texture_upload_data process_texture_data(
        SDL_Surface      * sdl_surface,
        texture_flags_type flags,
        GLint              gl_internal_format
    )
    {
        // Surface is passed as a pointer for consistency with the SDL API, so
        // we'll want to check if it's NULL
        if( !sdl_surface )
        {
            throw std::runtime_error(
                "null SDL_Surface* for yavsg::gl::process_texture_data()"s
            );
        }
        
        GLenum incoming_format;
        GLenum incoming_type = GL_UNSIGNED_BYTE;
        SDL_Surface* converted_surface = nullptr;
        
        std::size_t channels;
        
        if( sdl_surface->format->format == SDL_PIXELFORMAT_RGBA8888 )
        {
            incoming_format = GL_RGBA; // Rgb + A
            channels = 4;
        }
        else if( sdl_surface->format->format == SDL_PIXELFORMAT_BGRA8888 )
        {
            incoming_format = GL_BGRA; // Bgr + A
            channels = 4;
        }
        else if( sdl_surface->format->format == SDL_PIXELFORMAT_RGB888 )
        {
            incoming_format = GL_RGB;  // Rgb
            channels = 3;
        }
        else if( sdl_surface->format->format == SDL_PIXELFORMAT_BGR888 )
        {
            incoming_format = GL_BGR;  // Bgr
            channels = 3;
        }
        else // Convert pixels
        {
            SDL_PixelFormat* new_format = nullptr;
            
            bool format_has_alpha = false;
            switch( sdl_surface->format->format )
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
                break;
            }
            
            if( format_has_alpha )
            {
                new_format = SDL_AllocFormat( SDL_PIXELFORMAT_RGBA32 );
                incoming_format = GL_RGBA;
                channels = 4;
            }
            else
            {
                new_format = SDL_AllocFormat( SDL_PIXELFORMAT_RGB24 );\
                incoming_format = GL_RGB;
                channels = 3;
            }
            
            if( !new_format )
            {
                throw std::runtime_error( fmt::format(
                    "couldn't allocate SDL format for "
                        "yavsg::gl::process_texture_data(): {}"sv,
                    SDL_GetError()
                ) );
            }
            
            converted_surface = SDL_ConvertSurface(
                sdl_surface,
                new_format,
                0x00 // Flags (unused)
            );
            
            // Free format before possibly throwing an error
            SDL_FreeFormat( new_format );
            
            if( !converted_surface )
            {
                throw std::runtime_error( fmt::format(
                    "couldn't convert SDL surface for "
                        "yavsg::gl::process_texture_data(): {}"sv,
                    SDL_GetError()
                ) );
            }
            
            std::swap( sdl_surface, converted_surface );
            SDL_FreeSurface( converted_surface );
        }
        
        std::size_t width  = static_cast< std::size_t >( sdl_surface->w );
        std::size_t height = static_cast< std::size_t >( sdl_surface->h );
        
        // Copy data; need to do this because the data may/will be freed
        // elsewhere and be freed with `delete[]`, but SDL allocates with
        // `malloc()` since it's written in C
        auto sample_count = channels * width * height;
        auto sdl_surface_data = new std::byte[ sample_count ];
        std::copy_n(
            static_cast< std::byte* >( sdl_surface->pixels ),
            sample_count,
            sdl_surface_data
        );
        
        SDL_FreeSurface( sdl_surface );
        
        return process_texture_data(
            {
                gl_internal_format,
                width,
                height,
                incoming_format,
                incoming_type,
                std::unique_ptr< std::byte[] >{ sdl_surface_data }
            },
            flags
        );
    }
    
    void upload_texture_data(
        GLuint                         gl_id,
        texture_upload_data            upload_data,
        texture_filter_settings const& settings
    )
    {
        gl::BindTexture( GL_TEXTURE_2D, gl_id );
        
        REQUIRE( upload_data.width  <= std::numeric_limits< GLsizei >::max() );
        REQUIRE( upload_data.height <= std::numeric_limits< GLsizei >::max() );
        auto data_width  = static_cast< GLsizei >( upload_data.width  );
        auto data_height = static_cast< GLsizei >( upload_data.height );
        
        gl::TexImage2D(   // Loads starting at 0,0 as bottom left
            GL_TEXTURE_2D,
            0,                              // LoD, 0 = base
            upload_data.gl_internal_format, // Internal format
            data_width,                     // Width
            data_height,                    // Height
            0,                              // Border; must be 0
            upload_data.gl_incoming_format, // Incoming format
            upload_data.gl_incoming_type,   // Pixel type
            upload_data.data.get()          // Pixel data
        );
        
        set_bound_texture_filtering( settings );
    }
    
    void set_bound_texture_filtering( texture_filter_settings const& settings )
    {
        using magnify_mode = texture_filter_settings::magnify_mode;
        using  minify_mode = texture_filter_settings::minify_mode;
        using  mipmap_type = texture_filter_settings::mipmap_type;
        
        GLint mag_filter = 0;
        GLint min_filter = 0;
        
        switch( settings.magnify )
        {
        case magnify_mode::nearest: mag_filter = GL_NEAREST; break;
        case magnify_mode::linear : mag_filter = GL_LINEAR ; break;
        }
        
        gl::TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter );
        
        switch( settings.minify )
        {
        case minify_mode::nearest:
            {
                switch( settings.mipmaps )
                {
                case mipmap_type::nearest:
                    min_filter = GL_NEAREST_MIPMAP_NEAREST;
                    break;
                case mipmap_type::linear:
                    min_filter = GL_NEAREST_MIPMAP_LINEAR;
                    break;
                case mipmap_type::none:
                    min_filter = GL_NEAREST;
                    break;
                }
            }
            break;
        case minify_mode::linear:
            {
                switch( settings.mipmaps )
                {
                case mipmap_type::nearest:
                    min_filter = GL_LINEAR_MIPMAP_NEAREST;
                    break;
                case mipmap_type::linear:
                    min_filter = GL_LINEAR_MIPMAP_LINEAR;
                    break;
                case mipmap_type::none:
                    min_filter = GL_LINEAR;
                    break;
                }
            }
            break;
        }
        
        gl::TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter );
        
        if( settings.mipmaps != mipmap_type::none )
        {
            gl::GenerateMipmap( GL_TEXTURE_2D );
            
            if( anisotropic_filtering_supported() )
            {
                GLfloat max_anisotropic_level;
                gl::GetFloatv(
                    GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,
                    &max_anisotropic_level
                );
                gl::TexParameterf(
                    GL_TEXTURE_2D,
                    GL_TEXTURE_MAX_ANISOTROPY_EXT,
                    max_anisotropic_level
                );
            }
        }
    }
}
