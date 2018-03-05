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
uniform struct
{
    float width;
    float height;
} framebuffer_target;

void main()
{
    vec2 norm_screen_coord = vec2(
        ( gl_FragCoord.x - ( framebuffer_target.width  / 2 ) ) / framebuffer_target.width,
        ( gl_FragCoord.y - ( framebuffer_target.height / 2 ) ) / framebuffer_target.height
    );
    fragment_out_color = texture(
        framebuffer_source_color,
        fragment_in.texture
    ) * ( 1 - sqrt(
          norm_screen_coord.x * norm_screen_coord.x
        + norm_screen_coord.y * norm_screen_coord.y
    ) );
}
