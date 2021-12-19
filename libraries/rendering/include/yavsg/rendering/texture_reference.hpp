#pragma once


#include <yavsg/gl/texture.hpp>
#include <yavsg/tasking/task.hpp>
#include <yavsg/tasking/tasking.hpp>

// TODO: Don't have this in the header
#include <SDL2/SDL_image.h>

#include <cstddef>      // size_t
#include <exception>    // invalid_argument, runtime_error
#include <filesystem>
#include <memory>       // shared_ptr, make_shared, unique_ptr, make_unique
#include <mutex>
#include <string>
#include <utility>      // move


namespace JadeMatrix::yavsg
{
    template< typename DataType, std::size_t Channels > class texture_reference
    {
    public:
        using texture_type = gl::texture< DataType, Channels >;
        
        class shared_data
        {
        public:
            std::mutex mutable data_mutex;
            // TODO: std::unique_ptr< texture_type > texture;
            texture_type* texture;
            
            shared_data();
            ~shared_data();
        };
        
        texture_reference();
        texture_reference( texture_reference const& );
        
        texture_reference& operator=( texture_reference&& ) = default;
        
        static texture_reference from_file(
            std::filesystem::path              filename,
            gl::texture_filter_settings const& settings,
            gl::texture_flags_type             flags = gl::texture_flag::none
        );
        
        // Checks if this refers to a texture ready for rendering
        operator bool() const;
        
        // Access the referenced texture; throws `std::invalid_argument` if this
        // does not refer to a texture ready for rendering.  Note that once 
        // valid, the referenced texture will remain so as long as the reference
        // object exists.
        texture_type* operator ->();
        texture_type const* operator ->() const;
        texture_type& operator *()
        {
            return *( this->operator->() );
        }
        texture_type const& operator *() const
        {
            return *( this->operator->() );
        }
        
    protected:
        std::shared_ptr< shared_data > shared_data_;
    };
}


namespace JadeMatrix::yavsg // Tasks ///////////////////////////////////////////
{
    template<
        typename    DataType,
        std::size_t Channels
    > class load_texture_file_task : public task
    {
    public:
        using shared_data = typename texture_reference<
            DataType,
            Channels
        >::shared_data;
        
    protected:
        std::shared_ptr< shared_data > shared_data_;
        std::filesystem::path          filename_;
        gl::texture_filter_settings    settings_;
        gl::texture_flags_type         flags_;
        
    public:
        load_texture_file_task(
            std::shared_ptr< shared_data >     sd,
            std::filesystem::path              filename,
            const gl::texture_filter_settings& settings,
            gl::texture_flags_type             flags
        ) :
            shared_data_{ sd                    },
            filename_   { std::move( filename ) },
            settings_   { settings              },
            flags_      { flags                 }
        {
            using namespace std::string_literals;
            if( !shared_data_ )
            {
                throw std::invalid_argument(
                    "shared_data to load_texture_file_task is null"s
                );
            }
        }
        
        bool operator()() override;
    };
    
    template<
        typename    DataType,
        std::size_t Channels
    > class upload_texture_data_task : public task
    {
    public:
        using shared_data = typename texture_reference<
            DataType,
            Channels
        >::shared_data;
        
    protected:
        std::shared_ptr< shared_data > shared_data_;
        gl::texture_upload_data        upload_data_;
        gl::texture_filter_settings    settings_;
        gl::texture_flags_type         flags_;
        
    public:
        upload_texture_data_task(
            std::shared_ptr< shared_data > sd,
            gl::texture_upload_data        upload_data,
            gl::texture_filter_settings    settings,
            gl::texture_flags_type         flags
        ) :
            shared_data_{ sd                       },
            upload_data_{ std::move( upload_data ) },
            settings_   { settings                 },
            flags_      { flags                    }
        {
            using namespace std::string_literals;
            if( !shared_data_ )
            {
                throw std::invalid_argument(
                    "shared_data to upload_texture_data_task is null"s
                );
            }
        }
        
        task_flags_type flags() const override
        {
            return task_flag::gpu_thread;
        }
        
        bool operator()() override;
    };
    
    template<
        typename   DataType,
        std::size_t Channels
    > class destroy_texture_data_task : public task
    {
    public:
        using shared_data = typename texture_reference<
            DataType,
            Channels
        >::shared_data;
        
    protected:
        // TODO: std::unique_ptr< gl::texture< DataType, Channels > > texture_;
        gl::texture< DataType, Channels >* texture_;
        
    public:
        destroy_texture_data_task(
            // TODO: std::unique_ptr< gl::texture< DataType, Channels > > texture
            gl::texture< DataType, Channels >* texture
        ) : texture_{ texture }
        {}
        
        task_flags_type flags() const override
        {
            return task_flag::gpu_thread;
        }
        
        bool operator()() override;
    };
}


// Texture reference shared data implementation ////////////////////////////////

template<
    typename    DataType,
    std::size_t Channels
> JadeMatrix::yavsg::texture_reference<
    DataType,
    Channels
>::shared_data::shared_data() :
    texture( nullptr )
{}

