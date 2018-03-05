#version 150 core

// Input ///////////////////////////////////////////////////////////////////////

in VERTEX_OUT
{
    vec2 texture;
} fragment_in;

// Output //////////////////////////////////////////////////////////////////////

out vec4 fragment_out_color;

////////////////////////////////////////////////////////////////////////////////

uniform sampler2D framebuffer;

void main()
{
    fragment_out_color = texture( framebuffer, fragment_in.texture );
    
    // // Unweighted
    // float average = (
    //       fragment_out_color.r
    //     + fragment_out_color.g
    //     + fragment_out_color.b
    // ) / 3.0;
    
    // Weighted:
    float average = (
          fragment_out_color.r * 0.2126
        + fragment_out_color.g * 0.7152
        + fragment_out_color.b * 0.0722
    );
    
    fragment_out_color = vec4( vec3( average ), 1.0 );
}
