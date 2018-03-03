#version 150 core


in vec2 texture_coord;

out vec4 color_out;

uniform sampler2D framebuffer;


void main()
{
    color_out = texture( framebuffer, texture_coord );
}
