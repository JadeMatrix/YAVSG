#version 150 core


in vec2 texture_coord;

out vec4 color_out;

uniform sampler2D framebuffer;
uniform sampler2D framebuffer_depth_stencil;
uniform float view_width;
uniform float half_height;


void main()
{
    float depth = texture( framebuffer_depth_stencil, texture_coord ).r;
    
    // depth = pow( depth, 256 );
    color_out = vec4( depth, depth, depth, 1.0 );
    
    // color_out = vec4(
    //     1.0, // texture( framebuffer_depth_stencil, texture_coord ).r,
    //     texture( framebuffer_depth_stencil, texture_coord ).a,
    //     1.0,
    //     1.0
    // );
}
