#include "../../include/engine/obj_render_step.hpp"

#include "../../include/gl/shader.hpp"
#include "../../include/gl/texture.hpp"
#include "../../include/math/common_transforms.hpp"
#include "../../include/rendering/shader_variable_names.hpp"

#include <tiny_obj_loader.h>

#include <algorithm>    // std::max()
#include <array>
#include <exception>
#include <iostream>     // std::cerr for tinyobj warnings


namespace
{
    void manager_insert_obj(
        yavsg::obj_render_step::render_object_manager_type& manager,
        const std::string& obj_filename,
        const std::string& obj_mtl_directory
    )
    {
        tinyobj::attrib_t obj_attributes;
        std::vector< tinyobj::shape_t    > obj_shapes;
        std::vector< tinyobj::material_t > obj_materials;
        
        std::string tinyobj_error;
        if( !tinyobj::LoadObj(
            &obj_attributes,
            &obj_shapes,
            &obj_materials,
            &tinyobj_error,
            obj_filename.c_str(),
            obj_mtl_directory.c_str()
        ) )
            throw std::runtime_error(
                "unable to load OBJ file: " + tinyobj_error
            );
        else if( !tinyobj_error.empty() )
            std::cerr << "tinyobj warning: " << tinyobj_error << std::endl;
        
        // Load textures
        std::vector< yavsg::material_description > materials;
        for( auto& material : obj_materials )
        {
            yavsg::material_description description;
            
            struct texture_info
            {
                yavsg::material_texture_type& texture;
                const std::string& filename;
                yavsg::gl::texture_flags_type flags;
            };
            
            yavsg::material_texture_type    color_map;
            yavsg::material_texture_type   normal_map;
            yavsg::material_texture_type specular_map;
            
            for( auto& info : {
                texture_info{    color_map, material.diffuse_texname , yavsg::gl::texture_flag::NONE         },
                texture_info{   normal_map, material.bump_texname    , yavsg::gl::texture_flag::LINEAR_INPUT },
                texture_info{ specular_map, material.specular_texname, yavsg::gl::texture_flag::LINEAR_INPUT }
            } )
            {
                if( info.filename.size() )
                {
                    std::string texture_filename =
                        obj_mtl_directory + info.filename;
                    
                    using settings = yavsg::gl::texture_filter_settings;
                    
                    info.texture = yavsg::material_texture_type::from_file(
                        texture_filename,
                        {
                            settings::magnify_mode::LINEAR,
                            settings::minify_mode::LINEAR,
                            settings::mipmap_type::LINEAR,
                        },
                        info.flags
                    );
                }
            }
            
            materials.push_back( yavsg::material_description{
                   color_map,
                  normal_map,
                specular_map
            } );
        }
        
        // Create vertices
        std::vector< yavsg::obj_render_step::vertex_type > vertices;
        std::vector< std::vector< GLuint > > indices( materials.size() );
        GLfloat obj_max_x = 0.0f;
        GLfloat obj_min_x = 0.0f;
        GLfloat obj_max_y = 0.0f;
        GLfloat obj_min_y = 0.0f;
        GLfloat obj_max_z = 0.0f;
        GLfloat obj_min_z = 0.0f;
        for( auto& obj_shape : obj_shapes )
        {
            std::size_t index_offset = 0;
            
            for(
                std::size_t face = 0;
                face < obj_shape.mesh.num_face_vertices.size();
                ++face
            )
            {
                auto vertex_count = obj_shape.mesh.num_face_vertices[ face ];
                
                // TODO: Suport other vertex counts
                if( vertex_count != 3 )
                    throw std::runtime_error(
                        "vertex count = "
                        + std::to_string( vertex_count )
                    );
                
                auto& group_indices = indices[
                    obj_shape.mesh.material_ids[ face ]
                ];
                
                for( size_t i = 0; i < vertex_count; ++i )
                {
                    auto index = obj_shape.mesh.indices[ index_offset + i ];
                    
                    auto vx = obj_attributes.vertices[ 3 * index.vertex_index + 0 ];
                    auto vy = obj_attributes.vertices[ 3 * index.vertex_index + 1 ];
                    auto vz = obj_attributes.vertices[ 3 * index.vertex_index + 2 ];
                    
                    auto nx = obj_attributes.normals[ 3 * index.normal_index + 0 ];
                    auto ny = obj_attributes.normals[ 3 * index.normal_index + 1 ];
                    auto nz = obj_attributes.normals[ 3 * index.normal_index + 2 ];
                    
                    // Colors use index.vertex_index
                    auto cr = obj_attributes.colors[ 3 * index.vertex_index + 0 ];
                    auto cb = obj_attributes.colors[ 3 * index.vertex_index + 1 ];
                    auto cg = obj_attributes.colors[ 3 * index.vertex_index + 2 ];
                    
                    auto tu = obj_attributes.texcoords[ 2 * index.texcoord_index + 0 ];
                    auto tv = obj_attributes.texcoords[ 2 * index.texcoord_index + 1 ];
                    
                    vertices.push_back( {
                        { vx, vz, vy }, // { vx, vy, vz },
                        { nx, nz, ny },
                        { cr, cb, cg },
                        { tu, tv }
                    } );
                    group_indices.push_back( vertices.size() - 1 );
                    
                    if( vx > obj_max_x )
                        obj_max_x = vx;
                    if( vx < obj_min_x )
                        obj_min_x = vx;
                    
                    if( vy > obj_max_y )
                        obj_max_y = vy;
                    if( vy < obj_min_y )
                        obj_min_y = vy;
                    
                    if( vz > obj_max_z )
                        obj_max_z = vz;
                    if( vz < obj_min_z )
                        obj_min_z = vz;
                }
                
                index_offset += vertex_count;
            }
        }
        
        // Move OBJ data over to manager
        {
            using group_type =
                yavsg::obj_render_step::render_object_manager_type::render_group;
            
            auto objects_ref = manager.write();
            
            GLfloat width  = obj_max_x - obj_min_x;
            GLfloat length = obj_max_y - obj_min_y;
            GLfloat height = obj_max_z - obj_min_z;
            GLfloat scale  = 13.0f / std::max( { width, length, height } );
            
            objects_ref -> emplace_back(
                std::vector< group_type >{},
                vertices,
                yavsg::vector< GLfloat, 3 >{ 0.0f, 0.0f, 0.0f },
                yavsg::vector< GLfloat, 3 >{ scale, scale, scale },
                yavsg::versor< GLfloat >::from_euler(
                    yavsg::radians< GLfloat >( 0.0f ),
                    yavsg::vector< GLfloat, 3 >{ 0.0f, 0.0f, 1.0f }
                )
            );
            
            auto& groups = objects_ref -> back().render_groups;
            
            for( std::size_t i = 0; i < materials.size(); ++i )
                groups.emplace_back(
                    std::move( materials[ i ] ),
                    indices[ i ]
                );
        }
    }
}


