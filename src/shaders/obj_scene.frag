#version 150 core

// Input ///////////////////////////////////////////////////////////////////////

in VERTEX_OUT
{
    // vec3 position;  // position XYZ
    vec3 color;     // color    RGB
    vec2 texture;   // texture  UV
    // mat4 TBN_matrix;
} fragment_in;

uniform sampler2D map_color;
// uniform sampler2D map_normal;
// uniform sampler2D map_specular;

// Output //////////////////////////////////////////////////////////////////////

out vec4 fragment_out_color;

////////////////////////////////////////////////////////////////////////////////

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
    fragment_out_color = texture(
        map_color,
        vec2(
                  fragment_in.texture.x,
            1.0 - fragment_in.texture.y
        )
    );
    
    // vec3 normal = texture(
    //     map_normal,
    //     vec2(
    //               fragment_in.texture.x,
    //         1.0 - fragment_in.texture.y
    //     )
    // ).xyz;
    // if( !valid_normal( normal ) )
    //     normal = vec3( 0.5, 0.5, 1.0 );
    
    // fragment_out_color *= vec4( fragment_in.color, 1.0 );
    
    // // if( has_map_specular )
    // //     fragment_out_color += texture(
    // //         map_specular,
    // //         vec2(
    // //                   fragment_in.texture.x,
    // //             1.0 - fragment_in.texture.y
    // //         )
    // //     );
}
