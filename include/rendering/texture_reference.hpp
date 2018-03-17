#pragma once
#ifndef YAVSG_TEXTURE_REFERENCE_HPP
#define YAVSG_TEXTURE_REFERENCE_HPP


#include "../gl/texture.hpp"
#include "../tasking/task.hpp"
#include "../tasking/tasking.hpp"

#include <SDL2/SDL_image.h>

#include <exception>    // std::invalid_argument
#include <mutex>
#include <string>
// std::shared_ptr, std::make_shared(), std::unique_ptr, std::make_unique()
#include <memory>


namespace yavsg
{
    template< typename DataType, std::size_t Channels > class texture_reference
    {
    public:
        using texture_type = gl::texture< DataType, Channels >;
        
        class shared_data
        {
        public:
            std::mutex                      data_mutex;
            // TODO: std::unique_ptr< texture_type > texture;
            texture_type* texture;
            
            shared_data();
            ~shared_data();
        };
        
    protected:
        std::shared_ptr< shared_data > _shared_data;
        
    public:
        texture_reference();
        texture_reference( const texture_reference& );
        
        texture_reference& operator=( texture_reference&& ) = default;
        
        static texture_reference from_file(
            const std::string                & filename,
            const gl::texture_filter_settings& settings,
            gl::texture_flags_type             flags = gl::texture_flag::NONE
        );
        
        // Checks if this refers to a texture ready for rendering
        operator bool() const;
        
        // Access the referenced texture; throws `std::invalid_argument` if this
        // does not refer to a texture ready for rendering.  Note that once 
        // valid, the referenced texture will remain so as long as the reference
        // object exists.
        texture_type* operator ->();
        const texture_type* operator ->() const;
        texture_type& operator *()
        {
            return *( this -> operator->() );
        }
        const texture_type& operator *() const
        {
            return *( this -> operator->() );
        }
    };
}


namespace yavsg // Tasks ///////////////////////////////////////////////////////
{
    template<
        typename DataType,
        std::size_t Channels
    > class load_texture_file_task : public task
    {
    public:
        using shared_data = typename texture_reference<
            DataType,
            Channels
        >::shared_data;
        
    protected:
        std::shared_ptr< shared_data > _shared_data;
        std::string                    _filename;
        gl::texture_filter_settings    _settings;
        gl::texture_flags_type         _flags;
        
    public:
        load_texture_file_task(
            std::shared_ptr< shared_data >     sd,
            const std::string                & filename,
            const gl::texture_filter_settings& settings,
            gl::texture_flags_type             flags
        ) :
            _shared_data{ sd       },
            _filename   { filename },
            _settings   { settings },
            _flags      { flags    }
        {
            if( !_shared_data )
                throw std::invalid_argument(
                    "shared_data to load_texture_file_task is null"
                );
        }
        
        virtual bool operator()();
    };
    
    template<
        typename DataType,
        std::size_t Channels
    > class upload_texture_data_task : public task
    {
    public:
        using shared_data = typename texture_reference<
            DataType,
            Channels
        >::shared_data;
        
    protected:
        std::shared_ptr< shared_data > _shared_data;
        gl::texture_upload_data        _upload_data;
        gl::texture_filter_settings    _settings;
        gl::texture_flags_type         _flags;
        
    public:
        upload_texture_data_task(
            std::shared_ptr< shared_data > sd,
            gl::texture_upload_data        upload_data,
            gl::texture_filter_settings    settings,
            gl::texture_flags_type         flags
        ) :
            _shared_data{ sd                       },
            _upload_data{ std::move( upload_data ) },
            _settings   { settings                 },
            _flags      { flags                    }
        {
            if( !_shared_data )
                throw std::invalid_argument(
                    "shared_data to upload_texture_data_task is null"
                );
        }
        
        virtual task_flags_type flags() const
        {
            return task_flag::GPU_THREAD;
        }
        
        virtual bool operator()();
    };
    
    template<
        typename DataType,
        std::size_t Channels
    > class destroy_texture_data_task : public task
    {
    public:
        using shared_data = typename texture_reference<
            DataType,
            Channels
        >::shared_data;
        
    protected:
        // TODO: std::unique_ptr< gl::texture< DataType, Channels > > _texture;
        gl::texture< DataType, Channels >* _texture;
        
    public:
        destroy_texture_data_task(
            // TODO: std::unique_ptr< gl::texture< DataType, Channels > > texture
            gl::texture< DataType, Channels >* texture
        ) : _texture{ texture }
        {}
        
        virtual task_flags_type flags() const
        {
            return task_flag::GPU_THREAD;
        }
        
        virtual bool operator()();
    };
}


namespace yavsg // Texture reference shared data implementation ////////////////
{
    template< typename DataType, std::size_t Channels >
    texture_reference< DataType, Channels >::shared_data::shared_data() :
        texture( nullptr )
    {}
    
