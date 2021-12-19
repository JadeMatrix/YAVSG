#include <yavsg/gl/framebuffer.hpp>

// For yavsg::gl::write_only_framebuffer::dump_BMP()
#include <yavsg/sdl/sdl.hpp>

#include <fmt/format.h>         // format
#include <doctest/doctest.h>    // REQUIRE

#include <assert.h>
#include <fstream>
#include <iomanip>      // std::setw(), std::setfill()
#include <limits>       // std::numeric_limits
#include <mutex>        // std::call_once, std::once_flag
#include <sstream>
#include <stdexcept>    // std::runtime_error
#include <vector>


namespace
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;
    
    std::once_flag gl_max_got_flags;
}


namespace JadeMatrix::yavsg::gl
{
    GLenum color_attachment_name( std::size_t nth )
    {
        static GLint max_color_attachments;
        
        std::call_once(
            gl_max_got_flags,
            [](){
                glGetIntegerv(
                    GL_MAX_COLOR_ATTACHMENTS,
                    &max_color_attachments
                );
                YAVSG_GL_THROW_FOR_ERRORS(
                    "couldn't read GL_MAX_COLOR_ATTACHMENTS for "
                    "yavsg::gl::color_attachment_name()"s
                );
                REQUIRE( max_color_attachments > 0 );
            }
        );
        
        static_assert(
              std::numeric_limits< GLint       >::max()
            < std::numeric_limits< std::size_t >::max()
        );
        if( nth >= static_cast< std::size_t >( max_color_attachments ) )
        {
            throw std::runtime_error(fmt::format(
                "cannot have more than {} color attachments"sv,
                max_color_attachments
            ));
        }
        
        return static_cast< GLenum >( GL_COLOR_ATTACHMENT0 + nth );
    }
}


namespace JadeMatrix::yavsg::gl // Write-only framebuffer implementation ///////
{
    write_only_framebuffer& write_only_framebuffer::operator =(
        write_only_framebuffer&& o
    )
    {
        std::swap( width_         , o.width_          );
        std::swap( height_        , o.height_         );
        std::swap( alpha_blending_, o.alpha_blending_ );
        std::swap( gl_id_         , o.gl_id_          );
        
        return *this;
    }
    
    write_only_framebuffer::write_only_framebuffer(
        GLuint      gl_id,
        std::size_t num_color_targets,
        std::size_t width,
        std::size_t height
    ) :
        num_color_targets( num_color_targets ),
        width_ ( width  ),
        height_( height ),
        gl_id_ ( gl_id  )
    {
        alpha_blending( alpha_blend_mode::DISABLED );
    }
    
    std::size_t write_only_framebuffer::width() const
    {
        return width_;
    }
    
    std::size_t write_only_framebuffer::height() const
    {
        return height_;
    }
    
    void write_only_framebuffer::bind()
    {
        glBindFramebuffer( GL_FRAMEBUFFER, gl_id_ );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind framebuffer "
            + std::to_string( gl_id_ )
            + std::string( gl_id_ == 0 ? " (default)" : "" )
            + " for yavsg::gl::base_framebuffer::bind()"
        );
        
        if( alpha_blending_ == alpha_blend_mode::DISABLED )
        {
            glDisablei( GL_BLEND, 0 );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't disable blending for framebuffer "
                + std::to_string( gl_id_ )
                + std::string( gl_id_ == 0 ? " (default)" : "" )
                + " for yavsg::gl::base_framebuffer::alpha_blending()"
            );
        }
        else
        {
            glEnablei( GL_BLEND, 0 );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't enable blending for framebuffer "
                + std::to_string( gl_id_ )
                + std::string( gl_id_ == 0 ? " (default)" : "" )
                + " for yavsg::gl::base_framebuffer::alpha_blending()"
            );
        }
        
        if( alpha_blending_ != alpha_blend_mode::DISABLED )
        {
            // TODO: glBlendEquationSeparatei( 0, GL_FUNC_ADD, GL_FUNC_ADD );
            glBlendEquationSeparate( GL_FUNC_ADD, GL_FUNC_ADD );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't set separate blending equations for framebuffer "
                + std::to_string( gl_id_ )
                + std::string( gl_id_ == 0 ? " (default)" : "" )
                + " for yavsg::gl::base_framebuffer::alpha_blending()"
            );
            
            // Use a switch to get warnings about possible unhandled cases in
            // the future
            switch( alpha_blending_ )
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
                + std::to_string( gl_id_ )
                + std::string( gl_id_ == 0 ? " (default)" : "" )
                + " for yavsg::gl::base_framebuffer::alpha_blending()"
            );
        }
    }
    
    alpha_blend_mode write_only_framebuffer::alpha_blending(
        alpha_blend_mode mode
    )
    {
        alpha_blending_ = mode;
        return alpha_blending_;
    }
    
    alpha_blend_mode write_only_framebuffer::alpha_blending() const
    {
        return alpha_blending_;
    }
    
    // TODO: make this a debug function
    void write_only_framebuffer::dump_BMP(
        const std::string& descriptive_name
    )
    {
        assert( width_  <= std::numeric_limits< GLsizei >::max() );
        assert( height_ <= std::numeric_limits< GLsizei >::max() );
        auto w = static_cast< GLsizei >( width_  );
        auto h = static_cast< GLsizei >( height_ );
        
        // TODO: make class member & only realloc when buffer size increases
        std::vector< char > pixels( width_ * height_ * 4 );
        
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
