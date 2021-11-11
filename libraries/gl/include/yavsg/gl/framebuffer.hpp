#pragma once


#include "error.hpp"
#include "texture.hpp"

#include <cstdint>      // std::int_least16_t, std::int_least32_t
#include <exception>
#include <string>
#include <type_traits>  // std::enable_if
#include <tuple>
#include <utility>      // std::size_t


namespace JadeMatrix::yavsg
{
    class SDL_window_manager;
    class window;
}


namespace JadeMatrix::yavsg::gl // Depth/stencil texture attributes ////////////
{
    template< typename DepthType > struct depth_stecil_dummy
    {
        using sample_type = DepthType;
    };
    
    // There's no `int24`, so just use `int_least32_t`; also, just use
    // `GL_UNSIGNED_INT_24_8` as the incoming type, as (for now) depth/stencil
    // buffers will only be created with `texture_flag::ALLOCATE_ONLY`
    
    template<> struct texture_format_traits< depth_stecil_dummy<
        std::int_least16_t
    >, 1 >
    {
        static const GLint  gl_internal_format = GL_DEPTH_COMPONENT16;
        static const GLenum gl_incoming_format = GL_DEPTH_COMPONENT;
        static const GLenum gl_incoming_type   = GL_UNSIGNED_INT_24_8;
    };
    
    template<> struct texture_format_traits< depth_stecil_dummy<
        std::int_least32_t
    >, 1 >
    {
        static const GLint  gl_internal_format = GL_DEPTH_COMPONENT24;
        static const GLenum gl_incoming_format = GL_DEPTH_COMPONENT;
        static const GLenum gl_incoming_type   = GL_UNSIGNED_INT_24_8;
    };
    
    template<> struct texture_format_traits< depth_stecil_dummy< GLfloat >, 1 >
    {
        static const GLint  gl_internal_format = GL_DEPTH_COMPONENT32F;
        static const GLenum gl_incoming_format = GL_DEPTH_COMPONENT;
        static const GLenum gl_incoming_type   = GL_UNSIGNED_INT_24_8;
    };
    
    template<> struct texture_format_traits< depth_stecil_dummy<
        std::int_least32_t
    >, 2 >
    {
        static const GLint  gl_internal_format = GL_DEPTH24_STENCIL8;
        static const GLenum gl_incoming_format = GL_DEPTH_STENCIL;
        static const GLenum gl_incoming_type   = GL_UNSIGNED_INT_24_8;
    };
    
    template<> struct texture_format_traits< depth_stecil_dummy< GLfloat >, 2 >
    {
        static const GLint  gl_internal_format = GL_DEPTH32F_STENCIL8;
        static const GLenum gl_incoming_format = GL_DEPTH_STENCIL;
        static const GLenum gl_incoming_type   = GL_UNSIGNED_INT_24_8;
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
        
    protected:
        std::size_t _width;
        std::size_t _height;
        alpha_blend_mode _alpha_blending;
        GLuint gl_id;
        
        write_only_framebuffer(
            GLuint,
            std::size_t num_color_targets,
            std::size_t,
            std::size_t
        );
        
        // Protected move assignment operator for use by window management
        write_only_framebuffer& operator =( write_only_framebuffer&& );
        
    public:
        // Delete copy constructor & copy assignment operator
        write_only_framebuffer( const write_only_framebuffer& ) = delete;
        write_only_framebuffer& operator =(
            const write_only_framebuffer&
        ) = delete;
        
        const std::size_t num_color_targets;
        
        std::size_t  width() const;
        std::size_t height() const;
        
        void bind();
        
        alpha_blend_mode alpha_blending( alpha_blend_mode );
        alpha_blend_mode alpha_blending() const;
        
        void dump_BMP( const std::string& descriptive_name );
    };
    
