#pragma once
#ifndef YAVSG_GL_TEXTURE_HPP
#define YAVSG_GL_TEXTURE_HPP


#include "_gl_base.hpp"
#include "error.hpp"
#include "../sdl/sdl_utils.hpp"

#include <string>
#include <type_traits>  // std::enable_if
#include <utility>      // std::size_t


namespace yavsg { namespace gl
{
    static const GLuint default_texture_gl_id = 0x00;
    
    class texture
    {
    protected:
        GLuint gl_id = default_texture_gl_id;
        
        texture();
        
    public:
        struct filter_settings
        {
            enum class magnify_mode
            {
                NEAREST, LINEAR
            } magnify;
            enum class minify_mode
            {
                NEAREST, LINEAR
            } minify;
            enum class mipmap_type
            {
                NEAREST, LINEAR, NONE
            } mipmaps;
        };
        
        texture(
            SDL_Surface*, // Not `const` due to the SDL API
            const filter_settings&
        );
        texture( texture&& );
        
        // Disable copy & assignment
        texture( const texture& ) = delete;
        texture& operator=( const texture& ) = delete;
        
        static texture from_file(
            const std::string& filename,
            const filter_settings& settings
        );
        static texture rgba8_from_bytes(
            unsigned int width,
            unsigned int height,
            const char* bytes,
            const filter_settings& settings
        );
        // For use by stuff like framebuffers that need to control texture
        // allocation
        static texture make_empty();
        
        ~texture();
        
        // TODO: wrap settings
        void filtering( const filter_settings& );
        
        template<
            std::size_t ActiveTexture,
            typename std::enable_if<
                ( ActiveTexture < GL_MAX_TEXTURE_UNITS ),
                int
            >::type = 0
        > void bind_as() const
        {
            glActiveTexture( GL_TEXTURE0 + ActiveTexture );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't activate GL_TEXTURE"
                + std::to_string( ActiveTexture )
                + " to bind texture "
                + std::to_string( gl_id )
                + " for yavsg::gl::texture::bind_as<>()"
            );
            
            glBindTexture( GL_TEXTURE_2D, gl_id );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't bind texture "
                + std::to_string( gl_id )
                + " as GL_TEXTURE"
                + std::to_string( ActiveTexture )
                + " for yavsg::gl::texture::bind_as<>()"
            );
        }
        
        // TODO: Determine if there's a more opaque way to access this
        GLuint gl_texture_id();
    };
    
    template<
        std::size_t ActiveTexture,
        typename std::enable_if<
            ( ActiveTexture < GL_MAX_TEXTURE_UNITS ),
            int
        >::type = 0
    > void unbind_texture()
    {
        glActiveTexture( GL_TEXTURE0 + ActiveTexture );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't activate GL_TEXTURE"
            + std::to_string( ActiveTexture )
            + " to bind default texture for yavsg::gl::unbind_texture<>()"
        );
        
        glBindTexture( GL_TEXTURE_2D, 0 );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind default texture as GL_TEXTURE"
            + std::to_string( ActiveTexture )
            + " for yavsg::gl::unbind_texture<>()"
        );
    }
} }


#endif
