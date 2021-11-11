#include <yavsg/gl/framebuffer.hpp>

// For yavsg::gl::write_only_framebuffer::dump_BMP()
#include <yavsg/sdl/sdl.hpp>

#include <assert.h>
#include <fstream>
#include <iomanip>  // std::setw(), std::setfill()
#include <limits>   // std::numeric_limits
#include <sstream>
#include <vector>


namespace JadeMatrix::yavsg::gl // Write-only framebuffer implementation ///////
{
    write_only_framebuffer& write_only_framebuffer::operator =(
        write_only_framebuffer&& o
    )
    {
        std::swap( _width         , o._width          );
        std::swap( _height        , o._height         );
        std::swap( _alpha_blending, o._alpha_blending );
        std::swap( gl_id          , o.gl_id           );
        
        return *this;
    }
    
    write_only_framebuffer::write_only_framebuffer(
        GLuint gl_id,
        std::size_t num_color_targets,
        std::size_t width,
        std::size_t height
    ) :
        _width ( width  ),
        _height( height ),
        gl_id  ( gl_id  ),
        num_color_targets( num_color_targets )
    {
        /*
        _width
        _height
        _alpha_blending
        gl_id
        num_color_targets
        */
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
    
    // TODO: make this a debug function
    void write_only_framebuffer::dump_BMP(
        const std::string& descriptive_name
    )
    {
        assert( _width  <= std::numeric_limits< GLsizei >::max() );
        assert( _height <= std::numeric_limits< GLsizei >::max() );
        auto w = static_cast< GLsizei >( _width  );
        auto h = static_cast< GLsizei >( _height );
        
        // TODO: make class member & only realloc when buffer size increases
        std::vector< char > pixels( _width * _height * 4 );
        
        glReadPixels(
            0,
            0,
            w,
            h,
            GL_RGBA,
            GL_UNSIGNED_INT_8_8_8_8,
            pixels.data()
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't read framebuffer pixels for"
            " yavsg::gl::base_framebuffer::dump_BMP()"
        );
        
        auto surface = SDL_CreateRGBSurfaceFrom(
            pixels.data(),
            w,
            h,
            4 * 8,
            4 * w,
            0xFFu << ( 3 * 8 ),
            0xFFu << ( 2 * 8 ),
            0xFFu << ( 1 * 8 ),
            0xFFu << ( 0 * 8 )
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
    }
}
