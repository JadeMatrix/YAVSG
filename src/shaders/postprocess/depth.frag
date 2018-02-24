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
    // color_out = vec4( depth, depth, depth, 1.0 );
    
    if( depth < 0 )
        color_out = vec4( 0.0, 0.0, 1.0, 1.0 );
    else if( depth > 1 )
        color_out = vec4( 1.0, 0.0, 0.0, 1.0 );
    else if( depth == 0 )
        color_out = vec4( 0.0, 0.0, 0.0, 1.0 );
    else if( depth == 1 )
        color_out = vec4( 1.0, 1.0, 1.0, 1.0 );
    else
        color_out = vec4( 0.0, 1.0, 0.0, 1.0 );
}
