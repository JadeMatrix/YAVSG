#include <yavsg/rendering/shader_variable_names.hpp>


namespace
{
    using namespace std::string_literals;
    
    auto const vertex_in_position        = "vertex_in_position"s;
    auto const vertex_in_normal          = "vertex_in_normal"s;
    auto const vertex_in_tangent         = "vertex_in_tangent"s;
    auto const vertex_in_color           = "vertex_in_color"s;
    auto const vertex_in_texture         = "vertex_in_texture"s;
    
    auto const transform_model           = "transform.model"s;
    auto const transform_view            = "transform.view"s;
    auto const transform_projection      = "transform.projection"s;
    
    auto const position                  = "position"s;
    auto const color                     = "color"s;
    auto const texture                   = "texture"s;
    auto const TBN_matrix                = "TBN_matrix"s;
    auto const vertex_out_position       =  "vertex_out."s + position;
    auto const vertex_out_color          =  "vertex_out."s + color;
    auto const vertex_out_texture        =  "vertex_out."s + texture;
    auto const vertex_out_TBN_matrix     =  "vertex_out."s + TBN_matrix;
    auto const fragment_in_position      = "fragment_in."s + position;
    auto const fragment_in_color         = "fragment_in."s + color;
    auto const fragment_in_texture       = "fragment_in."s + texture;
    auto const fragment_in_TBN_matrix    = "fragment_in."s + TBN_matrix;
    
    auto const map_color                 = "map_color"s;
    auto const map_normal                = "map_normal"s;
    auto const map_specular              = "map_specular"s;
    
    auto const camera_point_near         = "camera_point.near"s;
    auto const camera_point_focal        = "camera_point.focal"s;
    auto const camera_point_far          = "camera_point.far"s;
    
    auto const framebuffer_source_color  = "framebuffer_source_color"s;
    auto const framebuffer_source_depth  = "framebuffer_source_depth"s;
    auto const framebuffer_target_width  = "framebuffer_target.width"s;
    auto const framebuffer_target_height = "framebuffer_target.height"s;
    
    auto const fragment_out_color        = "fragment_out_color"s;
}


const std::string& JadeMatrix::yavsg::shader_string( shader_string_id id )
{
    switch( id )
    {
        using ssid = shader_string_id;
    case ssid::vertex_in_position       : return vertex_in_position;
    case ssid::vertex_in_normal         : return vertex_in_normal;
    case ssid::vertex_in_tangent        : return vertex_in_tangent;
    case ssid::vertex_in_color          : return vertex_in_color;
    case ssid::vertex_in_texture        : return vertex_in_texture;
    
    case ssid::transform_model          : return transform_model;
    case ssid::transform_view           : return transform_view;
    case ssid::transform_projection     : return transform_projection;
    
    case ssid::vertex_out_position      : return vertex_out_position;
    case ssid::vertex_out_color         : return vertex_out_color;
    case ssid::vertex_out_texture       : return vertex_out_texture;
    case ssid::vertex_out_tbn_matrix    : return vertex_out_TBN_matrix;
    case ssid::fragment_in_position     : return fragment_in_position;
    case ssid::fragment_in_color        : return fragment_in_color;
    case ssid::fragment_in_texture      : return fragment_in_texture;
    case ssid::fragment_in_tbn_matrix   : return fragment_in_TBN_matrix;
    
    case ssid::map_color                : return map_color;
    case ssid::map_normal               : return map_normal;
    case ssid::map_specular             : return map_specular;
    
    case ssid::camera_point_near        : return camera_point_near;
    case ssid::camera_point_focal       : return camera_point_focal;
    case ssid::camera_point_far         : return camera_point_far;
    
    case ssid::framebuffer_source_color : return framebuffer_source_color;
    case ssid::framebuffer_source_depth : return framebuffer_source_depth;
    case ssid::framebuffer_target_width : return framebuffer_target_width;
    case ssid::framebuffer_target_height: return framebuffer_target_height;
    
    case ssid::fragment_out_color       : return fragment_out_color;
    }
}
