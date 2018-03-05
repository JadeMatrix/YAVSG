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
uniform float view_width;
uniform float view_height;

void main()
{
    vec2 norm_screen_coord = vec2(
        ( gl_FragCoord.x - ( view_width  / 2 ) ) / view_width,
        ( gl_FragCoord.y - ( view_height / 2 ) ) / view_height
    );
    fragment_out_color = texture( framebuffer, fragment_in.texture ) * ( 1 - sqrt(
          norm_screen_coord.x * norm_screen_coord.x
        + norm_screen_coord.y * norm_screen_coord.y
    ) );
}
