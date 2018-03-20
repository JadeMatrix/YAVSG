// Multi-sample, needs to be run as the first function in a multi-function
// postprocess step
void sobel()
{
    // float half_width  = framebuffer_target.width  / 2;
    // float half_height = framebuffer_target.height / 2;
    
    vec4 top          = texture( framebuffer_source_color, vec2( fragment_in.texture.x              , fragment_in.texture.y + 1.0 / 200.0 ) );
    vec4 bottom       = texture( framebuffer_source_color, vec2( fragment_in.texture.x              , fragment_in.texture.y - 1.0 / 200.0 ) );
    vec4 left         = texture( framebuffer_source_color, vec2( fragment_in.texture.x - 1.0 / 300.0, fragment_in.texture.y               ) );
    vec4 right        = texture( framebuffer_source_color, vec2( fragment_in.texture.x + 1.0 / 300.0, fragment_in.texture.y               ) );
    vec4 top_left     = texture( framebuffer_source_color, vec2( fragment_in.texture.x - 1.0 / 300.0, fragment_in.texture.y + 1.0 / 200.0 ) );
    vec4 top_right    = texture( framebuffer_source_color, vec2( fragment_in.texture.x + 1.0 / 300.0, fragment_in.texture.y + 1.0 / 200.0 ) );
    vec4 bottom_left  = texture( framebuffer_source_color, vec2( fragment_in.texture.x - 1.0 / 300.0, fragment_in.texture.y - 1.0 / 200.0 ) );
    vec4 bottom_right = texture( framebuffer_source_color, vec2( fragment_in.texture.x + 1.0 / 300.0, fragment_in.texture.y - 1.0 / 200.0 ) );
    vec4 sx = -top_left - 2 * left - bottom_left + top_right   + 2 * right  + bottom_right;
    vec4 sy = -top_left - 2 * top  - top_right   + bottom_left + 2 * bottom + bottom_right;
    fragment_out_color = sqrt( sx * sx + sy * sy );
}
