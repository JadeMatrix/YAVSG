#version 150 core


in vec3 color;
in vec2 texture_coord;

out vec4 color_out;

uniform bool      has_color_map;
uniform sampler2D     color_map;
uniform bool      has_normal_map;
uniform sampler2D     normal_map;
uniform bool      has_specular_map;
uniform sampler2D     specular_map;
uniform bool      has_mask_map;
uniform sampler2D     mask_map;

uniform vec3 tint;

uniform float time_absolute;
uniform float time_delta;


void main()
{
    if(
        has_color_map
        // has_normal_map
        // has_specular_map
        // has_mask_map
    )
        color_out = texture(
            color_map,
            // normal_map,
            // specular_map,
            // mask_map,
            vec2(
                      texture_coord.x,
                1.0 - texture_coord.y
            )
        );
    else
        color_out = vec4( 0.5, 0.5, 0.5, 1.0 );
    
    // if( has_specular_map )
    //     color_out += texture(
    //         specular_map,
    //         vec2(
    //                   texture_coord.x,
    //             1.0 - texture_coord.y
    //         )
    //     );
    
    color_out *= vec4( color, 1.0 );
    
    color_out *= vec4( tint,  1.0 );
}
