#pragma once
#ifndef YAVSG_ENGINE_OBJ_HPP
#define YAVSG_ENGINE_OBJ_HPP


#include "../engine/scene.hpp"
#include "../rendering/render_object_manager.hpp"
#include "../tasking/task.hpp"
#include "../tasking/tasking.hpp"


namespace yavsg
{
    class load_obj_task : public yavsg::task
    {
    protected:
        scene::render_object_manager_type& object_manager;
        std::string obj_filename;
        std::string obj_mtl_directory;
        
    public:
        load_obj_task(
            // TODO: Possibly `std::shared_ptr` so the manager doesn't get
            // destroyed before this executes
            scene::render_object_manager_type& om,
            const std::string                & fn,
            const std::string                & md
        ) :
            object_manager(    om ),
            obj_filename(      fn ),
            obj_mtl_directory( md )
        {}
        
        task_flags_type flags() const
        {
            return task_flag::GPU_THREAD;
        }
        
        bool operator()();
    };
}


#endif
