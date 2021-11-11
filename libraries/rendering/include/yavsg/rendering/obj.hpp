#pragma once


#include "render_object_manager.hpp"
#include "scene.hpp"

#include <yavsg/tasking/task.hpp>
#include <yavsg/tasking/tasking.hpp>


namespace JadeMatrix::yavsg
{
    class load_obj_task : public yavsg::task
    {
    protected:
        scene::render_object_manager_type& object_manager;
        std::string obj_filename;
        std::string obj_mtl_directory;
        
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
        
    public:
        load_obj_task(
            // TODO: Possibly `std::shared_ptr` so the manager doesn't get
            // destroyed before this executes
            scene::render_object_manager_type& om,
            const std::string                & fn,
            const std::string                & md
        ) :
            object_manager{    om },
            obj_filename{      fn },
            obj_mtl_directory{ md },
            upload_mode{    false }
        {}
        
        task_flags_type flags() const;
        
        bool operator()();
    };
}
