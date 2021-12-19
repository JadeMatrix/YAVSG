#pragma once


#include <string>


namespace JadeMatrix::yavsg
{
    // Standard declarations in shaders
#if 0
    in vec3 vertex_in_position;
    in vec3 vertex_in_normal;
    in vec3 vertex_in_tangent;
    in vec3 vertex_in_color;
    in vec2 vertex_in_texture;
    
    uniform TRANSFORM
    {
        mat4 model;
        mat4 view;
        mat4 projection;
    } transform;
    
    out VERTEX_OUT
    {
        vec3 position;
        vec3 color;
        vec2 texture;
        mat4 TBN_matrix;
    } vertex_out;
    in VERTEX_OUT
    {
        vec3 position;
        vec3 color;
        vec2 texture;
        mat4 TBN_matrix;
    } fragment_in;
    
    uniform struct
    {
        sampler2D color;
        sampler2D normal;
        sampler2D specular;
        sampler2D mask;
    } map;
    
    uniform struct
    {
        float near;
        float focal;
        float far;
    } camera_point;
    
    uniform sampler2D framebuffer_source_color;
    uniform sampler2D framebuffer_source_depth;
    uniform struct
    {
        float width;
        float height;
    } framebuffer_target;
    
    out vec4 fragment_out_color;
#endif
    
    enum class shader_string_id
    {
        vertex_in_position,
        vertex_in_normal,
        vertex_in_tangent,
        vertex_in_color,
        vertex_in_texture,
        
        transform_model,
        transform_view,
        transform_projection,
        
        vertex_out_position,
        vertex_out_color,
        vertex_out_texture,
        vertex_out_tbn_matrix,
        fragment_in_position,
        fragment_in_color,
        fragment_in_texture,
        fragment_in_tbn_matrix,
        
        map_color,
        map_normal,
        map_specular,
        
        camera_point_near,
        camera_point_focal,
        camera_point_far,
        
        framebuffer_source_color,
        framebuffer_source_depth,
        framebuffer_target_width,
        framebuffer_target_height,
        
        fragment_out_color
    };
    
    const std::string& shader_string( shader_string_id );
}