    template< class... ColorTargetTypes > class framebuffer :
        public write_only_framebuffer
    {
    public:
        static const std::size_t num_color_targets
            = sizeof...( ColorTargetTypes );
        
    protected:
        using color_buffers_type = std::tuple< ColorTargetTypes... >;
        using depth_stencil_type = texture<
            depth_stecil_dummy< GLfloat >,
            2
        >;
        
        depth_stencil_type _depth_stencil_buffer;
        color_buffers_type _color_buffers;
        
        GLuint framebuffer_init();
        depth_stencil_type depth_stencil_buffer_init();
        color_buffers_type color_buffers_init();
        void check_finalized();
        
    public:
        framebuffer(
            std::size_t width,
            std::size_t height
        );
        ~framebuffer();
        
        // Delete copy constructor & copy assignment operator
        framebuffer( const framebuffer& ) = delete;
        framebuffer& operator =( const framebuffer& ) = delete;
        
        // Make move assignment operator public
        framebuffer& operator =( framebuffer&& );
        
        const depth_stencil_type& depth_stencil_buffer() const;
        
        template< std::size_t Nth > const typename std::tuple_element<
            Nth,
            color_buffers_type
        >::type& color_buffer() const;
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
            GLuint gl_framebuffer_id,
            std::size_t nth = 0
        )
        {
            std::tuple< FirstColorTargetType > first_target( {
                FirstColorTargetType{
                    width,
                    height,
                    nullptr,
                    {
                        texture_filter_settings::magnify_mode::LINEAR,
                        texture_filter_settings::minify_mode::LINEAR,
                        texture_filter_settings::mipmap_type::NONE,
                    },
                    texture_flag::ALLOCATE_ONLY
                }
            } );
            
            FirstColorTargetType& t = std::get< 0 >( first_target );
            
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + nth,
                GL_TEXTURE_2D,
                t.gl_texture_id(),
                0   // Mipmap level (unused for now)
            );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't attach texture "
                + std::to_string( t.gl_texture_id() )
                + " as color buffer "
                + std::to_string( nth )
                + " to framebuffer "
                + std::to_string( gl_framebuffer_id )
                + " for yavsg::gl::framebuffer_color_target_initializer::init()"
            );
            
            return std::tuple_cat(
                first_target,
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
            GLuint gl_framebuffer_id,
            std::size_t nth = 0
        )
        {
            std::tuple< LastColorTargetType > last_target( {
                LastColorTargetType{
                    width,
                    height,
                    nullptr,
                    {
                        texture_filter_settings::magnify_mode::LINEAR,
                        texture_filter_settings::minify_mode::LINEAR,
                        texture_filter_settings::mipmap_type::NONE,
                    },
                    texture_flag::ALLOCATE_ONLY
                }
            } );
            
            LastColorTargetType& t = std::get< 0 >( last_target );
            
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + nth,
                GL_TEXTURE_2D,
                t.gl_texture_id(),
                0   // Mipmap level (unused for now)
            );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't attach texture "
                + std::to_string( t.gl_texture_id() )
                + " as color buffer "
                + std::to_string( nth )
                + " to framebuffer "
                + std::to_string( gl_framebuffer_id )
                + " for yavsg::gl::framebuffer_color_target_initializer::init()"
            );
            
            return last_target;
        }
    };
}


namespace JadeMatrix::yavsg::gl // Framebuffer implementation //////////////////
{
    template< class... ColorTargetTypes >
    GLuint framebuffer< ColorTargetTypes... >::framebuffer_init()
    {
        // Even though this returns the framebuffer ID, we still need to set it
        // before `framebuffer_init()` returns so we can call the `bind()`
        // below
        glGenFramebuffers( 1, &( this -> gl_id ) );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't generate framebuffer for "
            "yavsg::gl::framebuffer::framebuffer_init()"
        );
        