namespace yavsg
{
    obj_render_step::obj_render_step(
        const std::string& obj_filename,
        const std::string& obj_mtl_directory
    ) :
        scene_program( {
            gl::shader::from_file(
                GL_VERTEX_SHADER,
                "../src/shaders/obj_scene.vert"
            ).id,
            gl::shader::from_file(
                GL_FRAGMENT_SHADER,
                "../src/shaders/obj_scene.frag"
            ).id
        } ),
        main_camera(
            { 2.2f, 0.0f, 2.2f },
            0.1f,
            1.0f,
            10.0f
        )
    {
        manager_insert_obj(
            object_manager,
            obj_filename,
            obj_mtl_directory
        );
        
        // Link attributes
        auto objects_ref = object_manager.write();
        auto& uploaded_vertices = objects_ref -> back().vertices;
        scene_program.link_attribute< 0 >(
            shader_string_id::VERTEX_IN_POSITION,
            uploaded_vertices
        );
        scene_program.link_attribute< 1 >(
            shader_string_id::VERTEX_IN_NORMAL,
            uploaded_vertices
        );
        scene_program.link_attribute< 2 >(
            shader_string_id::VERTEX_IN_COLOR,
            uploaded_vertices
        );
        scene_program.link_attribute< 3 >(
            shader_string_id::VERTEX_IN_TEXTURE,
            uploaded_vertices
        );
        
        scene_program.bind_target< 0 >(
            shader_string_id::FRAGMENT_OUT_COLOR
        );
        
        auto    base_fov = yavsg::radians< GLfloat >{ 45 }; // 58.31f°
        auto desired_fov = yavsg::radians< GLfloat >{
            yavsg::degrees< GLfloat >{ 90 }
        };
        main_camera.fov( desired_fov );
        main_camera.look_at( { 0.0f, 0.0f, 0.0f } );
        main_camera.focal_point( 1.0f );
    }
    
    // obj_render_step::~obj_render_step()
    // {
        
    // }
    
    void obj_render_step::run( gl::write_only_framebuffer& target )
    {
        // TODO: error checking
        
        glEnable( GL_DEPTH_TEST );
        glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        
        scene_program.set_uniform(
            shader_string_id::TRANSFORM_VIEW,
            main_camera.view< GLfloat >()
        );
        
        scene_program.set_uniform(
            shader_string_id::TRANSFORM_PROJECTION,
            main_camera.projection< GLfloat >(
                  static_cast< GLfloat >( target.width () )
                / static_cast< GLfloat >( target.height() )
            )
        );
        
        scene_program.set_uniform(
            shader_string_id::CAMERA_POINT_NEAR,
            main_camera.near_point()
        );
        scene_program.set_uniform(
            shader_string_id::CAMERA_POINT_FOCAL,
            main_camera.focal_point()
        );
        scene_program.set_uniform(
            shader_string_id::CAMERA_POINT_FAR,
            main_camera.far_point()
        );
        
        auto objects_ref = object_manager.read();
        
        for( auto& object : *objects_ref )
        {
            scene_program.set_uniform(
                shader_string_id::TRANSFORM_MODEL,
                object.transform_model()
            );
            
            for( auto& group : object.render_groups )
            {
                group.material.bind(
                    scene_program,
                    std::array<
                        shader_string_id,
                        std::tuple_size< material_description::tuple_type >::value
                    >{
                        shader_string_id::MAP_COLOR,
                        shader_string_id::MAP_NORMAL,
                        shader_string_id::MAP_SPECULAR
                    }
                );
                
                scene_program.run(
                    object.vertices,
                    group.indices
                );
            }
        }
    }
}