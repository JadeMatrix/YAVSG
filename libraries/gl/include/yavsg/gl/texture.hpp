#pragma once


#include <yavsg/gl/error.hpp>
#include "texture_utilities.hpp"

#include <array>
#include <cstddef>      // byte
#include <string>
#include <type_traits>  // enable_if


namespace JadeMatrix::yavsg::gl
{
    template<
        typename    DataType,
        std::size_t Channels
    > class texture
    {
        // friend class        texture_reference< DataType, Channels >;
        friend class yavsg::upload_texture_data_task< DataType, Channels >;
        
    public:
        using format_traits = texture_format_traits< DataType, Channels >;
        
        using                 sample_type = DataType;
        static constexpr auto channels    = Channels;
        
        texture(
            std::size_t                    width,
            std::size_t                    height,
            std::unique_ptr< std::byte[] > data,
            texture_filter_settings const& settings,
            texture_flags_type             flags
        );
        texture( texture&& );
        
        ~texture();
        
        // Disable copy & assignment
        texture( texture const& ) = delete;
        texture& operator=( texture const& ) = delete;
        
        // ... but enable move assignment
        texture& operator=( texture&& ) = default;
        
        // TODO: Determine if there's a more opaque way to access this
        GLuint gl_texture_id() const;
        
        // TODO: wrap settings
        // void wrapping( ... );
        void filtering( texture_filter_settings const& );
        
        template<
            std::size_t ActiveTexture,
            typename    = std::enable_if_t<
                ( ActiveTexture < GL_MAX_TEXTURE_UNITS )
            >
        > void bind_as() const
        {
            using namespace std::string_literals;
            
            gl::ActiveTexture( GL_TEXTURE0 + ActiveTexture );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't activate GL_TEXTURE"s
                + std::to_string( ActiveTexture )
                + " to bind texture "s
                + std::to_string( gl_id_ )
                + " for yavsg::gl::texture::bind_as<>()"s
            );
            
            gl::BindTexture( GL_TEXTURE_2D, gl_id_ );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't bind texture "s
                + std::to_string( gl_id_ )
                + " as GL_TEXTURE"s
                + std::to_string( ActiveTexture )
                + " for yavsg::gl::texture::bind_as<>()"s
            );
        }
        
    protected:
        GLuint gl_id_ = default_texture_gl_id;
        
        texture();
    };
    
    template<
        std::size_t ActiveTexture,
        typename    = std::enable_if_t<
            ( ActiveTexture < GL_MAX_TEXTURE_UNITS )
        >
    > void unbind_texture()
    {
        using namespace std::string_literals;
        
        gl::ActiveTexture( GL_TEXTURE0 + ActiveTexture );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't activate GL_TEXTURE"s
            + std::to_string( ActiveTexture )
            + " to bind default texture for yavsg::gl::unbind_texture()"s
        );
        
        gl::BindTexture( GL_TEXTURE_2D, 0 );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't bind default texture as GL_TEXTURE"s
            + std::to_string( ActiveTexture )
            + " for yavsg::gl::unbind_texture()"s
        );
    }
}


// Texture class implementation ////////////////////////////////////////////////

template< typename DataType, std::size_t Channels >
JadeMatrix::yavsg::gl::texture< DataType, Channels >::texture()
{
    using namespace std::string_literals;
    
    gl::GenTextures( 1, &gl_id_ );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't generate texture for yavsg::gl::texture"s
    );
}

template< typename DataType, std::size_t Channels >
JadeMatrix::yavsg::gl::texture< DataType, Channels >::~texture()
{
    if( gl_id_ != default_texture_gl_id )
    {
        gl::DeleteTextures( 1, &gl_id_ );
    }
}

// template< typename DataType, std::size_t Channels >
// JadeMatrix::yavsg::gl::texture< DataType, Channels >::texture(
//     texture_upload_data data
// ) : texture()
// {
//     upload_texture_data(
//         gl_id_,
//         data
//     );
// }
template< typename DataType, std::size_t Channels >
JadeMatrix::yavsg::gl::texture< DataType, Channels >::texture(
    std::size_t                    width,
    std::size_t                    height,
    std::unique_ptr< std::byte[] > data,
    texture_filter_settings const& settings,
    texture_flags_type             flags
) : texture()
{
    upload_texture_data(
        gl_id_,
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
JadeMatrix::yavsg::gl::texture< DataType, Channels >::texture(
    texture< DataType, Channels >&& o
)
{
    std::swap( gl_id_, o.gl_id_ );
}

template< typename DataType, std::size_t Channels >
GLuint JadeMatrix::yavsg::gl::texture< DataType, Channels >::gl_texture_id() const
{
    return gl_id_;
}

template< typename DataType, std::size_t Channels >
void JadeMatrix::yavsg::gl::texture< DataType, Channels >::filtering(
    texture_filter_settings const& settings
)
{
    using namespace std::string_literals;
    
    gl::BindTexture( GL_TEXTURE_2D, gl_id_ );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't bind texture "s
        + std::to_string( gl_id_ )
        + " for yavsg::gl::texture::filtering()"s
    );
    
    set_bound_texture_filtering( settings );
}
