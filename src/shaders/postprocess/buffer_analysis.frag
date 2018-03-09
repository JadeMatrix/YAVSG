#version 150 core

// Input ///////////////////////////////////////////////////////////////////////

in VERTEX_OUT
{
    vec2 texture;
} fragment_in;

// Output //////////////////////////////////////////////////////////////////////

out vec4 fragment_out_color;

////////////////////////////////////////////////////////////////////////////////

uniform sampler2D framebuffer_source_color;
uniform sampler2D framebuffer_source_depth;

void main()
{
    float alpha = texture(
        framebuffer_source_color,
        fragment_in.texture
    ).a;
    float depth = texture(
        framebuffer_source_depth,
        fragment_in.texture
    ).r;
    
    fragment_out_color = vec4( alpha, depth, 1.0, 1.0 );
}
