#version 150 core


in vec3 color;
in vec2 texture_coord;

out vec4 color_out;

uniform sampler2D color_map;
uniform vec3 tint;

uniform float time_absolute;
uniform float time_delta;


void main()
{
    color_out = texture(
        color_map,
        vec2(
                  texture_coord.x,
            1.0 - texture_coord.y
        )
    );
    // color_out = vec4( 1.0, 1.0, 1.0, 1.0 );
    
    color_out *= vec4( color, 1.0 );
    
    color_out *= vec4( tint,  1.0 );
}
