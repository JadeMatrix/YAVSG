void buffer_analysis()
{
    float alpha = texture(
        framebuffer_source_color,
        fragment_in.texture
    ).a;
    float depth = texture(
        framebuffer_source_depth,
        fragment_in.texture
    ).r;
    
    fragment_out_color = vec4( alpha, depth, 1.0, 1.0 );
}
