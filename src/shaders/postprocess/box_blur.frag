#version 150 core


in vec2 texture_coord;

out vec4 color_out;

uniform sampler2D framebuffer;


void main()
{
    float blur_size_h = 1.0 / 300.0;
    float blur_size_v = 1.0 / 200.0;
    color_out = vec4( 0.0 );
    
    for( int x = -4; x <= 4; ++x )
        for( int y = -4; y <= 4; ++y )
        // {
        //     vec4 sample = texture(
        //         framebuffer,
        //         vec2(
        //             texture_coord.x + x * blur_size_h,
        //             texture_coord.y + y * blur_size_v
        //         )
        //     ) / 81.0;
            
        //     color_out += vec4(
        //         color_out.r * 0.2126,
        //         color_out.g * 0.7152,
        //         color_out.b * 0.0722,
        //         color_out.w
        //     );
        // }
            color_out += texture(
                framebuffer,
                vec2(
                    texture_coord.x + x * blur_size_h,
                    texture_coord.y + y * blur_size_v
                )
            ) / 81.0;
}
