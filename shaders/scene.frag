#version 150 core

// Input ///////////////////////////////////////////////////////////////////////

in VERTEX_OUT
{
    vec3 position;  // position XYZ
    vec3 color;     // color    RGB
    vec2 texture;   // texture  UV
    
    mat4 TBN_matrix;
} fragment_in;

uniform HAS_MAP
{
    bool color;
    bool normal;
    bool specular;
    bool mask;
} has_map;

uniform MAP
{
    sampler2D color;
    sampler2D normal;
    sampler2D specular;
    sampler2D mask;
} map;

// Output //////////////////////////////////////////////////////////////////////

out vec4 fragment_out_color;

////////////////////////////////////////////////////////////////////////////////

void main()
{
    // Basic fragment color
    if( has_map.color )
        fragment_out_color = texture(
            map.color,
            vec2(
                      fragment_in.texture.x,
                1.0 - fragment_in.texture.y
            )
        );
    else
        fragment_out_color = vec4( 0.5, 0.5, 0.5, 1.0 );
    
    // // Fragment normal
    // vec3 normal;
    // if( has_map.normal )
    //     normal = texture(
    //         map.normal,
    //         vec2(
    //                   fragment_in.texture.x,
    //             1.0 - fragment_in.texture.y
    //         )
    //     );
    // else
    //     normal = vec3( 0.5, 0.5, 1.0 );
    // normal = normalize( normal );
    
    // // Apply lighting;
}
