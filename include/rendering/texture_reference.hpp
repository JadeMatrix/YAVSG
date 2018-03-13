#pragma once
#ifndef YAVSG_TEXTURE_REFERENCE_HPP
#define YAVSG_TEXTURE_REFERENCE_HPP


#include "../gl/texture.hpp"

#include <string>


namespace yavsg
{
    template<
        typename DataType,
        std::size_t Channels
    > class texture_reference
    {
    public:
        using texture_type = gl::texture< DataType, Channels >;
        
    protected:
        texture_type*               _texture;
        std::string                 _filename;
        gl::texture_filter_settings _settings;
        gl::texture_flags_type      _flags;
        
    public:
        // This is very simple for now
        texture_reference(
            const std::string                & filename,
            const gl::texture_filter_settings& settings,
            gl::texture_flags_type             flags = gl::texture_flags::NONE
        );
        
        ~texture_reference();
        
        // These ensure the texture is created, so not strictly const in all
        // situations
        texture_type& texture();
        const texture_type& texture() const;
    };
}


namespace yavsg // Texture reference implementation ////////////////////////////
{
    template< typename DataType, std::size_t Channels >
    texture_reference< DataType, Channels >::texture_reference(
        const std::string                & filename,
        const gl::texture_filter_settings& settings,
        gl::texture_flags_type             flags
    ) :
        _texture(  nullptr  ),
        _filename( filename ),
        _settings( settings ),
        _flags(    flags    )
    {}
    
    template< typename DataType, std::size_t Channels >
    texture_reference< DataType, Channels >::~texture_reference()
    {
        if( _texture )
            delete _texture;
    }
    
    template< typename DataType, std::size_t Channels >
    typename texture_reference< DataType, Channels >::texture_type&
    texture_reference< DataType, Channels >::texture()
    {
        if( !_texture )
            _texture = new texture_type( texture_type::from_file(
                _filename,
                _settings,
                _flags
            ) );
        return *_texture;
    }
    
    template< typename DataType, std::size_t Channels >
    const typename texture_reference< DataType, Channels >::texture_type&
    texture_reference< DataType, Channels >::texture() const
    {
        return const_cast< texture_reference< DataType, Channels >* >(
            this
        ) -> texture();
    }
}


#endif
