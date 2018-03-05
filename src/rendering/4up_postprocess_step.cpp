#include "../../include/rendering/4up_postprocess_step.hpp"

#include "../../include/rendering/gl_tut.hpp"
#include "../../include/rendering/shader_variable_names.hpp"

#include "../../include/gl/shader.hpp"


namespace
{
    class push_framebuffer
    {
    protected:
        GLint pushed;
    public:
        push_framebuffer()
        {
            glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &pushed );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't get current draw framebuffer for push_framebuffer"
            );
        };
        ~push_framebuffer()
        {
            glBindFramebuffer( GL_FRAMEBUFFER, pushed );
            YAVSG_GL_THROW_FOR_ERRORS(
                "couldn't re-bind framebuffer "
                + std::to_string( pushed )
                + " for push_framebuffer"
            );
        }
    };
    
    const std::string vertex_shader = R"<<<(
#version 150 core
in vec3 vertex_in_position;
in vec2 vertex_in_texture;
out VERTEX_OUT
{
    vec2 texture;
} vertex_out;
void main()
{
    vertex_out.texture = vertex_in_texture;
    gl_Position = vec4(
        vertex_in_position.x,
        vertex_in_position.y,
        0.0,
        1.0
    );
}
)<<<";
    
    const std::string fragment_shader = R"<<<(
#version 150 core
in VERTEX_OUT
{
    vec2 texture;
} fragment_in;
out vec4 fragment_out_color;
uniform sampler2D framebuffer_source_color;
void main()
{
    fragment_out_color = texture(
        framebuffer_source_color,
        vec2(
            fragment_in.texture.x,
            fragment_in.texture.y * -1
        )
    );
}
)<<<";
}


namespace yavsg
{
    debug_4up_postprocess_step::debug_4up_postprocess_step(
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
            gl::shader( GL_VERTEX_SHADER  ,  vertex_shader ).id,
            gl::shader( GL_FRAGMENT_SHADER,fragment_shader ).id
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
        sub_buffer(
            gl_tut::window_width,
            gl_tut::window_height
        )
    {
        postprocess_program.link_attribute< 0 >(
            shader_string( shader_string_id::VERTEX_IN_POSITION ),
            vertices
        );
        postprocess_program.link_attribute< 1 >(
            shader_string( shader_string_id::VERTEX_IN_TEXTURE ),
            vertices
        );
        postprocess_program.bind_target< 0 >(
            shader_string( shader_string_id::FRAGMENT_OUT_COLOR )
        );
    }
    
    debug_4up_postprocess_step::~debug_4up_postprocess_step()
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
    
    void debug_4up_postprocess_step::run( framebuffer_type& source )
    {
        // TODO: error handling
        
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        glDisable( GL_DEPTH_TEST );
        
        struct substep_info
        {
            postprocess_step< 1 >* step;
            gl::index_buffer& indices;
        };
        
        yavsg::gl::framebuffer< 1 >* source_buffer;
        
        for( auto substep : {
            substep_info{     top_left,     top_left_indices },
            substep_info{    top_right,    top_right_indices },
            substep_info{  bottom_left,  bottom_left_indices },
            substep_info{ bottom_right, bottom_right_indices }
        } )
        {
            if( substep.step )
            {
                push_framebuffer pushed_framebuffer;
                
                sub_buffer.bind();
                substep.step -> run( source );
                
                source_buffer = &sub_buffer;
            }
            else
                source_buffer = &source;
            
            source_buffer -> color_buffer< 0 >().bind_as< 0 >();
            postprocess_program.set_uniform(
                shader_string( shader_string_id::FRAMEBUFFER_SOURCE_COLOR ),
                0
            );
            
            postprocess_program.run(
                vertices,
                substep.indices
            );
        }
    }
}