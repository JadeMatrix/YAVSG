#pragma once
#ifndef YAVSG_GL_FRAMEBUFFER_HPP
#define YAVSG_GL_FRAMEBUFFER_HPP


#include "error.hpp"
#include "texture.hpp"

#include <array>
#include <cstdint>      // std::int_least16_t, std::int_least32_t
#include <exception>
#include <string>
#include <type_traits>  // std::enable_if
#include <tuple>
#include <utility>      // std::size_t

// For yavsg::gl::write_only_framebuffer::dump_BMP()
#include "../sdl/_sdl_base.hpp"
#include <fstream>
#include <iomanip>  // std::setw(), std::setfill()
#include <limits>   // std::numeric_limits
#include <sstream>


namespace yavsg
{
    class SDL_window_manager;
}


namespace yavsg { namespace gl // Depth/stencil texture attributes /////////////
{
    template< typename DepthType > struct depth_stecil_dummy
    {
        using sample_type = DepthType;
    };
    
    // There's no `int24`, so just use `int_least32_t`; also, just use
    // `GL_UNSIGNED_INT_24_8` as the incoming type, as (for now) depth/stencil
    // buffers will only be created with `texture_flags::ALLOCATE_ONLY`
    
    template<> struct texture_format_traits< depth_stecil_dummy<
        std::int_least16_t
    >, 1 >
    {
        static const GLint  gl_internal_format = GL_DEPTH_COMPONENT16;
        static const GLenum gl_format          = GL_DEPTH_COMPONENT;
        static const GLenum gl_type            = GL_UNSIGNED_INT_24_8;
    };
    
    template<> struct texture_format_traits< depth_stecil_dummy<
        std::int_least32_t
    >, 1 >
    {
        static const GLint  gl_internal_format = GL_DEPTH_COMPONENT24;
        static const GLenum gl_format          = GL_DEPTH_COMPONENT;
        static const GLenum gl_type            = GL_UNSIGNED_INT_24_8;
    };
    
    template<> struct texture_format_traits< depth_stecil_dummy< GLfloat >, 1 >
    {
        static const GLint  gl_internal_format = GL_DEPTH_COMPONENT32F;
        static const GLenum gl_format          = GL_DEPTH_COMPONENT;
        static const GLenum gl_type            = GL_UNSIGNED_INT_24_8;
    };
    
    template<> struct texture_format_traits< depth_stecil_dummy<
        std::int_least32_t
    >, 2 >
    {
        static const GLint  gl_internal_format = GL_DEPTH24_STENCIL8;
        static const GLenum gl_format          = GL_DEPTH_STENCIL;
        static const GLenum gl_type            = GL_UNSIGNED_INT_24_8;
    };
    
    template<> struct texture_format_traits< depth_stecil_dummy< GLfloat >, 2 >
    {
        static const GLint  gl_internal_format = GL_DEPTH32F_STENCIL8;
        static const GLenum gl_format          = GL_DEPTH_STENCIL;
        static const GLenum gl_type            = GL_UNSIGNED_INT_24_8;
    };
} }


namespace yavsg { namespace gl // Framebuffer classes //////////////////////////
{
    enum class alpha_blend_mode
    {
        DISABLED,
        PREMULTIPLIED,
        PREMULTIPLIED_DROP_ALPHA,
        // Premultiplied alpha blending should generally be preferred over
        // these two if alpha blending is enabled
        BASIC,       // Source & target alpha blended
        PASSTHROUGH, // Source alpha written
        DROP_ALPHA   // Target alpha kept
    };
    
    template< class... ColorTargetTypes > class write_only_framebuffer
    {
        // This is so the window can create a base_framebuffer for the default
        // framebuffer
        friend class yavsg::SDL_window_manager;
        
    protected:
        std::size_t _width;
        std::size_t _height;
        alpha_blend_mode _alpha_blending;
        GLuint gl_id;
        
        write_only_framebuffer(
            GLuint,
            std::size_t,
            std::size_t
        );
        
    public:
        static const std::size_t num_color_targets
            = sizeof...( ColorTargetTypes );
        
        std::size_t  width() const;
        std::size_t height() const;
        
        void bind();
        
        alpha_blend_mode alpha_blending( alpha_blend_mode );
        alpha_blend_mode alpha_blending() const;
        
        void dump_BMP( const std::string& descriptive_name );
    };
    
    template< class... ColorTargetTypes > class framebuffer :
        public write_only_framebuffer< ColorTargetTypes... >
    {
    public:
        static const std::size_t num_color_targets
            = sizeof...( ColorTargetTypes );
        
    protected:
        using color_buffers_type = std::tuple< ColorTargetTypes... >;
        using depth_stencil_type = texture<
            depth_stecil_dummy< GLfloat >,
            2
        >;
        
        depth_stencil_type _depth_stencil_buffer;
        color_buffers_type _color_buffers;
        
        GLuint framebuffer_init();
        depth_stencil_type depth_stencil_buffer_init();
        color_buffers_type color_buffers_init();
        void check_finalized();
        
    public:
        framebuffer(
            std::size_t width,
            std::size_t height
        );
        ~framebuffer();
        
        void set_size( std::size_t width, std::size_t height );
        
        depth_stencil_type& depth_stencil_buffer();
        
        template< std::size_t Nth > typename std::tuple_element<
            Nth,
            color_buffers_type
        >::type& color_buffer();
    };
} }


