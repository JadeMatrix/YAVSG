#pragma once


#include "error.hpp"
#include "texture.hpp"

#include <cstddef>      // size_t
#include <cstdint>      // int_least16_t, int_least32_t
#include <stdexcept>    // runtime_error
#include <string>
#include <type_traits>  // enable_if
#include <tuple>
#include <utility>      // move


namespace JadeMatrix::yavsg
{
    class SDL_window_manager;
    class window;
}


namespace JadeMatrix::yavsg::gl
{
    GLenum color_attachment_name( std::size_t );
}


namespace JadeMatrix::yavsg::gl // Depth/stencil texture attributes ////////////
{
    template< typename DepthType > struct depth_stecil_dummy
    {
        using sample_type = DepthType;
    };
    
    // There's no `int24`, so just use `int_least32_t`; also, just use
    // `GL_UNSIGNED_INT_24_8` as the incoming type, as (for now) depth/stencil
    // buffers will only be created with `texture_flag::allocate_only`
    
    template<> struct texture_format_traits<
        depth_stecil_dummy< std::int_least16_t >,
        1
    >
    {
        static constexpr GLint  gl_internal_format = GL_DEPTH_COMPONENT16;
        static constexpr GLenum gl_incoming_format = GL_DEPTH_COMPONENT;
        static constexpr GLenum gl_incoming_type   = GL_UNSIGNED_INT_24_8;
    };
    
    template<> struct texture_format_traits<
        depth_stecil_dummy< std::int_least32_t >,
        1
    >
    {
        static constexpr GLint  gl_internal_format = GL_DEPTH_COMPONENT24;
        static constexpr GLenum gl_incoming_format = GL_DEPTH_COMPONENT;
        static constexpr GLenum gl_incoming_type   = GL_UNSIGNED_INT_24_8;
    };
    
    template<> struct texture_format_traits<
        depth_stecil_dummy< GLfloat >,
        1
    >
    {
        static constexpr GLint  gl_internal_format = GL_DEPTH_COMPONENT32F;
        static constexpr GLenum gl_incoming_format = GL_DEPTH_COMPONENT;
        static constexpr GLenum gl_incoming_type   = GL_UNSIGNED_INT_24_8;
    };
    
    template<> struct texture_format_traits<
        depth_stecil_dummy< std::int_least32_t >,
        2
    >
    {
        static constexpr GLint  gl_internal_format = GL_DEPTH24_STENCIL8;
        static constexpr GLenum gl_incoming_format = GL_DEPTH_STENCIL;
        static constexpr GLenum gl_incoming_type   = GL_UNSIGNED_INT_24_8;
    };
    
    template<> struct texture_format_traits<
        depth_stecil_dummy< GLfloat >,
        2
    >
    {
        static constexpr GLint  gl_internal_format = GL_DEPTH32F_STENCIL8;
        static constexpr GLenum gl_incoming_format = GL_DEPTH_STENCIL;
        static constexpr GLenum gl_incoming_type   = GL_UNSIGNED_INT_24_8;
    };
}


namespace JadeMatrix::yavsg::gl // Framebuffer classes /////////////////////////
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
    
    class write_only_framebuffer
    {
        // This is so the window can create a base_framebuffer for the default
        // framebuffer
        friend class yavsg::SDL_window_manager;
        friend class yavsg::window;
        
    public:
        // Delete copy constructor & copy assignment operator
        write_only_framebuffer( write_only_framebuffer const& ) = delete;
        write_only_framebuffer& operator =(
            write_only_framebuffer const&
        ) = delete;
        
        const std::size_t num_color_targets;
        
        std::size_t  width() const;
        std::size_t height() const;
        
        void bind();
        
        alpha_blend_mode alpha_blending( alpha_blend_mode );
        alpha_blend_mode alpha_blending() const;
        
        void dump_BMP( std::string const& descriptive_name );
        
    protected:
        std::size_t      width_;
        std::size_t      height_;
        alpha_blend_mode alpha_blending_;
        GLuint           gl_id_;
        
        write_only_framebuffer(
            GLuint      gl_id,
            std::size_t num_color_targets,
            std::size_t width,
            std::size_t height
        );
        
        // Protected move assignment operator for use by window management
        write_only_framebuffer& operator =( write_only_framebuffer&& );
    };
    
    template< class... ColorTargetTypes > class framebuffer :
        public write_only_framebuffer
    {
    public:
        static constexpr std::size_t num_color_targets
            = sizeof...( ColorTargetTypes );
        using color_buffers_type = std::tuple< ColorTargetTypes... >;
        using depth_stencil_type = texture<
            depth_stecil_dummy< GLfloat >,
            2
        >;
        
        framebuffer(
            std::size_t width,
            std::size_t height
        );
        ~framebuffer();
        
        // Delete copy constructor & copy assignment operator
        framebuffer( framebuffer const& ) = delete;
        framebuffer& operator =( framebuffer const& ) = delete;
        
        // Make move assignment operator public
        framebuffer& operator =( framebuffer&& );
        
        depth_stencil_type const& depth_stencil_buffer() const;
        
        template< std::size_t Nth > typename std::tuple_element<
            Nth,
            color_buffers_type
        >::type const& color_buffer() const;
        
    protected:
        depth_stencil_type depth_stencil_buffer_;
        color_buffers_type color_buffers_;
        
        GLuint framebuffer_init();
        depth_stencil_type depth_stencil_buffer_init();
        color_buffers_type color_buffers_init();
        void check_finalized();
    };
}


