#include <yavsg/rendering/4up_postprocess_step.hpp>

#include <yavsg/gl/error.hpp>
#include <yavsg/gl/shader.hpp>
#include <yavsg/rendering/basic_postprocess_step.hpp>
#include <yavsg/rendering/shader_utils.hpp> // shaders_dir
#include <yavsg/rendering/shader_variable_names.hpp>

#include <assert.h>
#include <limits>


namespace JadeMatrix::yavsg
{
    debug_4up_postprocess_step::debug_4up_postprocess_step(
        std::unique_ptr< child_type > top_left,
        std::unique_ptr< child_type > top_right,
        std::unique_ptr< child_type > bottom_left,
        std::unique_ptr< child_type > bottom_right
    ) :
        top_left{     std::move( top_left     ) },
        top_right{    std::move( top_right    ) },
        bottom_left{  std::move( bottom_left  ) },
        bottom_right{ std::move( bottom_right ) }
    {
        struct substep_info
        {
            std::unique_ptr< child_type >& step;
        };
        
        for( auto substep : {
            substep_info{ this -> top_left     },
            substep_info{ this -> top_right    },
            substep_info{ this -> bottom_left  },
            substep_info{ this -> bottom_right }
        } )
            if( !substep.step )
                substep.step = std::make_unique< basic_postprocess_step >(
                    shaders_dir() + "/postprocess.frag"
                );
    }
    
    void debug_4up_postprocess_step::run(
        const source_type         & source,
        gl::write_only_framebuffer& target
    )
    {
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't clear buffer for yavsg::debug_4up_postprocess_step::run()"
        );
        
        glDisable( GL_DEPTH_TEST );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't disable depth testing for "
            "yavsg::debug_4up_postprocess_step::run()"
        );
        
        struct substep_info
        {
            child_type* step;
            unsigned char x_offset_factor;
            unsigned char y_offset_factor;
        };
        
        for( auto substep : {
            substep_info{     top_left.get(), 0, 1 },
            substep_info{    top_right.get(), 1, 1 },
            substep_info{  bottom_left.get(), 0, 0 },
            substep_info{ bottom_right.get(), 1, 0 }
        } )
        {
            auto half_width  = target. width() / 2;
            auto half_height = target.height() / 2;
            
            assert( half_height <= std::numeric_limits< GLint >::max() );
            assert( half_width  <= std::numeric_limits< GLint >::max() );
            
            glViewport(
                static_cast< GLint >( substep.x_offset_factor * half_width  ),
                static_cast< GLint >( substep.y_offset_factor * half_height ),
                static_cast< GLint >(                           half_width  ),
                static_cast< GLint >(                           half_height )
            );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't set viewport for "
                "yavsg::debug_4up_postprocess_step::run()"
            );
            
            glEnable( GL_SCISSOR_TEST );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't enable scissor testing for "
                "yavsg::debug_4up_postprocess_step::run()"
            );
            
            glScissor(
                static_cast< GLint >( substep.x_offset_factor * half_width  ),
                static_cast< GLint >( substep.y_offset_factor * half_height ),
                static_cast< GLint >(                           half_width  ),
                static_cast< GLint >(                           half_height )
            );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't set scissor for "
                "yavsg::debug_4up_postprocess_step::run()"
            );
            
            if( substep.step )
            {
                target.bind();
                substep.step -> run( source, target );
            }
        }
        
        glDisable( GL_SCISSOR_TEST );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't disable scissor testing for "
            "yavsg::debug_4up_postprocess_step::run()"
        );
    }
}
