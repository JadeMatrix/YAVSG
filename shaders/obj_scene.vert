#version 150 core

// Input ///////////////////////////////////////////////////////////////////////

in vec3 vertex_in_position; // position XYZ
// in vec3 vertex_in_normal;   // normal   XYZ
// in vec3 vertex_in_tangent;  // tangent  XYZ
in vec3 vertex_in_color;    // color    RGB
in vec2 vertex_in_texture;  // texture  UV

uniform struct
{
    mat4 model;
    mat4 view;
    mat4 projection;
} transform;

// Output //////////////////////////////////////////////////////////////////////

out VERTEX_OUT
{
    // vec3 position; // position XYZ
    vec3 color;    // color    RGB
    vec2 texture;  // texture  UV
    // mat4 TBN_matrix;
} vertex_out;

////////////////////////////////////////////////////////////////////////////////

void main()
{
    gl_Position = (
          transform.projection
        * transform.view
        * transform.model
        * vec4(
            vertex_in_position.x,
            vertex_in_position.y * -1.0,
            vertex_in_position.z,
            1.0
        )
    );
    
    // vertex_out.position = gl_Position.xyz;
    vertex_out.color    = vertex_in_color;
    vertex_out.texture  = vertex_in_texture;
}
