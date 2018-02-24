#version 150 core


in vec2 texture_coord;

out vec4 color_out;

uniform sampler2D framebuffer;


void main()
{
    color_out = texture( framebuffer, texture_coord );
    
    // // Unweighted
    // float average = (
    //       color_out.r
    //     + color_out.g
    //     + color_out.b
    // ) / 3.0;
    
    // Weighted:
    float average = (
          color_out.r * 0.2126
        + color_out.g * 0.7152
        + color_out.b * 0.0722
    );
    
    color_out = vec4( vec3( average ), 1.0 );
}