namespace yavsg { namespace gl // Write-only framebuffer implementation ////////
{
    template< class... ColorTargetTypes >
    write_only_framebuffer< ColorTargetTypes... >::write_only_framebuffer(
        GLuint gl_id,
        std::size_t width,
        std::size_t height
    ) :
        gl_id(   gl_id  ),
        _width(  width  ),
        _height( height )
    {
        alpha_blending( alpha_blend_mode::DISABLED );
    }
    
    template< class... ColorTargetTypes >
    std::size_t write_only_framebuffer< ColorTargetTypes... >::width() const
    {
        return _width;
    }
    
    template< class... ColorTargetTypes >
    std::size_t write_only_framebuffer< ColorTargetTypes... >::height() const
    {
        return _height;
    }
    
    template< class... ColorTargetTypes >
    void write_only_framebuffer< ColorTargetTypes... >::bind()
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
    
    template< class... ColorTargetTypes >
    alpha_blend_mode write_only_framebuffer<
        ColorTargetTypes...
    >::alpha_blending( alpha_blend_mode mode )
    {
        _alpha_blending = mode;
        return _alpha_blending;
    }
    
    template< class... ColorTargetTypes >
    alpha_blend_mode write_only_framebuffer<
        ColorTargetTypes...
    >::alpha_blending() const
    {
        return _alpha_blending;
    }
    
    template< class... ColorTargetTypes >
    void write_only_framebuffer< ColorTargetTypes... >::dump_BMP(
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


namespace yavsg { namespace gl // Framebuffer color target init ////////////////
{
    template<
        class FirstColorTargetType,
        class... ColorTargetTypes
    > struct framebuffer_color_target_initializer
    {
        static std::tuple< FirstColorTargetType, ColorTargetTypes... > init(
            std::size_t width,
            std::size_t height,
            GLuint gl_framebuffer_id,
            std::size_t nth = 0
        )
        {
            std::tuple< FirstColorTargetType > first_target( { FirstColorTargetType{
                width,
                height,
                static_cast< std::array<
                    typename FirstColorTargetType::sample_type,
                    FirstColorTargetType::channels
                >* >( nullptr ),
                {
                    texture_filter_settings::magnify_mode::LINEAR,
                    texture_filter_settings::minify_mode::LINEAR,
                    texture_filter_settings::mipmap_type::NONE,
                },
                texture_flags::ALLOCATE_ONLY
            } } );
            
            FirstColorTargetType& t = std::get< 0 >( first_target );
            
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + nth,
                GL_TEXTURE_2D,
                t.gl_texture_id(),
                0   // Mipmap level (unused for now)
            );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't attach texture "
                + std::to_string( t.gl_texture_id() )
                + " as color buffer "
                + std::to_string( nth )
                + " to framebuffer "
                + std::to_string( gl_framebuffer_id )
                + " for yavsg::gl::framebuffer_color_target_initializer::init()"
            );
            
            return std::tuple_cat(
                first_target,
                framebuffer_color_target_initializer<
                    ColorTargetTypes...
                >::init( width, height, nth + 1 )
            );
        }
    };
    
    template< class LastColorTargetType >
    struct framebuffer_color_target_initializer< LastColorTargetType >
    {
        static std::tuple< LastColorTargetType > init(
            std::size_t width,
            std::size_t height,
            GLuint gl_framebuffer_id,
            std::size_t nth = 0
        )
        {
            std::tuple< LastColorTargetType > last_target( { LastColorTargetType{
                width,
                height,
                static_cast< std::array<
                    typename LastColorTargetType::sample_type,
                    LastColorTargetType::channels
                >* >( nullptr ),
                {
                    texture_filter_settings::magnify_mode::LINEAR,
                    texture_filter_settings::minify_mode::LINEAR,
                    texture_filter_settings::mipmap_type::NONE,
                },
                texture_flags::ALLOCATE_ONLY
            } } );
            
            LastColorTargetType& t = std::get< 0 >( last_target );
            
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + nth,
                GL_TEXTURE_2D,
                t.gl_texture_id(),
                0   // Mipmap level (unused for now)
            );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't attach texture "
                + std::to_string( t.gl_texture_id() )
                + " as color buffer "
                + std::to_string( nth )
                + " to framebuffer "
                + std::to_string( gl_framebuffer_id )
                + " for yavsg::gl::framebuffer_color_target_initializer::init()"
            );
            
            return last_target;
        }
    };
} }


