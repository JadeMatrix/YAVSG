#pragma once
#ifndef YAVSG_GL_FRAMEBUFFER_HPP
#define YAVSG_GL_FRAMEBUFFER_HPP


#include "error.hpp"
#include "texture.hpp"

#include <array>
#include <cstdint>      // std::int_least16_t, std::int_least32_t
#include <string>
#include <type_traits>  // std::enable_if
#include <utility>      // std::size_t


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
    class base_framebuffer
    {
        // This is so the window can create a base_framebuffer for the default
        // framebuffer
        friend class yavsg::SDL_window_manager;
        
    public:
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
        
    protected:
        std::size_t _width;
        std::size_t _height;
        alpha_blend_mode _alpha_blending;
        
        // `base_framebuffer` does not own the actual OpenGL framebuffer
        GLuint gl_id;
        
        base_framebuffer(
            GLuint,
            std::size_t,
            std::size_t
        );
        
    public:
        std::size_t  width() const;
        std::size_t height() const;
        
        void bind();
        
        alpha_blend_mode alpha_blending( alpha_blend_mode );
        alpha_blend_mode alpha_blending() const;
        
        void dump_BMP( const std::string& descriptive_name );
    };
    
    template< std::size_t ColorTargets > class framebuffer :
        public base_framebuffer
    {
    protected:
        texture< depth_stecil_dummy< GLfloat >, 2 > _depth_stencil_buffer;
        
        // Eh, I'll improve it later
        alignas( alignof( std::array< texture<>, ColorTargets > ) )
        typename std::enable_if<
            ( ColorTargets < GL_MAX_COLOR_ATTACHMENTS - 1 ),
            char
        >::type color_buffers[
            sizeof( std::array< texture<>, ColorTargets > )
        ];
        
        GLuint framebuffer_init();
        texture< depth_stecil_dummy< GLfloat >, 2 > depth_stencil_buffer_init();
        // Recursive initializer to help with cleanup on errors
        void color_buffers_init( std::size_t i = 0 );
        void check_finalized();
        
    public:
        static const std::size_t num_color_targets = ColorTargets;
        
        framebuffer(
            std::size_t width,
            std::size_t height
        );
        ~framebuffer();
        
        texture< depth_stecil_dummy< GLfloat >, 2 >& depth_stencil_buffer();
        template< std::size_t N > texture<>& color_buffer();
    };
} }


namespace yavsg { namespace gl // Framebuffer implementation ///////////////////
{
    template< std::size_t ColorTargets >
    GLuint framebuffer< ColorTargets >::framebuffer_init()
    {
        glGenFramebuffers( 1, &gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't generate framebuffer for "
            "yavsg::gl::framebuffer::framebuffer_init()"
        );
        
        try
        {
            // Calling bind() should be OK as long as framebuffers continue to
            // be initialized with alpha blend mode = DISABLED (so bind() won't
            // set a blending mode during construction)
            bind();
        }
        catch( ... )
        {
            glDeleteFramebuffers( 1, &gl_id );
            throw;
        }
        
        return gl_id;
    }
    
    template< std::size_t ColorTargets >
    texture<
        depth_stecil_dummy< GLfloat >,
        2
    > framebuffer< ColorTargets >::depth_stencil_buffer_init()
    {
        auto t = texture< depth_stecil_dummy< GLfloat >, 2 >(
            _width,
            _height,
            static_cast< std::array<
                texture< depth_stecil_dummy< GLfloat >, 2 >::sample_type,
                texture< depth_stecil_dummy< GLfloat >, 2 >::channels
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
            + std::to_string( gl_id )
            + " for yavsg::gl::framebuffer::depth_buffer_init()"
        );
        
        return t;
    }
    
    template< std::size_t ColorTargets >
    void framebuffer< ColorTargets >::color_buffers_init( std::size_t i )
    {
        if( i < ColorTargets )
        {
            auto color_buffer_array = reinterpret_cast<
                std::array< texture<>, ColorTargets >*
            >( color_buffers );
            
            auto texture_ptr = &( *color_buffer_array )[ i ];
            
            new( texture_ptr ) texture<>(
                _width,
                _height,
                static_cast< std::array<
                    texture<>::sample_type,
                    texture<>::channels
                >* >( nullptr ),
                {
                    texture_filter_settings::magnify_mode::LINEAR,
                    texture_filter_settings::minify_mode::LINEAR,
                    texture_filter_settings::mipmap_type::NONE,
                },
                texture_flags::ALLOCATE_ONLY
            );
            
            try
            {
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0 + i,
                    GL_TEXTURE_2D,
                    texture_ptr -> gl_texture_id(),
                    0   // Mipmap level (unused for now)
                );
                YAVSG_GL_THROW_FOR_ERRORS(
                    "couldn't attach texture "
                    + std::to_string( texture_ptr -> gl_texture_id() )
                    + " as color buffer "
                    + std::to_string( i )
                    + " to framebuffer "
                    + std::to_string( gl_id )
                    + " for yavsg::gl::framebuffer::framebuffer_init()"
                );
                
                color_buffers_init( i + 1 );
            }
            // TODO: one this isn't recursive, add on thsi " for ..."
            // catch( const summary_error& e )
            // {
            //     texture_ptr -> ~texture<>();
            //     throw summary_error(
            //         e.what() + std::string(
            //             " for yavsg::gl::framebuffer::framebuffer_init()"
            //         ),
            //         e.error_codes
            //     );
            // }
            catch( ... )
            {
                texture_ptr -> ~texture<>();
                throw;
            }
        }
        else
            check_finalized();
    }
    
    template< std::size_t ColorTargets >
    void framebuffer< ColorTargets >::check_finalized()
    {
        if(
            glCheckFramebufferStatus( GL_FRAMEBUFFER )
            != GL_FRAMEBUFFER_COMPLETE
        )
            throw std::runtime_error(
                "failed to complete famebuffer "
                + std::to_string( gl_id )
                + " for yavsg::gl::framebuffer"
            );
    }
    
    template< std::size_t ColorTargets >
    framebuffer< ColorTargets >::framebuffer(
        std::size_t width,
        std::size_t height
    ) :
        base_framebuffer(
            framebuffer_init(),
            width,
            height
        ),
        _depth_stencil_buffer( depth_stencil_buffer_init() )
    {
        color_buffers_init();
    }
    
    template< std::size_t ColorTargets >
    framebuffer< ColorTargets >::~framebuffer()
    {
        auto color_buffer_array = reinterpret_cast<
            std::array< texture<>, ColorTargets >*
        >( color_buffers );
        
        for( auto& t : *color_buffer_array )
            t.~texture<>();
        
        glDeleteFramebuffers( 1, &gl_id );
    }
    
    template< std::size_t ColorTargets >
    texture<
        depth_stecil_dummy< GLfloat >,
        2
    >& framebuffer< ColorTargets >::depth_stencil_buffer()
    {
        return _depth_stencil_buffer;
    }
    
    template< std::size_t ColorTargets >
    template< std::size_t N >
    texture<>& framebuffer< ColorTargets >::color_buffer()
    {
        auto color_buffer_array = reinterpret_cast<
            std::array< texture<>, ColorTargets >*
        >( color_buffers );
        return std::get< N >( *color_buffer_array );
    }
} }


#endif