template<
    typename    DataType,
    std::size_t Channels
> JadeMatrix::yavsg::texture_reference<
    DataType,
    Channels
>::shared_data::~shared_data()
{
    // The shared data being destroyed means that nothing refers to the texture
    // anymore, including any tasks operating on it
    std::unique_lock lock( data_mutex );
    if( texture )
    {
        submit_task( std::make_unique<
            destroy_texture_data_task< DataType, Channels >
        >( texture ) );
    }
}


// Texture reference implementation ////////////////////////////////////////////

template<
    typename    DataType,
    std::size_t Channels
> JadeMatrix::yavsg::texture_reference<
    DataType,
    Channels
>::texture_reference()
{}

template<
    typename    DataType,
    std::size_t Channels
> JadeMatrix::yavsg::texture_reference<
    DataType,
    Channels
>::texture_reference( texture_reference const& o ) :
    shared_data_( o.shared_data_ )
{}

template<
    typename    DataType,
    std::size_t Channels
> JadeMatrix::yavsg::texture_reference<
    DataType,
    Channels
>::operator bool() const
{
    if( shared_data_ )
    {
        std::unique_lock lock( shared_data_->data_mutex );
        return static_cast< bool >( shared_data_->texture );
    }
    
    return false;
}

template<
    typename    DataType,
    std::size_t Channels
> typename JadeMatrix::yavsg::texture_reference<
    DataType,
    Channels
>::texture_type* JadeMatrix::yavsg::texture_reference<
    DataType,
    Channels
>::operator ->()
{
    using namespace std::string_literals;
    
    if( shared_data_ )
    {
        std::unique_lock lock( shared_data_->data_mutex );
        if( shared_data_->texture )
        {
            return shared_data_->texture;
        }
    }
    
    throw std::invalid_argument(
        "yavsg::gl::texture_reference does not refer to a texture ready for "
        "rendering"s
    );
}

template<
    typename    DataType,
    std::size_t Channels
> const typename JadeMatrix::yavsg::texture_reference<
    DataType,
    Channels
>::texture_type* JadeMatrix::yavsg::texture_reference<
    DataType,
    Channels
>::operator ->() const
{
    using namespace std::string_literals;
    
    if( shared_data_ )
    {
        std::unique_lock lock( shared_data_->data_mutex );
        if( shared_data_->texture )
        {
            return shared_data_->texture;
        }
    }
    
    throw std::invalid_argument(
        "yavsg::gl::texture_reference does not refer to a texture ready for "
        "rendering"s
    );
}


// Texture reference static methods implementation /////////////////////////////

template<
    typename    DataType,
    std::size_t Channels
> JadeMatrix::yavsg::texture_reference<
    DataType,
    Channels
> JadeMatrix::yavsg::texture_reference< DataType, Channels >::from_file(
    std::filesystem::path              filename,
    gl::texture_filter_settings const& settings,
    gl::texture_flags_type             flags
)
{
    using ref_type = texture_reference< DataType, Channels >;
    
    ref_type ref;
    
    ref.shared_data_ = std::make_shared< ref_type::shared_data >();
    
    submit_task( std::make_unique<
        load_texture_file_task< DataType, Channels >
    >(
        ref.shared_data_,
        std::move( filename ),
        settings,
        flags
    ) );
    
    return ref;
}


// Task implementations ////////////////////////////////////////////////////////

template<
    typename    DataType,
    std::size_t Channels
> bool JadeMatrix::yavsg::load_texture_file_task<
    DataType,
    Channels
>::operator()()
{
    using namespace std::string_literals;
    
    SDL_Surface* sdl_surface = IMG_Load( filename_.c_str() );
    if( !sdl_surface )
    {
        // TODO: graceful failure (just don't load)
        throw std::runtime_error(
            "failed to load texture \""s
            + filename_.native()
            + "\": "s
            + IMG_GetError()
        );
    }
    
    using format_traits = gl::texture_format_traits< DataType, Channels >;
    
    submit_task( std::make_unique<
        upload_texture_data_task< DataType, Channels >
    >(
        shared_data_,
        gl::process_texture_data(
            sdl_surface,
            flags_,
            format_traits::gl_internal_format
        ),
        settings_,
        flags_
    ) );
    
    return false;
}

template<
    typename    DataType,
    std::size_t Channels
> bool JadeMatrix::yavsg::upload_texture_data_task<
    DataType,
    Channels
>::operator()()
{
    std::unique_lock lock( shared_data_->data_mutex );
    
    // shared_data_->texture = std::make_unique<
    //     gl::texture< DataType, Channels >
    // >();
    shared_data_->texture = new gl::texture< DataType, Channels >();
    
    upload_texture_data(
        shared_data_->texture->gl_texture_id(),
        std::move( upload_data_ ),
        settings_
    );
    
    return false;
}

template<
    typename    DataType,
    std::size_t Channels
> bool JadeMatrix::yavsg::destroy_texture_data_task<
    DataType,
    Channels
>::operator()()
{
    // TODO: texture_ = nullptr;
    delete texture_;
    return false;
}
