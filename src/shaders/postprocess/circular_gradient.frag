#version 150 core


in vec2 texture_coord;

out vec4 color_out;

uniform sampler2D framebuffer;
uniform float view_width;
uniform float view_height;


void main()
{
    vec2 norm_screen_coord = vec2(
        ( gl_FragCoord.x - ( view_width  / 2 ) ) / view_width,
        ( gl_FragCoord.y - ( view_height / 2 ) ) / view_height
    );
    color_out = texture( framebuffer, texture_coord ) * ( 1 - sqrt(
          norm_screen_coord.x * norm_screen_coord.x
        + norm_screen_coord.y * norm_screen_coord.y
    ) );
}
