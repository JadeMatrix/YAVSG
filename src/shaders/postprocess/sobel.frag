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
// uniform float view_width;
// uniform float view_height;

void main()
{
    // float half_width  = view_width / 2;
    // float half_height = view_height / 2;
    
    vec4 top          = texture( framebuffer, vec2( fragment_in.texture.x              , fragment_in.texture.y + 1.0 / 200.0 ) );
    vec4 bottom       = texture( framebuffer, vec2( fragment_in.texture.x              , fragment_in.texture.y - 1.0 / 200.0 ) );
    vec4 left         = texture( framebuffer, vec2( fragment_in.texture.x - 1.0 / 300.0, fragment_in.texture.y               ) );
    vec4 right        = texture( framebuffer, vec2( fragment_in.texture.x + 1.0 / 300.0, fragment_in.texture.y               ) );
    vec4 top_left     = texture( framebuffer, vec2( fragment_in.texture.x - 1.0 / 300.0, fragment_in.texture.y + 1.0 / 200.0 ) );
    vec4 top_right    = texture( framebuffer, vec2( fragment_in.texture.x + 1.0 / 300.0, fragment_in.texture.y + 1.0 / 200.0 ) );
    vec4 bottom_left  = texture( framebuffer, vec2( fragment_in.texture.x - 1.0 / 300.0, fragment_in.texture.y - 1.0 / 200.0 ) );
    vec4 bottom_right = texture( framebuffer, vec2( fragment_in.texture.x + 1.0 / 300.0, fragment_in.texture.y - 1.0 / 200.0 ) );
    vec4 sx = -top_left - 2 * left - bottom_left + top_right   + 2 * right  + bottom_right;
    vec4 sy = -top_left - 2 * top  - top_right   + bottom_left + 2 * bottom + bottom_right;
    fragment_out_color = sqrt( sx * sx + sy * sy );
}
