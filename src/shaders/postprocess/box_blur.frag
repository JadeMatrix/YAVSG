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

void main()
{
    float blur_size_h = 1.0 / 300.0;
    float blur_size_v = 1.0 / 200.0;
    fragment_out_color = vec4( 0.0 );
    
    for( int x = -4; x <= 4; ++x )
        for( int y = -4; y <= 4; ++y )
        // {
        //     vec4 sample = texture(
        //         framebuffer_source_color,
        //         vec2(
        //             fragment_in.texture.x + x * blur_size_h,
        //             fragment_in.texture.y + y * blur_size_v
        //         )
        //     ) / 81.0;
            
        //     fragment_out_color += vec4(
        //         fragment_out_color.r * 0.2126,
        //         fragment_out_color.g * 0.7152,
        //         fragment_out_color.b * 0.0722,
        //         fragment_out_color.w
        //     );
        // }
            fragment_out_color += texture(
                framebuffer_source_color,
                vec2(
                    fragment_in.texture.x + x * blur_size_h,
                    fragment_in.texture.y + y * blur_size_v
                )
            ) / 81.0;
}
