#include "../../include/engine/4up_postprocess_step.hpp"

#include "../../include/gl/shader.hpp"
#include "../../include/engine/basic_postprocess_step.hpp"
#include "../../include/rendering/shader_variable_names.hpp"


namespace yavsg
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
                    "../src/shaders/postprocess.frag"
                );
    }
    
    void debug_4up_postprocess_step::run(
        source_type               & source,
        gl::write_only_framebuffer& target
    )
    {
        // TODO: error handling
        
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        glDisable( GL_DEPTH_TEST );
        
        struct substep_info
        {
            child_type* step;
            int x_offset_factor;
            int y_offset_factor;
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
            
            glViewport(
                substep.x_offset_factor * half_width,
                substep.y_offset_factor * half_height,
                half_width,
                half_height
            );
            
            glEnable( GL_SCISSOR_TEST );
            glScissor(
                substep.x_offset_factor * half_width,
                substep.y_offset_factor * half_height,
                half_width,
                half_height
            );
            
            if( substep.step )
            {
                target.bind();
                substep.step -> run( source, target );
            }
        }
        
        glDisable( GL_SCISSOR_TEST );
    }
}
