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

uniform struct
{
    float near;
    float focal;
    float far;
} camera_point;

////////////////////////////////////////////////////////////////////////////////

float linear_depth( float depth )
{
    depth = 2 * depth - 1;
    return (
        2 * camera_point.near * camera_point.far / (
              camera_point.far
            + camera_point.near
            - depth * ( camera_point.far - camera_point.near )
        )
    );
}

void main()
{
    float depth = texture( framebuffer_source_depth, fragment_in.texture ).r;
    
    float linearized_depth = linear_depth( depth );
    depth -= camera_point.focal / linearized_depth;
    if( depth < 0 )
        depth = 0;
    
    float blur_size_h = depth / 300.0;
    float blur_size_v = depth / 200.0;
    
    fragment_out_color = vec4( 0.0 );
    float divisor = 0.0;
    
    for( int x = -4; x <= 4; ++x )
        for( int y = -4; y <= 4; ++y )
        {
            float sample_depth = texture(
                framebuffer_source_depth,
                vec2(
                    fragment_in.texture.x + x * blur_size_h,
                    fragment_in.texture.y + y * blur_size_v
                )
            ).r;
            
            // float weight = 1.0 + ( sample_depth - depth );
            float weight = 1.0;
            if( depth > sample_depth )
                weight = 0.0;
            
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