        try
        {
            // Calling bind() should be OK as long as framebuffers continue to
            // be initialized with alpha blend mode = DISABLED (so bind() won't
            // set a blending mode during construction)
            this -> bind();
            
            return this -> gl_id;
        }
        catch( const summary_error& e )
        {
            glDeleteFramebuffers( 1, &( this -> gl_id ) );
            throw summary_error(
                e.what() + std::string(
                    " for yavsg::gl::framebuffer"
                ),
                e.error_codes
            );
        }
        catch( ... )
        {
            glDeleteFramebuffers( 1, &( this -> gl_id ) );
            throw;
        }
    }
    
    template< class... ColorTargetTypes >
    texture<
        depth_stecil_dummy< GLfloat >,
        2
    > framebuffer< ColorTargetTypes... >::depth_stencil_buffer_init()
    {
        auto t = depth_stencil_type(
            this -> _width,
            this -> _height,
            nullptr,
            {
                texture_filter_settings::magnify_mode::LINEAR,
                texture_filter_settings::minify_mode::LINEAR,
                texture_filter_settings::mipmap_type::NONE,
            },
            texture_flag::ALLOCATE_ONLY
        );
        
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_TEXTURE_2D,
            t.gl_texture_id(),
            0   // Mipmap level (not useful)
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't attach depth/stencil buffer texture "
            + std::to_string( t.gl_texture_id() )
            + " to framebuffer "
            + std::to_string( this -> gl_id )
            + " for yavsg::gl::framebuffer::depth_stencil_buffer_init()"
        );
        
        return t;
    }
    
    template< class... ColorTargetTypes >
    typename framebuffer< ColorTargetTypes... >::color_buffers_type framebuffer<
        ColorTargetTypes...
    >::color_buffers_init()
    {
        try
        {
            return framebuffer_color_target_initializer<
                ColorTargetTypes...
            >::init(
                this -> _width,
                this -> _height,
                this -> gl_id
            );
        }
        catch( const summary_error& e )
        {
            throw summary_error(
                e.what() + std::string(
                    " for yavsg::gl::framebuffer::framebuffer_init()"
                ),
                e.error_codes
            );
        }
    }
    
    template< class... ColorTargetTypes >
    void framebuffer< ColorTargetTypes... >::check_finalized()
    {
        auto status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
        
        if( status != GL_FRAMEBUFFER_COMPLETE )
        {
            std::string status_string;
            switch( status )
            {
            case GL_FRAMEBUFFER_UNDEFINED:
                status_string = "GL_FRAMEBUFFER_UNDEFINED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                status_string = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                status_string = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                status_string = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                status_string = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                status_string = "GL_FRAMEBUFFER_UNSUPPORTED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                status_string = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                status_string = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
                break;
            case 0:
                status_string = "error calling glCheckFramebufferStatus()";
                break;
            default:
                status_string = "<unrecognized code>";
                break;
            }
            
            throw std::runtime_error(
                "failed to complete famebuffer "
                + std::to_string( this -> gl_id )
                + " ("
                + status_string
                + ") for yavsg::gl::framebuffer"
            );
        }
    }
    
    template< class... ColorTargetTypes >
    framebuffer< ColorTargetTypes... >::framebuffer(
        std::size_t width,
        std::size_t height
    ) :
        write_only_framebuffer(
            framebuffer_init(),
            sizeof...( ColorTargetTypes ),
            width,
            height
        ),
        _depth_stencil_buffer( depth_stencil_buffer_init() ),
        _color_buffers( color_buffers_init() )
    {
        check_finalized();
    }
    
    template< class... ColorTargetTypes >
    framebuffer< ColorTargetTypes... >::~framebuffer()
    {
        glDeleteFramebuffers( 1, &( this -> gl_id ) );
    }
    
    template< class... ColorTargetTypes >
    framebuffer< ColorTargetTypes... >& framebuffer<
        ColorTargetTypes...
    >::operator =( framebuffer&& o )
    {
        // `write_only_framebuffer` members
        std::swap( _width         , o._width          );
        std::swap( _height        , o._height         );
        std::swap( _alpha_blending, o._alpha_blending );
        std::swap( gl_id          , o.gl_id           );
        
        // `framebuffer` memebrs
        std::swap( _depth_stencil_buffer, o._depth_stencil_buffer );
        std::swap( _color_buffers       , o._color_buffers        );
        
        return *this;
    }
    
    template< class... ColorTargetTypes >
    const typename framebuffer<
        ColorTargetTypes...
    >::depth_stencil_type& framebuffer<
        ColorTargetTypes...
    >::depth_stencil_buffer() const
    {
        return _depth_stencil_buffer;
    }
    
    template< class... ColorTargetTypes >
    template< std::size_t Nth >
    const typename std::tuple_element<
        Nth,
        typename framebuffer< ColorTargetTypes... >::color_buffers_type
    >::type& framebuffer< ColorTargetTypes... >::color_buffer() const
    {
        return std::get< Nth >( _color_buffers );
    }
}
