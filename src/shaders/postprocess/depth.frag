#version 150 core

// Input ///////////////////////////////////////////////////////////////////////

in VERTEX_OUT
{
    vec2 texture;
} fragment_in;

// Output //////////////////////////////////////////////////////////////////////

out vec4 fragment_out_color;

////////////////////////////////////////////////////////////////////////////////

// uniform sampler2D framebuffer_source_color;
uniform sampler2D framebuffer_source_depth_stencil;

void main()
{
    float depth = texture(
        framebuffer_source_depth_stencil,
        fragment_in.texture
    ).r;
    
    // depth = pow( depth, 256 );
    fragment_out_color = vec4( depth, depth, depth, 1.0 );
    
    // fragment_out_color = vec4(
    //     1.0, // texture( framebuffer_source_depth_stencil, fragment_in.texture ).r,
    //     texture( framebuffer_source_depth_stencil, fragment_in.texture ).a,
    //     1.0,
    //     1.0
    // );
}
