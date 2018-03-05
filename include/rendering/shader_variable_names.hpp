#pragma once
#ifndef YAVSG_RENDERING_SHADER_VARIABLE_NAMES_HPP
#define YAVSG_RENDERING_SHADER_VARIABLE_NAMES_HPP


#include <string>


namespace yavsg
{
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
    
    uniform MAP
    {
        sampler2D color;
        sampler2D normal;
        sampler2D specular;
        sampler2D mask;
    } map;
    
    out vec4 fragment_out_color;
    
    #endif
    
    namespace shader_strings
    {
        const std::string& vertex_in_position();
        const std::string& vertex_in_normal  ();
        const std::string& vertex_in_tangent ();
        const std::string& vertex_in_color   ();
        const std::string& vertex_in_texture ();
        
        const std::string& transform_model     ();
        const std::string& transform_view      ();
        const std::string& transform_projection();
        
        const std::string& vertex_out_position  ();
        const std::string& vertex_out_color     ();
        const std::string& vertex_out_texture   ();
        const std::string& vertex_out_TBN_matrix();
        const std::string& fragment_in_position  ();
        const std::string& fragment_in_color     ();
        const std::string& fragment_in_texture   ();
        const std::string& fragment_in_TBN_matrix();
        
        const std::string& map_color   ();
        const std::string& map_normal  ();
        const std::string& map_specular();
        
        const std::string& fragment_out_color();
    }
}


#endif
