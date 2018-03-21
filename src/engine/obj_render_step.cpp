#include "../../include/engine/obj_render_step.hpp"

#include "../../include/gl/error.hpp"
#include "../../include/gl/shader.hpp"
#include "../../include/gl/texture.hpp"
#include "../../include/math/common_transforms.hpp"
#include "../../include/rendering/shader_variable_names.hpp"

#include <array>
#include <exception>
#include <vector>


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
        } )
    {
        obj_scene.insert_model(
            obj_filename,
            obj_mtl_directory
        );
        
        // Link attributes
        auto objects_ref = obj_scene.object_manager.write();
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
        obj_scene.main_camera.fov( desired_fov );
        obj_scene.main_camera.look_at( { 0.0f, 0.0f, 0.0f } );
        obj_scene.main_camera.focal_point( 1.0f );
    }
    
    // obj_render_step::~obj_render_step()
    // {
        
    // }
    
    void obj_render_step::run( gl::write_only_framebuffer& target )
    {
        glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
        glClear(
              GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT
            | GL_STENCIL_BUFFER_BIT
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't clear buffer for yavsg::obj_render_step::run()"
        );
        
        glEnable( GL_DEPTH_TEST );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't enable depth testing for yavsg::obj_render_step::run()"
        );
        
        scene_program.set_uniform(
            shader_string_id::TRANSFORM_VIEW,
            obj_scene.main_camera.view< GLfloat >()
        );
        
        scene_program.set_uniform(
            shader_string_id::TRANSFORM_PROJECTION,
            obj_scene.main_camera.projection< GLfloat >(
                  static_cast< GLfloat >( target.width () )
                / static_cast< GLfloat >( target.height() )
            )
        );
        
        scene_program.set_uniform(
            shader_string_id::CAMERA_POINT_NEAR,
            obj_scene.main_camera.near_point()
        );
        scene_program.set_uniform(
            shader_string_id::CAMERA_POINT_FOCAL,
            obj_scene.main_camera.focal_point()
        );
        scene_program.set_uniform(
            shader_string_id::CAMERA_POINT_FAR,
            obj_scene.main_camera.far_point()
        );
        
        auto objects_ref = obj_scene.object_manager.read();
        
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
                        scene::material_texture_count
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
