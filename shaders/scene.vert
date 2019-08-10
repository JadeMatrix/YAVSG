#version 150 core

// Input ///////////////////////////////////////////////////////////////////////

in vec3 vertex_in_position; // position XYZ
in vec3 vertex_in_normal;   // normal   XYZ
in vec3 vertex_in_tangent;  // tangent  XYZ
in vec3 vertex_in_color;    // color    RGB
in vec2 vertex_in_texture;  // texture  UV

uniform TRANSFORM
{
    mat4 model;
    mat4 view;
    mat4 projection;
} transform;

// Output //////////////////////////////////////////////////////////////////////

out VERTEX_OUT
{
    vec3 position; // position XYZ
    vec3 color;    // color    RGB
    vec2 texture;  // texture  UV
    
    mat4 TBN_matrix;
} vertex_out;

////////////////////////////////////////////////////////////////////////////////

void main()
{
    mat4 transform = (
          transform.projection
        * transform.view
        * transform.model
    );
    
    vec4 adjusted_position = vec4(
        vertex_in_position.x,
        vertex_in_position.y * -1.0,
        vertex_in_position.z,
        1.0
    );
    
    gl_Position = transform * adjusted_position;
    
    vertex_out.color   = vertex_in_color;
    vertex_out.texture = vertex_in_texture;
    
    vertex_out.position = adjusted_position.xyz;
    
    vec3 N = normalize( vec3( transform.model * vec4(  vertex_in_normal, 0.0 ) ) );
    vec3 T = normalize( vec3( transform.model * vec4( vertex_in_tangent, 0.0 ) ) );
    vec3 B = cross( N, T );
    vertex_out.TBN_matrix = transpose( mat3( T, B, N ) );
}
