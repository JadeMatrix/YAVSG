#include <yavsg/rendering/obj_render_step.hpp>

#include <yavsg/gl/error.hpp>
#include <yavsg/gl/shader.hpp>
#include <yavsg/gl/texture.hpp>
#include <yavsg/math/common_transforms.hpp>
#include <yavsg/rendering/shader_utils.hpp> // shaders_dir
#include <yavsg/rendering/shader_variable_names.hpp>
#include <yavsg/tasking/tasking.hpp>

#include <fmt/format.h>

#include <array>
#include <string>
#include <string_view>


namespace
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;
}


JadeMatrix::yavsg::obj_render_step::obj_render_step() :
    scene_program( {
        gl::shader::from_file(
            GL_VERTEX_SHADER,
            shaders_dir() / "obj_scene.vert"sv
        ).id,
        gl::shader::from_file(
            GL_FRAGMENT_SHADER,
            shaders_dir() / "obj_scene.frag"sv
        ).id
    } ),
    first_run{ true }
{
    scene_program.bind_target< 0 >( shader_string_id::fragment_out_color );
}

void JadeMatrix::yavsg::obj_render_step::run(
    scene                const& obj_scene,
    gl::write_only_framebuffer& target
)
{
    gl::ClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    gl::Clear(
          GL_COLOR_BUFFER_BIT
        | GL_DEPTH_BUFFER_BIT
        | GL_STENCIL_BUFFER_BIT
    );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't clear buffer for yavsg::obj_render_step::run()"s
    );
    
    gl::Enable( GL_DEPTH_TEST );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't enable depth testing for yavsg::obj_render_step::run()"s
    );
    
    scene_program.set_uniform(
        shader_string_id::transform_view,
        obj_scene.main_camera.view< GLfloat >()
    );
    
    scene_program.set_uniform(
        shader_string_id::transform_projection,
        obj_scene.main_camera.projection< GLfloat >(
              static_cast< GLfloat >( target.width () )
            / static_cast< GLfloat >( target.height() )
        )
    );
    
    scene_program.set_uniform(
        shader_string_id::camera_point_near,
        obj_scene.main_camera.near_point()
    );
    scene_program.set_uniform(
        shader_string_id::camera_point_focal,
        obj_scene.main_camera.focal_point()
    );
    scene_program.set_uniform(
        shader_string_id::camera_point_far,
        obj_scene.main_camera.far_point()
    );
    
    auto objects_ref = obj_scene.object_manager.read();
    
    if(
        first_run
        && !objects_ref->empty()
        && !objects_ref->back().vertices.empty()
    )
    {
        // Link attributes
        auto& uploaded_vertices = objects_ref->back().vertices;
        scene_program.link_attribute< 0 >(
            shader_string_id::vertex_in_position,
            uploaded_vertices
        );
        scene_program.link_attribute< 1 >(
            shader_string_id::vertex_in_normal,
            uploaded_vertices
        );
        scene_program.link_attribute< 2 >(
            shader_string_id::vertex_in_color,
            uploaded_vertices
        );
        scene_program.link_attribute< 3 >(
            shader_string_id::vertex_in_texture,
            uploaded_vertices
        );
        
        first_run = false;
    }
    
    for( auto& object : *objects_ref )
    {
        scene_program.set_uniform(
            shader_string_id::transform_model,
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
                    shader_string_id::map_color,
                    shader_string_id::map_normal,
                    shader_string_id::map_specular
                }
            );
            
            scene_program.run( object.vertices, group.indices );
        }
    }
}