namespace JadeMatrix::yavsg::gl // Framebuffer color target init ///////////////
{
    template<
        class FirstColorTargetType,
        class... ColorTargetTypes
    > struct framebuffer_color_target_initializer
    {
        static std::tuple< FirstColorTargetType, ColorTargetTypes... > init(
            std::size_t width,
            std::size_t height,
            GLuint      gl_framebuffer_id,
            std::size_t nth = 0
        )
        {
            using namespace std::string_literals;
            
            std::tuple< FirstColorTargetType > first_target( {
                FirstColorTargetType{
                    width,
                    height,
                    nullptr,
                    {
                        texture_filter_settings::magnify_mode::linear,
                        texture_filter_settings::minify_mode::linear,
                        texture_filter_settings::mipmap_type::none,
                    },
                    texture_flag::allocate_only
                }
            } );
            
            FirstColorTargetType const& t = std::get< 0 >( first_target );
            
            gl::FramebufferTexture2D(
                GL_FRAMEBUFFER,
                color_attachment_name( nth ),
                GL_TEXTURE_2D,
                t.gl_texture_id(),
                0   // Mipmap level (unused for now)
            );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't attach texture "s
                + std::to_string( t.gl_texture_id() )
                + " as color buffer "s
                + std::to_string( nth )
                + " to framebuffer "s
                + std::to_string( gl_framebuffer_id )
                + " for yavsg::gl::framebuffer_color_target_initializer::init()"s
            );
            
            return std::tuple_cat(
                std::move( first_target ),
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
            GLuint      gl_framebuffer_id,
            std::size_t nth = 0
        )
        {
            using namespace std::string_literals;
            
            std::tuple< LastColorTargetType > last_target( {
                LastColorTargetType{
                    width,
                    height,
                    nullptr,
                    {
                        texture_filter_settings::magnify_mode::linear,
                        texture_filter_settings::minify_mode::linear,
                        texture_filter_settings::mipmap_type::none,
                    },
                    texture_flag::allocate_only
                }
            } );
            
            LastColorTargetType const& t = std::get< 0 >( last_target );
            
            gl::FramebufferTexture2D(
                GL_FRAMEBUFFER,
                color_attachment_name( nth ),
                GL_TEXTURE_2D,
                t.gl_texture_id(),
                0   // Mipmap level (unused for now)
            );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't attach texture "s
                + std::to_string( t.gl_texture_id() )
                + " as color buffer "s
                + std::to_string( nth )
                + " to framebuffer "s
                + std::to_string( gl_framebuffer_id )
                + " for yavsg::gl::framebuffer_color_target_initializer::init()"s
            );
            
            return last_target;
        }
    };
}


// Framebuffer implementation //////////////////////////////////////////////////

template< class... ColorTargetTypes >
GLuint JadeMatrix::yavsg::gl::framebuffer<
    ColorTargetTypes...
>::framebuffer_init()
{
    using namespace std::string_literals;
    
    // Even though this returns the framebuffer ID, we still need to set it
    // before `framebuffer_init()` returns so we can call the `bind()`
    // below
    gl::GenFramebuffers( 1, &gl_id_ );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't generate framebuffer for "
        "yavsg::gl::framebuffer::framebuffer_init()"s
    );
    
    try
    {
        // Calling bind() should be OK as long as framebuffers continue to
        // be initialized with alpha blend mode = DISABLED (so bind() won't
        // set a blending mode during construction)
        bind();
        
        return gl_id_;
    }
    catch( summary_error const& e )
    {
        gl::DeleteFramebuffers( 1, &gl_id_ );
        throw summary_error(
            e.what() + " for yavsg::gl::framebuffer"s,
            e.error_codes
        );
    }
    catch( ... )
    {
        gl::DeleteFramebuffers( 1, &gl_id_ );
        throw;
    }
}

template< class... ColorTargetTypes >
JadeMatrix::yavsg::gl::texture<
    JadeMatrix::yavsg::gl::depth_stecil_dummy< GLfloat >,
    2
> JadeMatrix::yavsg::gl::framebuffer<
    ColorTargetTypes...
