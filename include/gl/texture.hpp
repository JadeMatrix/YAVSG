#pragma once
#ifndef YAVSG_GL_TEXTURE_HPP
#define YAVSG_GL_TEXTURE_HPP


#include "_gl_base.hpp"
#include "../sdl/sdl_utils.hpp"

#include <string>


namespace yavsg { namespace gl
{
    void load_bound_texture( const std::string& filename );
    
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
        
        texture( SDL_Surface* sdl_surface ); // Not `const` due to the SDL API
        texture( texture&& );
        
        // Disable copy & assignment
        texture( const texture& ) = delete;
        texture& operator=( const texture& ) = delete;
        
        static texture from_file( const std::string& filename );
        static texture rgba8_from_bytes(
            unsigned int width,
            unsigned int height,
            const char* bytes
        );
        // For use by stuff like framebuffers that need to control texture
        // allocation
        static texture make_empty();
        
        ~texture();
        
        // TODO: wrap settings
        void filtering( const filter_settings& );
        
        // TODO: Determine if there's a more opaque way to access this
        GLuint gl_texture_id() const;
    };
} }


#endif
