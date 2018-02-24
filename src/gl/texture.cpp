#include "../../include/gl/texture.hpp"

#include "../../include/gl/_gl_base.hpp"
#include "../../include/gl/error.hpp"

#include <SDL2/SDL_image.h>

#include <exception>


namespace yavsg { namespace gl
{
    void load_bound_texture( const std::string& filename )
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
        //         << std::endl
        //     ;
        
        GLenum texture_mode = (
            sdl_surface -> format -> BytesPerPixel == 4 ?
            GL_RGBA : GL_RGB
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
            + std::string( "CURRENT_TEXTURE" ) // std::to_string( color_buffer )
            + " for yavsg::gl::load_bound_texture"
        );
        SDL_FreeSurface( sdl_surface );
        
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
        // float texture_border_color[] = { 1.0f, 0.0f, 0.0f };
        // glTexParameterfv(
        //     GL_TEXTURE_2D,
        //     GL_TEXTURE_BORDER_COLOR,
        //     texture_border_color
        // );
        
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't set S-wrap for texture "
            + std::string( "CURRENT_TEXTURE" ) // std::to_string( color_buffer )
            + " for yavsg::gl::load_bound_texture"
        );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't set T-wrap for texture "
            + std::string( "CURRENT_TEXTURE" ) // std::to_string( color_buffer )
            + " for yavsg::gl::load_bound_texture"
        );
        
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't set min filter for texture "
            + std::string( "CURRENT_TEXTURE" ) // std::to_string( color_buffer )
            + " for yavsg::gl::load_bound_texture"
        );
        
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't set min filter for texture "
            + std::string( "CURRENT_TEXTURE" ) // std::to_string( color_buffer )
            + " for yavsg::gl::load_bound_texture"
        );
        
        glGenerateMipmap( GL_TEXTURE_2D );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't generate mipmaps for texture "
            + std::string( "CURRENT_TEXTURE" ) // std::to_string( color_buffer )
            + " for yavsg::gl::load_bound_texture"
        );
    }
} }
