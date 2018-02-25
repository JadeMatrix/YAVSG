#pragma once
#ifndef YAVSG_GL_FRAMEBUFFER_HPP
#define YAVSG_GL_FRAMEBUFFER_HPP


#include "error.hpp"
#include "texture.hpp"

#include <array>
#include <utility>      // std::size_t
#include <type_traits>  // std::enable_if


namespace yavsg { namespace gl
{
    template< std::size_t ColorTargets > class framebuffer
    {
    protected:
        std::size_t width;
        std::size_t height;
        
        GLuint gl_id;
        texture _depth_stencil_buffer;
        
        // Eh, I'll improve it later
        typename std::enable_if<
            ( ColorTargets < GL_MAX_COLOR_ATTACHMENTS - 1 ),
            char
        >::type color_buffers[
            sizeof( texture ) * ColorTargets
        ];
        
        GLuint             framebuffer_init();
        texture   depth_stencil_buffer_init();
        // Recursive initializer to help with cleanup on errors
        void             color_buffers_init( std::size_t i = 0 );
        void check_finalized();
        
    public:
        framebuffer(
            std::size_t width,
            std::size_t height
        );
        ~framebuffer();
        
        void bind();
        
        texture&   depth_stencil_buffer();
        // texture& stencil_buffer();
        template< std::size_t N > texture& color_buffer();
    };
    
    void bind_default_framebuffer();
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
        bind();
        return gl_id;
    }
    
    template< std::size_t ColorTargets >
    texture framebuffer< ColorTargets >::depth_stencil_buffer_init()
    {
        // glGenRenderbuffers( 1, &id );
        // glBindRenderbuffer( GL_RENDERBUFFER, id );
        // glRenderbufferStorage(
        //     GL_RENDERBUFFER,
        //     GL_DEPTH24_STENCIL8,
        //     width, height
        // );
        // glFramebufferRenderbuffer(
        //     GL_FRAMEBUFFER,
        //     GL_DEPTH_STENCIL_ATTACHMENT,
        //     GL_RENDERBUFFER,
        //     id
        // );
        
        auto t = texture::make_empty();
        auto t_id = t.gl_texture_id();
        
        glBindTexture( GL_TEXTURE_2D, t_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind depth buffer texture "
            + std::to_string( t_id )
            + " for framebuffer "
            + std::to_string( gl_id )
            + " for yavsg::gl::framebuffer::depth_buffer_init()"
        );
        
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_DEPTH24_STENCIL8,
            width, height,
            0,
            GL_DEPTH_STENCIL,
            GL_UNSIGNED_INT_24_8,
            nullptr
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't allocate "
            + std::to_string( width )
            + "x"
            + std::to_string( height )
            + " depth buffer texture "
            + std::to_string( t_id )
            + " for framebuffer "
            + std::to_string( gl_id )
            + " for yavsg::gl::framebuffer::depth_buffer_init()"
        );
        
        t.filtering( {
            texture::filter_settings::magnify_mode::LINEAR,
            texture::filter_settings::minify_mode::LINEAR,
            texture::filter_settings::mipmap_type::NONE,
        } );
        
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_TEXTURE_2D,
            t_id,
            0   // Mipmap level (not useful)
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't attach depth buffer texture "
            + std::to_string( t_id )
            + " for framebuffer "
            + std::to_string( gl_id )
            + " for yavsg::gl::framebuffer::depth_buffer_init()"
        );
        
        return t;
    }
    
    template< std::size_t ColorTargets >
    void framebuffer< ColorTargets >::color_buffers_init( std::size_t i )
    {
        if( i < ColorTargets  )
        {
            auto& color_buffer_array = *( std::array<
                texture,
                ColorTargets
            >* )color_buffers;
            
            new( &color_buffer_array[ i ] ) texture(
                texture::make_empty()
            );
            
            try
            {
                auto texture_id = color_buffer_array[ i ].gl_texture_id();
                
                glBindTexture( GL_TEXTURE_2D, texture_id );
                YAVSG_GL_THROW_FOR_ERRORS(
                    "couldn't bind texture "
                    + std::to_string( texture_id )
                    + " for color buffer "
                    + std::to_string( i )
                    + " for framebuffer "
                    + std::to_string( gl_id )
                    + " for yavsg::gl::framebuffer::color_buffers_init()"
                );
                
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RGB,
                    width, height,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    nullptr
                );
                YAVSG_GL_THROW_FOR_ERRORS(
                    "couldn't allocate "
                    + std::to_string( width )
                    + "x"
                    + std::to_string( height )
                    + " texture "
                    + std::to_string( texture_id )
                    + " for color buffer "
                    + std::to_string( i )
                    + " for framebuffer "
                    + std::to_string( gl_id )
                    + " for yavsg::gl::framebuffer::color_buffers_init()"
                );
                
                color_buffer_array[ i ].filtering( {
                    texture::filter_settings::magnify_mode::LINEAR,
                    texture::filter_settings::minify_mode::LINEAR,
                    texture::filter_settings::mipmap_type::NONE,
                } );
                
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0 + i,
                    GL_TEXTURE_2D,
                    texture_id,
                    0   // Mipmap level (not useful)
                );
                
                color_buffers_init( i + 1 );
            }
            catch( ... )
            {
                color_buffer_array[ i ].~texture();
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
        width( width ),
        height( height ),
        gl_id( framebuffer_init() ),
        _depth_stencil_buffer( depth_stencil_buffer_init() )
    {
        color_buffers_init();
    }
    
    template< std::size_t ColorTargets >
    framebuffer< ColorTargets >::~framebuffer()
    {
        auto& color_buffer_array = *( std::array< texture, ColorTargets >* )color_buffers;
        
        for( auto& t : color_buffer_array )
            t.~texture();
        
        glDeleteFramebuffers( 1, &gl_id );
        
        // glDeleteRenderbuffers( 1, &depth_stencil_buffer );
    }
    
    template< std::size_t ColorTargets >
    void framebuffer< ColorTargets >::bind()
    {
        glBindFramebuffer( GL_FRAMEBUFFER, gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind framebuffer "
            + std::to_string( gl_id )
            + " for yavsg::gl::framebuffer::bind()"
        );
    }
    
    template< std::size_t ColorTargets >
    texture& framebuffer< ColorTargets >::depth_stencil_buffer()
    {
        return _depth_stencil_buffer;
    }
    
    template< std::size_t ColorTargets >
    template< std::size_t N >
    texture& framebuffer< ColorTargets >::color_buffer()
    {
        auto& color_buffer_array = *( std::array< texture, ColorTargets >* )color_buffers;
        return std::get< N >( color_buffer_array );
    }
} }


#endif