    template< typename DataType, std::size_t Channels >
    texture_reference< DataType, Channels >::shared_data::~shared_data()
    {
        // The shared data being destroyed means that nothing refers to the
        // texture anymore, including any tasks operating on it
        std::lock_guard< std::mutex > lock( data_mutex );
        if( texture )
            submit_task( std::make_unique<
                destroy_texture_data_task< DataType, Channels >
            >( texture ) );
    }
}


namespace yavsg // Texture reference implementation ////////////////////////////
{
    template< typename DataType, std::size_t Channels >
    texture_reference< DataType, Channels >::texture_reference()
    {}
    
    template< typename DataType, std::size_t Channels >
    texture_reference< DataType, Channels >::texture_reference(
        const texture_reference& o
    ) : _shared_data( o._shared_data )
    {}
    
    template< typename DataType, std::size_t Channels >
    texture_reference< DataType, Channels >::operator bool() const
    {
        if( _shared_data )
        {
            std::lock_guard< std::mutex > lock( const_cast< std::mutex& >(
                _shared_data -> data_mutex
            ) );
            return static_cast< bool >( _shared_data -> texture );
        }
        
        return false;
    }
    
    template< typename DataType, std::size_t Channels >
    typename texture_reference<
        DataType,
        Channels
    >::texture_type* texture_reference< DataType, Channels >::operator ->()
    {
        if( _shared_data )
        {
            std::lock_guard< std::mutex > lock( _shared_data -> data_mutex );
            if( _shared_data -> texture )
                return _shared_data -> texture;
        }
        
        throw std::invalid_argument(
            "yavsg::gl::texture_reference does not refer to a texture ready "
            "for rendering"
        );
    }
    
    template< typename DataType, std::size_t Channels >
    const typename texture_reference<
        DataType,
        Channels
    >::texture_type* texture_reference<
        DataType,
        Channels
    >::operator ->() const
    {
        if( _shared_data )
        {
            std::lock_guard< std::mutex > lock( const_cast< std::mutex& >(
                _shared_data -> data_mutex
            ) );
            if( _shared_data -> texture )
                return _shared_data -> texture;
        }
        
        throw std::invalid_argument(
            "yavsg::gl::texture_reference does not refer to a texture ready "
            "for rendering"
        );
    }
}


namespace yavsg // Texture reference static methods implementation /////////////
{
    template< typename DataType, std::size_t Channels >
    texture_reference<
        DataType,
        Channels
    > texture_reference< DataType, Channels >::from_file(
        const std::string                & filename,
        const gl::texture_filter_settings& settings,
        gl::texture_flags_type             flags
    )
    {
        using ref_type = texture_reference< DataType, Channels >;
        
        ref_type ref;
        
        ref._shared_data = std::make_shared< ref_type::shared_data >();
        
        submit_task( std::make_unique<
            load_texture_file_task< DataType, Channels >
        >(
            ref._shared_data,
            filename,
            settings,
            flags
        ) );
        
        return ref;
    }
}


namespace yavsg // Task implementations ////////////////////////////////////////
{
    template<
        typename DataType,
        std::size_t Channels
    > bool load_texture_file_task< DataType, Channels >::operator()()
    {
        SDL_Surface* sdl_surface = IMG_Load(
            _filename.c_str()
        );
        if( !sdl_surface )
            // TODO: graceful failure (just don't load)
            throw std::runtime_error(
                "failed to load texture \""
                + _filename
                + "\": "
                + IMG_GetError()
            );
        
        using format_traits = gl::texture_format_traits< DataType, Channels >;
        
        submit_task( std::make_unique<
            upload_texture_data_task< DataType, Channels >
        >(
            _shared_data,
            gl::process_texture_data(
                sdl_surface,
                _flags,
                format_traits::gl_internal_format
            ),
            _settings,
            _flags
        ) );
        
        return false;
    }
    
    template<
        typename DataType,
        std::size_t Channels
    > bool upload_texture_data_task< DataType, Channels >::operator()()
    {
        std::lock_guard< std::mutex > lock( _shared_data -> data_mutex );
        
        // _shared_data -> texture = std::make_unique<
        //     gl::texture< DataType, Channels >
        // >();
        _shared_data -> texture = new gl::texture< DataType, Channels >();
        
        upload_texture_data(
            _shared_data -> texture -> gl_texture_id(),
            std::move( _upload_data ),
            _settings
        );
        
        return false;
    }
    
    template<
        typename DataType,
        std::size_t Channels
    > bool destroy_texture_data_task< DataType, Channels >::operator()()
    {
        // TODO: _texture = nullptr;
        delete _texture;
        return false;
    }
}


#endif
