#include <yavsg/rendering/shader_variable_names.hpp>


namespace
{
    std::string vertex_in_position_str        = "vertex_in_position";
    std::string vertex_in_normal_str          = "vertex_in_normal";
    std::string vertex_in_tangent_str         = "vertex_in_tangent";
    std::string vertex_in_color_str           = "vertex_in_color";
    std::string vertex_in_texture_str         = "vertex_in_texture";
    
    std::string transform_model_str           = "transform.model";
    std::string transform_view_str            = "transform.view";
    std::string transform_projection_str      = "transform.projection";
    
    std::string position_str                  = "position";
    std::string color_str                     = "color";
    std::string texture_str                   = "texture";
    std::string TBN_matrix_str                = "TBN_matrix";
    std::string vertex_out_position_str       =  "vertex_out." + position_str;
    std::string vertex_out_color_str          =  "vertex_out." + color_str;
    std::string vertex_out_texture_str        =  "vertex_out." + texture_str;
    std::string vertex_out_TBN_matrix_str     =  "vertex_out." + TBN_matrix_str;
    std::string fragment_in_position_str      = "fragment_in." + position_str;
    std::string fragment_in_color_str         = "fragment_in." + color_str;
    std::string fragment_in_texture_str       = "fragment_in." + texture_str;
    std::string fragment_in_TBN_matrix_str    = "fragment_in." + TBN_matrix_str;
    
    std::string map_color_str                 = "map_color";
    std::string map_normal_str                = "map_normal";
    std::string map_specular_str              = "map_specular";
    
    std::string camera_point_near_str         = "camera_point.near";
    std::string camera_point_focal_str        = "camera_point.focal";
    std::string camera_point_far_str          = "camera_point.far";
    
    std::string framebuffer_source_color_str  = "framebuffer_source_color";
    std::string framebuffer_source_depth_str  = "framebuffer_source_depth";
    std::string framebuffer_target_width_str  = "framebuffer_target.width";
    std::string framebuffer_target_height_str = "framebuffer_target.height";
    
    std::string fragment_out_color_str        = "fragment_out_color";
}


namespace JadeMatrix::yavsg
{
    const std::string& shader_string( shader_string_id id )
    {
        switch( id )
        {
        case shader_string_id::VERTEX_IN_POSITION:
            return vertex_in_position_str;
        case shader_string_id::VERTEX_IN_NORMAL:
            return vertex_in_normal_str;
        case shader_string_id::VERTEX_IN_TANGENT:
            return vertex_in_tangent_str;
        case shader_string_id::VERTEX_IN_COLOR:
            return vertex_in_color_str;
        case shader_string_id::VERTEX_IN_TEXTURE:
            return vertex_in_texture_str;
        
        case shader_string_id::TRANSFORM_MODEL:
            return transform_model_str;
        case shader_string_id::TRANSFORM_VIEW:
            return transform_view_str;
        case shader_string_id::TRANSFORM_PROJECTION:
            return transform_projection_str;
        
        case shader_string_id::VERTEX_OUT_POSITION:
            return vertex_out_position_str;
        case shader_string_id::VERTEX_OUT_COLOR:
            return vertex_out_color_str;
        case shader_string_id::VERTEX_OUT_TEXTURE:
            return vertex_out_texture_str;
        case shader_string_id::VERTEX_OUT_TBN_MATRIX:
            return vertex_out_TBN_matrix_str;
        case shader_string_id::FRAGMENT_IN_POSITION:
            return fragment_in_position_str;
        case shader_string_id::FRAGMENT_IN_COLOR:
            return fragment_in_color_str;
        case shader_string_id::FRAGMENT_IN_TEXTURE:
            return fragment_in_texture_str;
        case shader_string_id::FRAGMENT_IN_TBN_MATRIX:
            return fragment_in_TBN_matrix_str;
        
        case shader_string_id::MAP_COLOR:
            return map_color_str;
        case shader_string_id::MAP_NORMAL:
            return map_normal_str;
        case shader_string_id::MAP_SPECULAR:
            return map_specular_str;
        
        case shader_string_id::CAMERA_POINT_NEAR:
            return camera_point_near_str;
        case shader_string_id::CAMERA_POINT_FOCAL:
            return camera_point_focal_str;
        case shader_string_id::CAMERA_POINT_FAR:
            return camera_point_far_str;
        
        case shader_string_id::FRAMEBUFFER_SOURCE_COLOR:
            return framebuffer_source_color_str;
        case shader_string_id::FRAMEBUFFER_SOURCE_DEPTH:
            return framebuffer_source_depth_str;
        case shader_string_id::FRAMEBUFFER_TARGET_WIDTH:
            return framebuffer_target_width_str;
        case shader_string_id::FRAMEBUFFER_TARGET_HEIGHT:
            return framebuffer_target_height_str;
        
        case shader_string_id::FRAGMENT_OUT_COLOR:
            return fragment_out_color_str;
        }
    }
}
