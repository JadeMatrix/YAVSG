#pragma once


#include "error.hpp"
#include "texture_utilities.hpp"

#include <array>
#include <string>
#include <type_traits>  // std::enable_if


namespace JadeMatrix::yavsg::gl
{
    template<
        typename DataType,
        std::size_t Channels
    > class texture
    {
        // friend class        texture_reference< DataType, Channels >;
        friend class yavsg::upload_texture_data_task< DataType, Channels >;
        
    protected:
        GLuint gl_id = default_texture_gl_id;
        
        texture();
        
    public:
        using format_traits = texture_format_traits< DataType, Channels >;
        
        using             sample_type = DataType;
        static const auto channels    = Channels;
        
        texture(
            std::size_t                    width,
            std::size_t                    height,
            std::unique_ptr< char[] >      data,
            const texture_filter_settings& settings,
            texture_flags_type             flags
        );
        texture( texture&& );
        
        ~texture();
        
        // Disable copy & assignment
        texture( const texture& ) = delete;
        texture& operator=( const texture& ) = delete;
        
        // ... but enable move assignment
        texture& operator=( texture&& ) = default;
        
        // TODO: Determine if there's a more opaque way to access this
        GLuint gl_texture_id();
        
        // TODO: wrap settings
        // void wrapping( ... );
        void filtering( const texture_filter_settings& );
        
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
            + " to bind default texture for yavsg::gl::unbind_texture()"
        );
        
        glBindTexture( GL_TEXTURE_2D, 0 );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind default texture as GL_TEXTURE"
            + std::to_string( ActiveTexture )
            + " for yavsg::gl::unbind_texture()"
        );
    }
}


namespace JadeMatrix::yavsg::gl // Texture class implementation ////////////////
{
    template< typename DataType, std::size_t Channels >
    texture< DataType, Channels >::texture()
    {
        glGenTextures( 1, &gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't generate texture for yavsg::gl::texture"
        );
    }
    
    template< typename DataType, std::size_t Channels >
    texture< DataType, Channels >::~texture()
    {
        if( gl_id != default_texture_gl_id )
            glDeleteTextures( 1, &gl_id );
    }
    
    // template< typename DataType, std::size_t Channels >
    // texture< DataType, Channels >::texture( texture_upload_data data ) :
    //     texture()
    // {
    //     upload_texture_data(
    //         gl_id,
    //         data
    //     );
    // }
    template< typename DataType, std::size_t Channels >
    texture< DataType, Channels >::texture(
        std::size_t                    width,
        std::size_t                    height,
        std::unique_ptr< char[] >      data,
        const texture_filter_settings& settings,
        texture_flags_type             flags
    ) : texture()
    {
        upload_texture_data(
            gl_id,
            process_texture_data(
                texture_upload_data{
                    format_traits::gl_internal_format,
                    width,
                    height,
                    format_traits::gl_incoming_format,
                    format_traits::gl_incoming_type,
                    std::move( data )
                },
                flags
            ),
            settings
        );
    }
    
    template< typename DataType, std::size_t Channels >
    texture< DataType, Channels >::texture( texture< DataType, Channels >&& o )
    {
        std::swap( gl_id, o.gl_id );
    }
    
    template< typename DataType, std::size_t Channels >
    GLuint texture< DataType, Channels >::gl_texture_id()
    {
        return gl_id;
    }
    
    template< typename DataType, std::size_t Channels >
    void texture< DataType, Channels >::filtering(
        const texture_filter_settings& settings
    )
    {
        glBindTexture( GL_TEXTURE_2D, gl_id );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind texture "
            + std::to_string( gl_id )
            + " for yavsg::gl::texture::filtering()"
        );
        
        set_bound_texture_filtering( settings );
    }
}
