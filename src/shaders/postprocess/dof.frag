#version 150 core


in vec2 texture_coord;

out vec4 color_out;

uniform sampler2D framebuffer;
uniform sampler2D framebuffer_depth_stencil;
uniform float view_width;
uniform float half_height;


void main()
{
    float depth = texture( framebuffer_depth_stencil, texture_coord ).r;
    
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
    
    color_out = vec4( 0.0 );
    float divisor = 0.0;
    
    for( int x = -radius; x <= radius; ++x )
        for( int y = -radius; y <= radius; ++y )
        {
            float sample_depth = texture(
                framebuffer_depth_stencil,
                vec2(
                    texture_coord.x + x * blur_size_h,
                    texture_coord.y + y * blur_size_v
                )
            ).r;
            
            float weight = 1.0 + ( sample_depth - depth );
            
            color_out += texture(
                framebuffer,
                vec2(
                    texture_coord.x + x * blur_size_h,
                    texture_coord.y + y * blur_size_v
                )
            ) * weight;
            divisor += weight;
        }
    
    color_out = color_out / divisor;
}


// void main()
// {
//     float depth = texture( framebuffer_depth_stencil, texture_coord ).r;
    
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
    
//     color_out = vec4( 0.0 );
//     float divisor = 0.0;
    
//     for( int x = -radius; x <= radius; ++x )
//         for( int y = -radius; y <= radius; ++y )
//         {
//             color_out += texture(
//                 framebuffer,
//                 vec2(
//                     texture_coord.x + x * blur_size_h,
//                     texture_coord.y + y * blur_size_v
//                 )
//             );
//             divisor += 1.0;
//         }
    
//     color_out = color_out / divisor;
// }
