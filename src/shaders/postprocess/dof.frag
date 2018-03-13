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
uniform sampler2D framebuffer_source_depth;

void main()
{
    float depth = texture( framebuffer_source_depth, fragment_in.texture ).r;
    
    float blur_size_h = depth / 300.0;
    float blur_size_v = depth / 200.0;
    
    fragment_out_color = vec4( 0.0 );
    float divisor = 0.0;
    
    for( int x = -4; x <= 4; ++x )
        for( int y = -4; y <= 4; ++y )
        {
            fragment_out_color += texture(
                framebuffer_source_color,
                vec2(
                    fragment_in.texture.x + x * blur_size_h,
                    fragment_in.texture.y + y * blur_size_v
                )
            );
            divisor += 1.0;
        }
    
    fragment_out_color = fragment_out_color / divisor;
}

