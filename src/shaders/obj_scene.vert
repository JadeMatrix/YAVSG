#version 150 core


in vec3 position;
in vec3 color_in;
in vec2 texture_coord_in;

out vec3 color;
out vec2 texture_coord;

uniform mat4  transform_model;
uniform mat4  transform_view;
uniform mat4  transform_projection;
uniform float time_absolute;
uniform float time_delta;


void main()
{
    gl_Position = (
          transform_projection
        * transform_view
        // * rotation
        * transform_model
        * vec4(
            position.x,
            position.y * -1.0,
            position.z,
            1.0
        )
    );
    
    color = color_in;
    texture_coord = texture_coord_in;
}
