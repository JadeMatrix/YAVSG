#version 150 core

// Input ///////////////////////////////////////////////////////////////////////

in vec3 vertex_in_position;
in vec2 vertex_in_texture;

// Output //////////////////////////////////////////////////////////////////////

out VERTEX_OUT
{
    vec2 texture;
} vertex_out;

////////////////////////////////////////////////////////////////////////////////

void main()
{
    vertex_out.texture = vertex_in_texture;
    gl_Position = vec4(
        vertex_in_position.x,
        vertex_in_position.y * -1.0,
        0.0,
        1.0
    );
}
