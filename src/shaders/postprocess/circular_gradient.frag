void circular_gradient()
{
    vec4 gradient_color = vec4( 0.0, 0.0, 0.0, 1.0 );
    
    vec2 norm_screen_coord = vec2(
        ( gl_FragCoord.x - ( framebuffer_target.width  / 2 ) ) / framebuffer_target.width,
        ( gl_FragCoord.y - ( framebuffer_target.height / 2 ) ) / framebuffer_target.height
    );
    float gradient_intensity = gradient_color.a * sqrt(
          norm_screen_coord.x * norm_screen_coord.x
        + norm_screen_coord.y * norm_screen_coord.y
    );
    fragment_out_color = vec4(
        (
                  gradient_color.rgb *       gradient_intensity
            + fragment_out_color.rgb * ( 1 - gradient_intensity )
        ),
        fragment_out_color.a
    );
}
