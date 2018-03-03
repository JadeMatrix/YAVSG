#version 150 core


in vec3 color;
in vec2 texture_coord;

out vec4 color_out;

uniform sampler2D color_map;
uniform sampler2D normal_map;
uniform sampler2D specular_map;

uniform vec3 tint;

uniform float time_absolute;
uniform float time_delta;


// (Theoretically) vendor-independent check for invalid normal texture
bool valid_normal( vec3 normal )
{
    float length = length( normal );
    return (
           !isinf( length )
        && !isnan( length )
        && length != 0.0
    );
}


void main()
{
    color_out = texture(
        color_map,
        vec2(
                  texture_coord.x,
            1.0 - texture_coord.y
        )
    );
    
    vec3 normal = texture(
        normal_map,
        vec2(
                  texture_coord.x,
            1.0 - texture_coord.y
        )
    ).xyz;
    if( !valid_normal( normal ) )
        normal = vec3( 0.5, 0.5, 1.0 );
    
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
