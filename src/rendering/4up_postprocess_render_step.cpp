#include "../../include/rendering/gl_tut.hpp"
#include "../../include/rendering/4up_postprocess_render_step.hpp"

#include "../../include/gl/shader.hpp"


namespace gl_tut // gl_tut_postprocess_render_step implementations ////////////////////
{
    debug_4up_postprocess_render_step::debug_4up_postprocess_render_step(
        postprocess_step< 1 >* top_left,
        postprocess_step< 1 >* top_right,
        postprocess_step< 1 >* bottom_left,
        postprocess_step< 1 >* bottom_right
    ) :
        top_left(     top_left     ),
        top_right(    top_right    ),
        bottom_left(  bottom_left  ),
        bottom_right( bottom_right ),
        postprocess_program( {
            yavsg::gl::shader::from_file(
                GL_VERTEX_SHADER,
                "../src/shaders/postprocess.vert"
            ).id,
            yavsg::gl::shader::from_file(
                GL_FRAGMENT_SHADER,
                "../src/shaders/postprocess.frag"
            ).id
        } ),
        vertices( {
            /*
            (-1, 1)   ( 0, 1)   ( 1, 1)
            
            (-1, 0)   ( 0, 0)   ( 1, 0)
            
            (-1,-1)   ( 0,-1)   ( 1,-1)
            */
            
            // Top left
            { { -1.0f,  1.0f }, { 0.0f, 0.0f } }, //    top left
            { {  0.0f,  1.0f }, { 1.0f, 0.0f } }, //    top right
            { {  0.0f,  0.0f }, { 1.0f, 1.0f } }, // bottom right
            { { -1.0f,  0.0f }, { 0.0f, 1.0f } }, // bottom left
            
            // Top right
            { {  0.0f,  1.0f }, { 0.0f, 0.0f } }, //    top left
            { {  1.0f,  1.0f }, { 1.0f, 0.0f } }, //    top right
            { {  1.0f,  0.0f }, { 1.0f, 1.0f } }, // bottom right
            { {  0.0f,  0.0f }, { 0.0f, 1.0f } }, // bottom left
            
            // Bottom left
            { { -1.0f,  0.0f }, { 0.0f, 0.0f } }, //    top left
            { {  0.0f,  0.0f }, { 1.0f, 0.0f } }, //    top right
            { {  0.0f, -1.0f }, { 1.0f, 1.0f } }, // bottom right
            { { -1.0f, -1.0f }, { 0.0f, 1.0f } }, // bottom left
            
            // Bottom right
            { {  0.0f,  0.0f }, { 0.0f, 0.0f } }, //    top left
            { {  1.0f,  0.0f }, { 1.0f, 0.0f } }, //    top right
            { {  1.0f, -1.0f }, { 1.0f, 1.0f } }, // bottom right
            { {  0.0f, -1.0f }, { 0.0f, 1.0f } }  // bottom left
        } ),
        top_left_indices( {
            0 +  0, 1 +  0, 2 +  0,
            2 +  0, 3 +  0, 0 +  0
        } ),
        top_right_indices( {
            0 +  4, 1 +  4, 2 +  4,
            2 +  4, 3 +  4, 0 +  4
        } ),
        bottom_left_indices( {
            0 +  8, 1 +  8, 2 +  8,
            2 +  8, 3 +  8, 0 +  8
        } ),
        bottom_right_indices( {
            0 + 12, 1 + 12, 2 + 12,
            2 + 12, 3 + 12, 0 + 12
        } ),
    {
        postprocess_program.link_attribute< 0 >( "position"        , vertices );
        postprocess_program.link_attribute< 1 >( "texture_coord_in", vertices );
        postprocess_program.bind_target< 0 >( "color_out" );
    }
    
    void debug_4up_postprocess_render_step::~debug_4up_postprocess_render_step()
    {
        if( top_left )
            delete top_left;
        if( top_right )
            delete top_right;
        if( bottom_left )
            delete bottom_left;
        if( bottom_right )
            delete bottom_right;
    }
    
    void debug_4up_postprocess_render_step::run(
        yavsg::gl::framebuffer< 1 >& source
    )
    {
        // TODO: error handling
        
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        glDisable( GL_DEPTH_TEST );
        
        for( auto substep : {
            top_left,
            top_right,
            bottom_left,
            bottom_right
        } )
            if( substep )
            {
                
            }
            else
            {
                source.color_buffer< 0 >().bind_as< 0 >();
                postprocess_program.set_uniform( "framebuffer", 0 );
                
                // Depth buffer
                source.depth_stencil_buffer().bind_as< 1 >();
                postprocess_program.set_uniform( "framebuffer_depth_stencil", 1 );
                
                postprocess_program.set_uniform(
                    "view_width",
                    ( GLfloat )window_width
                );
                postprocess_program.set_uniform(
                    "view_height",
                    ( GLfloat )window_height
                );
                
                postprocess_program.run(
                    vertices,
                    indices
                );
            }
    }
}