>::depth_stencil_buffer_init()
{
    using namespace std::string_literals;
    
    auto t = depth_stencil_type(
        width_,
        height_,
        nullptr,
        {
            texture_filter_settings::magnify_mode::linear,
            texture_filter_settings::minify_mode::linear,
            texture_filter_settings::mipmap_type::none,
        },
        texture_flag::allocate_only
    );
    
    gl::FramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_TEXTURE_2D,
        t.gl_texture_id(),
        0   // Mipmap level (not useful)
    );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't attach depth/stencil buffer texture "s
        + std::to_string( t.gl_texture_id() )
        + " to framebuffer "s
        + std::to_string( gl_id_ )
        + " for yavsg::gl::framebuffer::depth_stencil_buffer_init()"s
    );
    
    return t;
}

template< class... ColorTargetTypes >
typename JadeMatrix::yavsg::gl::framebuffer<
    ColorTargetTypes...
>::color_buffers_type JadeMatrix::yavsg::gl::framebuffer<
    ColorTargetTypes...
>::color_buffers_init()
{
    using namespace std::string_literals;
    
    try
    {
        return framebuffer_color_target_initializer<
            ColorTargetTypes...
        >::init( width_, height_, gl_id_ );
    }
    catch( summary_error const& e )
    {
        throw summary_error(
            e.what() + " for yavsg::gl::framebuffer::framebuffer_init()"s,
            e.error_codes
        );
    }
}

template< class... ColorTargetTypes >
void JadeMatrix::yavsg::gl::framebuffer<
    ColorTargetTypes...
>::check_finalized()
{
    using namespace std::string_literals;
    
    auto const status = gl::CheckFramebufferStatus( GL_FRAMEBUFFER );
    
    if( status != GL_FRAMEBUFFER_COMPLETE )
    {
        std::string status_string;
        switch( status )
        {
        case GL_FRAMEBUFFER_UNDEFINED:
            status_string = "GL_FRAMEBUFFER_UNDEFINED"s;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            status_string = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"s;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            status_string = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"s;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            status_string = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"s;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            status_string = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"s;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            status_string = "GL_FRAMEBUFFER_UNSUPPORTED"s;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            status_string = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"s;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            status_string = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"s;
            break;
        case 0:
            status_string = "error calling glCheckFramebufferStatus()"s;
            break;
        default:
            status_string = "<unrecognized code>"s;
            break;
        }
        
        throw std::runtime_error(
            "failed to complete famebuffer "s
            + std::to_string( gl_id_ )
            + " ("s
            + status_string
            + ") for yavsg::gl::framebuffer"s
        );
    }
}

template< class... ColorTargetTypes >
JadeMatrix::yavsg::gl::framebuffer< ColorTargetTypes... >::framebuffer(
    std::size_t width,
    std::size_t height
) :
    write_only_framebuffer(
        framebuffer_init(),
        sizeof...( ColorTargetTypes ),
        width,
        height
    ),
    depth_stencil_buffer_( depth_stencil_buffer_init() ),
    color_buffers_( color_buffers_init() )
{
    check_finalized();
}

template< class... ColorTargetTypes >
JadeMatrix::yavsg::gl::framebuffer< ColorTargetTypes... >::~framebuffer()
{
    gl::DeleteFramebuffers( 1, &gl_id_ );
}

template< class... ColorTargetTypes >
JadeMatrix::yavsg::gl::framebuffer< ColorTargetTypes... >&
JadeMatrix::yavsg::gl::framebuffer<
    ColorTargetTypes...
>::operator =( framebuffer&& o )
{
    // `write_only_framebuffer` members
    std::swap( width_         , o.width_          );
    std::swap( height_        , o.height_         );
    std::swap( alpha_blending_, o.alpha_blending_ );
    std::swap( gl_id_         , o.gl_id_          );
    
    // `framebuffer` memebrs
    std::swap( depth_stencil_buffer_, o.depth_stencil_buffer_ );
    std::swap( color_buffers_       , o.color_buffers_        );
    
    return *this;
}

template< class... ColorTargetTypes >
typename JadeMatrix::yavsg::gl::framebuffer<
    ColorTargetTypes...
>::depth_stencil_type const& JadeMatrix::yavsg::gl::framebuffer<
    ColorTargetTypes...
>::depth_stencil_buffer() const
{
    return depth_stencil_buffer_;
}

template< class... ColorTargetTypes >
template< std::size_t Nth >
typename std::tuple_element<
    Nth,
    typename JadeMatrix::yavsg::gl::framebuffer<
        ColorTargetTypes...
    >::color_buffers_type
>::type const& JadeMatrix::yavsg::gl::framebuffer<
    ColorTargetTypes...
>::color_buffer() const
{
    return std::get< Nth >( color_buffers_ );
}
