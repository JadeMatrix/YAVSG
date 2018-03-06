#include "../../include/gl/texture.hpp"

#include "../../include/gl/_gl_base.hpp"
#include "../../include/gl/error.hpp"

#include <SDL2/SDL_image.h>

#include <exception>
#include <utility>      // std::swap()

// DEBUG:
#include <iostream>


namespace
{
    bool format_has_alpha( Uint32 format )
    {
        switch( format )
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
            return true;
        default:
            return false;
        }
    }
    
    enum
    {
        NEED_CHECK,
        SUPPORTED,
        UNSUPPORTED
    } anisotropic_filtering_supported = NEED_CHECK;
}


namespace yavsg { namespace gl // Texture class implementation /////////////////
{
    texture::texture()
    {
        glGenTextures( 1, &gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't generate texture for yavsg::gl::texture"
        );
    }
    
    texture::texture( SDL_Surface* sdl_surface ) : texture()
    {
        // Surface is passed as a pointer for consistency with the SDL API, so
        // we'll want to check if it's NULL
        if( !sdl_surface )
            throw std::runtime_error(
                "null SDL_Surface* for yavsg::gl::texture"
            );
        
        GLenum texture_mode;
        SDL_Surface* converted_surface = nullptr;
        
        if( sdl_surface -> format -> format == SDL_PIXELFORMAT_RGBA8888 )
            texture_mode = GL_RGBA;
        else if( sdl_surface -> format -> format == SDL_PIXELFORMAT_RGB888 )
            texture_mode = GL_RGB;
        else // Convert pixels
        {
            SDL_PixelFormat* new_format = nullptr;
            
            if( format_has_alpha( sdl_surface -> format -> format ) )
            {
                new_format = SDL_AllocFormat( SDL_PIXELFORMAT_RGBA32 );
                texture_mode = GL_RGBA;
            }
            else
            {
                new_format = SDL_AllocFormat( SDL_PIXELFORMAT_RGB24 );\
                texture_mode = GL_RGB;
            }
            
            if( !new_format )
                throw std::runtime_error(
                    "couldn't allocate SDL format for yavsg::gl::texture: "
                    + std::string( SDL_GetError() )
                );
            
            converted_surface = SDL_ConvertSurface(
                sdl_surface,
                new_format,
                0x00
            );
            
            // Free format before possibly throwing an error
            SDL_FreeFormat( new_format );
            
            if( !converted_surface )
                throw std::runtime_error(
                    "couldn't convert SDL surface for yavsg::gl::texture: "
                    + std::string( SDL_GetError() )
                );
            
            sdl_surface = converted_surface;
        }
        
        try
        {
            glBindTexture( GL_TEXTURE_2D, gl_id );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't bind texture "
                + std::to_string( gl_id )
                + " for yavsg::gl::texture"
            );
            
            glTexImage2D(   // Loads starting at 0,0 as bottom left
                GL_TEXTURE_2D,
                0,                      // LoD, 0 = base
                texture_mode,           // Internal format
                sdl_surface -> w,       // Width
                sdl_surface -> h,       // Height
                0,                      // Border; must be 0
                texture_mode,           // Incoming format
                GL_UNSIGNED_BYTE,       // Pixel type
                sdl_surface -> pixels   // Pixel data
            );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't allocate "
                + std::to_string( sdl_surface -> w )
                + "x"
                + std::to_string( sdl_surface -> h )
                + " texture "
                + std::to_string( gl_id )
                + " for yavsg::gl::texture"
            );
        }
        catch( ... )
        {
            if( converted_surface )
                SDL_FreeSurface( converted_surface );
            throw;
        }
        
        if( converted_surface )
            SDL_FreeSurface( converted_surface );
    }
    
    texture::texture( texture&& o )
    {
        // gl_id = o.gl_id;
        // o.gl_id = default_texture_gl_id;
        std::swap( gl_id, o.gl_id );
    }
    
    texture::~texture()
    {
        if( gl_id != default_texture_gl_id )
        {
            glDeleteTextures( 1, &gl_id );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't delete texture "
                + std::to_string( gl_id )
                + " for yavsg::gl::texture::~texture()"
            );
        }
    }
    
    void texture::filtering( const filter_settings& settings )
    {
        using magnify_mode = filter_settings::magnify_mode;
        using  minify_mode = filter_settings::minify_mode;
        using  mipmap_type = filter_settings::mipmap_type;
        
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
    
    GLuint texture::gl_texture_id()
    {
        return gl_id;
    }
} }


namespace yavsg { namespace gl // Texture static method implementations ////////
{
    texture texture::from_file( const std::string& filename )
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
        // // DEBUG:
        // else
        //     std::cout
        //         << "loaded texture \""
        //         << filename
        //         << "\": w="
        //         << sdl_surface -> w
        //         << " h="
        //         << sdl_surface -> h
        //         << " bpp="
        //         << ( int )( sdl_surface -> format -> BytesPerPixel )
        //         << " fmt="
        //         << SDL_GetPixelFormatName( sdl_surface -> format -> format )
        //         << std::endl
        //     ;
        
        texture created_texture = texture( sdl_surface );
        SDL_FreeSurface( sdl_surface );
        return created_texture;
    }
    
    // texture texture::rgba8_from_bytes(
    //     unsigned int width,
    //     unsigned int height,
    //     const char* bytes
    // )
    // {
        
    // }
    
    texture texture::make_empty()
    {
        return texture();
    }
} }
