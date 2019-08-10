void depth()
{
    float depth = texture(
        framebuffer_source_depth,
        fragment_in.texture
    ).r;
    
    fragment_out_color = vec4( depth, depth, depth, 1.0 );
}
