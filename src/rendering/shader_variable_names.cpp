#include "../../include/rendering/shader_variable_names.hpp"


namespace
{
    std::string vertex_in_position_str     = "vertex_in_position";
    std::string vertex_in_normal_str       = "vertex_in_normal";
    std::string vertex_in_tangent_str      = "vertex_in_tangent";
    std::string vertex_in_color_str        = "vertex_in_color";
    std::string vertex_in_texture_str      = "vertex_in_texture";
    
    std::string transform_model_str        = "transform.model";
    std::string transform_view_str         = "transform.view";
    std::string transform_projection_str   = "transform.projection";
    
    std::string position_str               = "position";
    std::string color_str                  = "color";
    std::string texture_str                = "texture";
    std::string TBN_matrix_str             = "TBN_matrix";
    std::string vertex_out_position_str    =  "vertex_out." + position_str;
    std::string vertex_out_color_str       =  "vertex_out." + color_str;
    std::string vertex_out_texture_str     =  "vertex_out." + texture_str;
    std::string vertex_out_TBN_matrix_str  =  "vertex_out." + TBN_matrix_str;
    std::string fragment_in_position_str   = "fragment_in." + position_str;
    std::string fragment_in_color_str      = "fragment_in." + color_str;
    std::string fragment_in_texture_str    = "fragment_in." + texture_str;
    std::string fragment_in_TBN_matrix_str = "fragment_in." + TBN_matrix_str;
    
    std::string map_color_str              = "map.color";
    std::string map_normal_str             = "map.normal";
    std::string map_specular_str           = "map.specular";
    
    std::string fragment_out_color_str     = "fragment_out_color";
}


namespace yavsg // Vertex in attributes ////////////////////////////////////////
{
    namespace shader_strings
    {
        const std::string& vertex_in_position()
        {
            return vertex_in_position_str;
        }
        
        const std::string& vertex_in_normal()
        {
            return vertex_in_normal_str;
        }
        
        const std::string& vertex_in_tangent()
        {
            return vertex_in_tangent_str;
        }
        
        const std::string& vertex_in_color()
        {
            return vertex_in_color_str;
        }
        
        const std::string& vertex_in_texture()
        {
            return vertex_in_texture_str;
        }
    }
}


namespace yavsg // Vertex in uniforms //////////////////////////////////////////
{
    namespace shader_strings
    {
        const std::string& transform_model()
        {
            return transform_model_str;
        }
        
        const std::string& transform_view()
        {
            return transform_view_str;
        }
        
        const std::string& transform_projection()
        {
            return transform_projection_str;
        }
    }
}


namespace yavsg // Vertex out / fragment in ////////////////////////////////////
{
    namespace shader_strings
    {
        const std::string& vertex_out_position()
        {
            return vertex_out_position_str;
        }
        
        const std::string& vertex_out_color()
        {
            return vertex_out_color_str;
        }
        
        const std::string& vertex_out_texture()
        {
            return vertex_out_texture_str;
        }
        
        const std::string& vertex_out_TBN_matrix()
        {
            return vertex_out_TBN_matrix_str;
        }
    }
    
    namespace shader_strings
    {
        const std::string& fragment_in_position()
        {
            return fragment_in_position_str;
        }
        
        const std::string& fragment_in_color()
        {
            return fragment_in_color_str;
        }
        
        const std::string& fragment_in_texture()
        {
            return fragment_in_texture_str;
        }
        
        const std::string& fragment_in_TBN_matrix()
        {
            return fragment_in_TBN_matrix_str;
        }
    }
}


namespace yavsg // Fragment in uniforms ////////////////////////////////////////
{
    namespace shader_strings
    {
        const std::string& map_color()
        {
            return map_color_str;
        }
        
        const std::string& map_normal()
        {
            return map_normal_str;
        }
        
        const std::string& map_specular()
        {
            return map_specular_str;
        }
    }
}


namespace yavsg // Fragment out ////////////////////////////////////////////////
{
    namespace shader_strings
    {
        const std::string& fragment_out_color()
        {
            return fragment_out_color_str;
        }
    }
}