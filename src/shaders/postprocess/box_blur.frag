// Multi-sample, needs to be run as the first function in a multi-function
// postprocess step
void box_blur()
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
            
        //     blurred_color += vec4(
        //         blurred_color.r * 0.2126,
        //         blurred_color.g * 0.7152,
        //         blurred_color.b * 0.0722,
        //         blurred_color.w
        //     );
        // }
            blurred_color += texture(
                framebuffer_source_color,
                vec2(
                    fragment_in.texture.x + x * blur_size_h,
                    fragment_in.texture.y + y * blur_size_v
                )
            ) / 81.0;
}
