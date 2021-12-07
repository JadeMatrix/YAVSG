#pragma once


#include "render_object_manager.hpp"
#include "scene.hpp"

#include <yavsg/tasking/task.hpp>
#include <yavsg/tasking/tasking.hpp>

#include <filesystem>
#include <utility>  // move


namespace JadeMatrix::yavsg
{
    class load_obj_task : public yavsg::task
    {
    public:
        load_obj_task(
            // TODO: Possibly `std::shared_ptr` so the manager doesn't get
            // destroyed before this executes
            scene::render_object_manager_type& om,
            std::filesystem::path              fn,
            std::filesystem::path              md
        ) :
            object_manager   { om              },
            obj_filename     { std::move( fn ) },
            obj_mtl_directory{ std::move( md ) },
            upload_mode      { false           }
        {}
        
        task_flags_type flags() const;
        
        bool operator()();
        
    protected:
        scene::render_object_manager_type& object_manager;
        std::filesystem::path              obj_filename;
        std::filesystem::path              obj_mtl_directory;
        
        std::vector< scene::material_description > materials;
        std::vector< scene::vertex_type          > vertices;
        std::vector< std::vector< GLuint >       > indices;
        GLfloat obj_max_x;
        GLfloat obj_min_x;
        GLfloat obj_max_y;
        GLfloat obj_min_y;
        GLfloat obj_max_z;
        GLfloat obj_min_z;
        
        bool upload_mode;
    };
}