namespace yavsg { namespace gl // Framebuffer implementation ///////////////////
{
    template< class... ColorTargetTypes >
    GLuint framebuffer< ColorTargetTypes... >::framebuffer_init()
    {
        // Even though this returns the framebuffer ID, we still need to set it
        // before `framebuffer_init()` returns so we can call the `bind()`
        // below
        glGenFramebuffers( 1, &( this -> gl_id ) );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't generate framebuffer for "
            "yavsg::gl::framebuffer::framebuffer_init()"
        );
        
        try
        {
            // Calling bind() should be OK as long as framebuffers continue to
            // be initialized with alpha blend mode = DISABLED (so bind() won't
            // set a blending mode during construction)
            this -> bind();
            
            return this -> gl_id;
        }
        catch( const summary_error& e )
        {
            glDeleteFramebuffers( 1, &( this -> gl_id ) );
            throw summary_error(
                e.what() + std::string(
                    " for yavsg::gl::framebuffer"
                ),
                e.error_codes
            );
        }
        catch( ... )
        {
            glDeleteFramebuffers( 1, &( this -> gl_id ) );
            throw;
        }
    }
    
    template< class... ColorTargetTypes >
    texture<
        depth_stecil_dummy< GLfloat >,
        2
    > framebuffer< ColorTargetTypes... >::depth_stencil_buffer_init()
    {
        auto t = depth_stencil_type(
            this -> _width,
            this -> _height,
            static_cast< std::array<
                depth_stencil_type::sample_type,
                depth_stencil_type::channels
            >* >( nullptr ),
            {
                texture_filter_settings::magnify_mode::LINEAR,
                texture_filter_settings::minify_mode::LINEAR,
                texture_filter_settings::mipmap_type::NONE,
            },
            texture_flags::ALLOCATE_ONLY
        );
        
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_TEXTURE_2D,
            t.gl_texture_id(),
            0   // Mipmap level (not useful)
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't attach depth buffer texture "
            + std::to_string( t.gl_texture_id() )
            + " to framebuffer "
            + std::to_string( this -> gl_id )
            + " for yavsg::gl::framebuffer::depth_buffer_init()"
        );
        
        return t;
    }
    
    template< class... ColorTargetTypes >
    typename framebuffer< ColorTargetTypes... >::color_buffers_type framebuffer<
        ColorTargetTypes...
    >::color_buffers_init()
    {
        try
        {
            return framebuffer_color_target_initializer<
                ColorTargetTypes...
            >::init(
                this -> _width,
                this -> _height,
                this -> gl_id
            );
        }
        catch( const summary_error& e )
        {
            throw summary_error(
                e.what() + std::string(
                    " for yavsg::gl::framebuffer::framebuffer_init()"
                ),
                e.error_codes
            );
        }
    }
    
    template< class... ColorTargetTypes >
    void framebuffer< ColorTargetTypes... >::check_finalized()
    {
        if(
            glCheckFramebufferStatus( GL_FRAMEBUFFER )
            != GL_FRAMEBUFFER_COMPLETE
        )
            throw std::runtime_error(
                "failed to complete famebuffer "
                + std::to_string( this -> gl_id )
                + " for yavsg::gl::framebuffer"
            );
    }
    
    template< class... ColorTargetTypes >
    framebuffer< ColorTargetTypes... >::framebuffer(
        std::size_t width,
        std::size_t height
    ) :
        write_only_framebuffer< ColorTargetTypes... >(
            framebuffer_init(),
            width,
            height
        ),
        _depth_stencil_buffer( depth_stencil_buffer_init() ),
        _color_buffers( color_buffers_init() )
    {
        check_finalized();
    }
    
    template< class... ColorTargetTypes >
    framebuffer< ColorTargetTypes... >::~framebuffer()
    {
        glDeleteFramebuffers( 1, &( this -> gl_id ) );
    }
    
    template< class... ColorTargetTypes >
    void framebuffer< ColorTargetTypes... >::set_size(
        std::size_t width,
        std::size_t height
    )
    {
        this -> _width  = width;
        this -> _height = height;
        
        try
        {
            _color_buffers = framebuffer_color_target_initializer<
                ColorTargetTypes...
            >::init(
                this -> _width,
                this -> _height,
                this -> gl_id
            );
        }
        catch( const summary_error& e )
        {
            throw summary_error(
                e.what() + std::string(
                    " for yavsg::gl::framebuffer::set_size()"
                ),
                e.error_codes
            );
        }
    }
    
    template< class... ColorTargetTypes >
    typename framebuffer<
        ColorTargetTypes...
    >::depth_stencil_type& framebuffer<
        ColorTargetTypes...
    >::depth_stencil_buffer()
    {
        return _depth_stencil_buffer;
    }
    
    template< class... ColorTargetTypes >
    template< std::size_t Nth >
    typename std::tuple_element<
        Nth,
        typename framebuffer< ColorTargetTypes... >::color_buffers_type
    >::type& framebuffer< ColorTargetTypes... >::color_buffer()
    {
        return std::get< Nth >( _color_buffers );
    }
} }


#endif
