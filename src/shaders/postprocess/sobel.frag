#version 150 core


in vec2 texture_coord;

out vec4 color_out;

uniform sampler2D framebuffer;
uniform float view_width;
uniform float half_height;


void main()
{
    // float half_width  = view_width / 2;
    // float half_height = view_height / 2;
    
    vec4 top          = texture( framebuffer, vec2( texture_coord.x              , texture_coord.y + 1.0 / 200.0 ) );
    vec4 bottom       = texture( framebuffer, vec2( texture_coord.x              , texture_coord.y - 1.0 / 200.0 ) );
    vec4 left         = texture( framebuffer, vec2( texture_coord.x - 1.0 / 300.0, texture_coord.y               ) );
    vec4 right        = texture( framebuffer, vec2( texture_coord.x + 1.0 / 300.0, texture_coord.y               ) );
    vec4 top_left     = texture( framebuffer, vec2( texture_coord.x - 1.0 / 300.0, texture_coord.y + 1.0 / 200.0 ) );
    vec4 top_right    = texture( framebuffer, vec2( texture_coord.x + 1.0 / 300.0, texture_coord.y + 1.0 / 200.0 ) );
    vec4 bottom_left  = texture( framebuffer, vec2( texture_coord.x - 1.0 / 300.0, texture_coord.y - 1.0 / 200.0 ) );
    vec4 bottom_right = texture( framebuffer, vec2( texture_coord.x + 1.0 / 300.0, texture_coord.y - 1.0 / 200.0 ) );
    vec4 sx = -top_left - 2 * left - bottom_left + top_right   + 2 * right  + bottom_right;
    vec4 sy = -top_left - 2 * top  - top_right   + bottom_left + 2 * bottom + bottom_right;
    color_out = sqrt( sx * sx + sy * sy );
}
