#include "../../include/gl/framebuffer.hpp"

// For yavsg::gl::write_only_framebuffer::dump_BMP()
#include "../../include/sdl/_sdl_base.hpp"
#include <fstream>
#include <iomanip>  // std::setw(), std::setfill()
#include <limits>   // std::numeric_limits
#include <sstream>


namespace yavsg { namespace gl // Write-only framebuffer implementation ////////
{
    write_only_framebuffer::write_only_framebuffer(
        GLuint gl_id,
        std::size_t num_color_targets,
        std::size_t width,
        std::size_t height
    ) :
        gl_id(   gl_id  ),
        num_color_targets( num_color_targets ),
        _width(  width  ),
        _height( height )
    {
        alpha_blending( alpha_blend_mode::DISABLED );
    }
    
    std::size_t write_only_framebuffer::width() const
    {
        return _width;
    }
    
    std::size_t write_only_framebuffer::height() const
    {
        return _height;
    }
    
    void write_only_framebuffer::bind()
    {
        glBindFramebuffer( GL_FRAMEBUFFER, gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind framebuffer "
            + std::to_string( gl_id )
            + std::string( gl_id == 0 ? " (default)" : "" )
            + " for yavsg::gl::base_framebuffer::bind()"
        );
        
        if( _alpha_blending == alpha_blend_mode::DISABLED )
        {
            glDisablei( GL_BLEND, 0 );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't disable blending for framebuffer "
                + std::to_string( gl_id )
                + std::string( gl_id == 0 ? " (default)" : "" )
                + " for yavsg::gl::base_framebuffer::alpha_blending()"
            );
        }
        else
        {
            glEnablei( GL_BLEND, 0 );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't enable blending for framebuffer "
                + std::to_string( gl_id )
                + std::string( gl_id == 0 ? " (default)" : "" )
                + " for yavsg::gl::base_framebuffer::alpha_blending()"
            );
        }
        
        if( _alpha_blending != alpha_blend_mode::DISABLED )
        {
            // TODO: glBlendEquationSeparatei( 0, GL_FUNC_ADD, GL_FUNC_ADD );
            glBlendEquationSeparate( GL_FUNC_ADD, GL_FUNC_ADD );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't set separate blending equations for framebuffer "
                + std::to_string( gl_id )
                + std::string( gl_id == 0 ? " (default)" : "" )
                + " for yavsg::gl::base_framebuffer::alpha_blending()"
            );
            
            // Use a switch to get warnings about possible unhandled cases in
            // the future
            switch( _alpha_blending )
            {
            case alpha_blend_mode::DISABLED:
                // Shouldn't really be possible, but makes the compiler shut
                // up without disabling the warning for this code
                throw std::logic_error(
                    "impossible blending state reached in "
                    "yavsg::gl::base_framebuffer::alpha_blending()"
                );
            case alpha_blend_mode::PREMULTIPLIED:
                glBlendFuncSeparate(
                // TODO: glBlendFuncSeparatei(
                //     0,
                    GL_ONE,
                    GL_ONE_MINUS_SRC_ALPHA,
                    GL_ONE,
                    GL_ONE_MINUS_SRC_ALPHA
                );
                break;
            case alpha_blend_mode::PREMULTIPLIED_DROP_ALPHA:
                glBlendFuncSeparate(
                // TODO: glBlendFuncSeparatei(
                //     0,
                    GL_ONE,
                    GL_ONE_MINUS_SRC_ALPHA,
                    GL_ZERO,
                    GL_ONE
                );
                break;
            case alpha_blend_mode::BASIC:
                glBlendFuncSeparate(
                // TODO: glBlendFuncSeparatei(
                //     0,
                    GL_SRC_ALPHA,
                    GL_ONE_MINUS_SRC_ALPHA,
                    GL_ONE,
                    GL_ONE_MINUS_SRC_ALPHA
                );
                break;
            case alpha_blend_mode::PASSTHROUGH:
                glBlendFuncSeparate(
                // TODO: glBlendFuncSeparatei(
                //     0,
                    GL_SRC_ALPHA,
                    GL_ONE_MINUS_SRC_ALPHA,
                    GL_ONE,
                    GL_ZERO
                );
                break;
            case alpha_blend_mode::DROP_ALPHA:
                glBlendFuncSeparate(
                // TODO: glBlendFuncSeparatei(
                //     0,
                    GL_SRC_ALPHA,
                    GL_ONE_MINUS_SRC_ALPHA,
                    GL_ZERO,
                    GL_ONE
                );
                break;
            }
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't set separate blending functions for framebuffer "
                + std::to_string( gl_id )
                + std::string( gl_id == 0 ? " (default)" : "" )
                + " for yavsg::gl::base_framebuffer::alpha_blending()"
            );
        }
    }
    
    alpha_blend_mode write_only_framebuffer::alpha_blending(
        alpha_blend_mode mode
    )
    {
        _alpha_blending = mode;
        return _alpha_blending;
    }
    
    alpha_blend_mode write_only_framebuffer::alpha_blending() const
    {
        return _alpha_blending;
    }
    
    void write_only_framebuffer::dump_BMP(
        const std::string& descriptive_name
    )
    {
        char* pixel_data = new char[ _width * _height * 4 ];
        
        try
        {
            glReadPixels(
                0,
                0,
                _width,
                _height,
                GL_RGBA,
                GL_UNSIGNED_INT_8_8_8_8,
                pixel_data
            );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't read framebuffer pixels for"
                " yavsg::gl::base_framebuffer::dump_BMP()"
            );
            
            auto surface = SDL_CreateRGBSurfaceFrom(
                pixel_data,
                _width,
                _height,
                4 * 8,
                4 * _width,
                0xFF << ( 3 * 8 ),
                0xFF << ( 2 * 8 ),
                0xFF << ( 1 * 8 ),
                0xFF << ( 0 * 8 )
            );
            
            if( !surface )
                throw std::runtime_error(
                    "couldn't create SDL surface from pixels for"
                    " yavsg::gl::base_framebuffer::dump_BMP(): "
                    + std::string( SDL_GetError() )
                );
            
            static std::size_t nth = 0;
            
            std::stringstream filename;
            filename
                << std::setw( std::numeric_limits< std::size_t >::digits10 + 1 )
                << std::setfill( '0' )
                << nth
                << " - "
                << descriptive_name
                << ".bmp"
            ;
            
            ++nth;
            
            auto save_error = SDL_SaveBMP( surface, filename.str().c_str() );
            SDL_FreeSurface( surface );
            
            if( save_error )
                throw std::runtime_error(
                    "couldn't open file `"
                    + filename.str()
                    + "` for yavsg::gl::base_framebuffer::dump_BMP(): "
                    + std::string( SDL_GetError() )
                );
            
            delete[] pixel_data;
        }
        catch( ... )
        {
            delete[] pixel_data;
            throw;
        }
    }
} }
