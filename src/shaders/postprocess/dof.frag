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
uniform sampler2D framebuffer_source_depth_stencil;

void main()
{
    float depth = texture(
        framebuffer_source_depth_stencil,
        fragment_in.texture
    ).r;
    
    int radius;
    if( depth >= 0.0 && depth < 0.2 )
        radius = 0;
    else if( depth >= 0.2 && depth < 0.4 )
        radius = 1;
    else if( depth >= 0.4 && depth < 0.6 )
        radius = 2;
    else if( depth >= 0.6 && depth < 0.8 )
        radius = 3;
    else if( depth >= 0.8 && depth <= 1.0 )
        radius = 4;
    
    float blur_size_h = 1.0 / 300.0;
    float blur_size_v = 1.0 / 200.0;
    
    fragment_out_color = vec4( 0.0 );
    float divisor = 0.0;
    
    for( int x = -radius; x <= radius; ++x )
        for( int y = -radius; y <= radius; ++y )
        {
            float sample_depth = texture(
                framebuffer_source_depth_stencil,
                vec2(
                    fragment_in.texture.x + x * blur_size_h,
                    fragment_in.texture.y + y * blur_size_v
                )
            ).r;
            
            float weight = 1.0 + ( sample_depth - depth );
            
            fragment_out_color += texture(
                framebuffer_source_color,
                vec2(
                    fragment_in.texture.x + x * blur_size_h,
                    fragment_in.texture.y + y * blur_size_v
                )
            ) * weight;
            divisor += weight;
        }
    
    fragment_out_color = fragment_out_color / divisor;
}


// void main()
// {
//     float depth = texture(
//         framebuffer_source_depth_stencil,
//         fragment_in.texture
//     ).r;
    
//     int radius;
//     if( depth >= 0.0 && depth < 0.2 )
//         radius = 0;
//     else if( depth >= 0.2 && depth < 0.4 )
//         radius = 1;
//     else if( depth >= 0.4 && depth < 0.6 )
//         radius = 2;
//     else if( depth >= 0.6 && depth < 0.8 )
//         radius = 3;
//     else if( depth >= 0.8 && depth <= 1.0 )
//         radius = 4;
    
//     float blur_size_h = 1.0 / 300.0;
//     float blur_size_v = 1.0 / 200.0;
    
//     fragment_out_color = vec4( 0.0 );
//     float divisor = 0.0;
    
//     for( int x = -radius; x <= radius; ++x )
//         for( int y = -radius; y <= radius; ++y )
//         {
//             fragment_out_color += texture(
//                 framebuffer_source_color,
//                 vec2(
//                     fragment_in.texture.x + x * blur_size_h,
//                     fragment_in.texture.y + y * blur_size_v
//                 )
//             );
//             divisor += 1.0;
//         }
    
//     fragment_out_color = fragment_out_color / divisor;
// }
