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
    
    enum class shader_string_id
    {
        VERTEX_IN_POSITION,
        VERTEX_IN_NORMAL,
        VERTEX_IN_TANGENT,
        VERTEX_IN_COLOR,
        VERTEX_IN_TEXTURE,
        
        TRANSFORM_MODEL,
        TRANSFORM_VIEW,
        TRANSFORM_PROJECTION,
        
        VERTEX_OUT_POSITION,
        VERTEX_OUT_COLOR,
        VERTEX_OUT_TEXTURE,
        VERTEX_OUT_TBN_MATRIX,
        FRAGMENT_IN_POSITION,
        FRAGMENT_IN_COLOR,
        FRAGMENT_IN_TEXTURE,
        FRAGMENT_IN_TBN_MATRIX,
        
        MAP_COLOR,
        MAP_NORMAL,
        MAP_SPECULAR,
        
        FRAGMENT_OUT_COLOR
    };
    
    const std::string& shader_string( shader_string_id );
}


#endif
