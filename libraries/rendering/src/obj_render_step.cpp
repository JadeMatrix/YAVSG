#include <yavsg/rendering/obj_render_step.hpp>

#include <yavsg/gl/error.hpp>
#include <yavsg/gl/shader.hpp>
#include <yavsg/gl/texture.hpp>
#include <yavsg/math/common_transforms.hpp>
#include <yavsg/rendering/shader_utils.hpp> // shaders_dir
#include <yavsg/rendering/shader_variable_names.hpp>
#include <yavsg/tasking/tasking.hpp>

#include <array>


namespace yavsg
{
    obj_render_step::obj_render_step() :
        first_run{ true },
        scene_program( {
            gl::shader::from_file(
                GL_VERTEX_SHADER,
                shaders_dir() + "/obj_scene.vert"
            ).id,
            gl::shader::from_file(
                GL_FRAGMENT_SHADER,
                shaders_dir() + "/obj_scene.frag"
            ).id
        } )
    {
        scene_program.bind_target< 0 >(
            shader_string_id::FRAGMENT_OUT_COLOR
        );
    }
    
    void obj_render_step::run(
        const scene               & obj_scene,
        gl::write_only_framebuffer& target
    )
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
        
        if(
            first_run
            && objects_ref -> size()
            && objects_ref -> back().vertices.size()
        )
        {
            // Link attributes
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
            
            first_run = false;
        }
        
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
