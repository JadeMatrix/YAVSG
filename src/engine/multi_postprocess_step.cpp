#include "../../include/engine/multi_postprocess_step.hpp"

#include "../../include/gl/error.hpp"
#include "../../include/rendering/shader_variable_names.hpp"

#include <exception>
#include <fstream>
#include <set>


namespace
{
    const std::string fragment_shader_header = R"<<<(
#version 150 core

in VERTEX_OUT
{
    vec2 texture;
} fragment_in;

uniform sampler2D framebuffer_source_color;
uniform sampler2D framebuffer_source_depth;
uniform struct
{
    float width;
    float height;
} framebuffer_target;

out vec4 fragment_out_color;
)<<<";
    
    const std::string fragment_shader_body_pre = R"<<<(
void main()
{
    fragment_out_color = texture(
        framebuffer_source_color,
        fragment_in.texture
    );
)<<<";
    
    const std::string fragment_shader_body_post = "}";
}


namespace yavsg
{
    gl::shader multi_postprocess_step::generate_fragment_shader(
        const std::vector< std::string > function_names
    )
    {
        std::string fragment_shader_source = fragment_shader_header;
        
        // Iterating over a set of function names ensures each one is defined
        // only once even if it's called multiple times
        for( auto& function_name : std::set< std::string >{
            function_names.begin(),
            function_names.end()
        } )
        {
            auto filename = (
                "../src/shaders/postprocess/"
                + function_name
                + ".frag"
            );
            
            std::filebuf function_source_file;
            function_source_file.open(
                filename,
                std::ios_base::in
            );
            
            if( !function_source_file.is_open() )
                throw std::runtime_error(
                    "could not open shader function source file \""
                    + filename
                    + "\""
                );
            
            std::string function_source = std::string(
                std::istreambuf_iterator< char >( &function_source_file ),
                {}
            );
            
            fragment_shader_source += function_source;
        }
        
        fragment_shader_source += fragment_shader_body_pre;
        
        for( auto& function_name : function_names )
            fragment_shader_source += function_name + "();";
        
        fragment_shader_source += fragment_shader_body_post;
        
        return gl::shader{ GL_FRAGMENT_SHADER, fragment_shader_source };
    }
    
    multi_postprocess_step::multi_postprocess_step(
        const std::vector< std::string > function_names
    ) :
        multi_program{ {
            gl::shader::from_file(
                GL_VERTEX_SHADER,
                "../src/shaders/postprocess.vert"
            ).id,
            generate_fragment_shader( function_names ).id
        } },
        vertices{ {
            { { -1.0f,  1.0f }, { 0.0f, 0.0f } }, //    top left
            { {  1.0f,  1.0f }, { 1.0f, 0.0f } }, //    top right
            { {  1.0f, -1.0f }, { 1.0f, 1.0f } }, // bottom right
            { { -1.0f, -1.0f }, { 0.0f, 1.0f } }  // bottom left
        } },
        indices{ {
            0, 1, 2,
            2, 3, 0
        } }
    {
        multi_program.link_attribute< 0 >(
            shader_string_id::VERTEX_IN_POSITION,
            vertices
        );
        multi_program.link_attribute< 1 >(
            shader_string_id::VERTEX_IN_TEXTURE,
            vertices
        );
        multi_program.bind_target< 0 >(
            shader_string_id::FRAGMENT_OUT_COLOR
        );
    }
    
    void multi_postprocess_step::run(
        gl::framebuffer< gl::texture< GLfloat, 3 > >& source,
        gl::write_only_framebuffer            & target
    )
    {
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't clear buffer for yavsg::multi_postprocess_step::run()"
        );
        
        glDisable( GL_DEPTH_TEST );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't disable depth testing for "
            "yavsg::multi_postprocess_step::run()"
        );
        
        source.color_buffer< 0 >().bind_as< 0 >();
        multi_program.set_uniform(
            shader_string_id::FRAMEBUFFER_SOURCE_COLOR,
            0
        );
        
        source.depth_stencil_buffer().bind_as< 1 >();
        multi_program.set_uniform(
            shader_string_id::FRAMEBUFFER_SOURCE_DEPTH,
            1
        );
        
        multi_program.set_uniform(
            shader_string_id::FRAMEBUFFER_TARGET_WIDTH,
            static_cast< GLfloat >( target.width() )
        );
        multi_program.set_uniform(
            shader_string_id::FRAMEBUFFER_TARGET_HEIGHT,
            static_cast< GLfloat >( target.height() )
        );
        
        multi_program.run(
            vertices,
            indices
        );
    }
}
