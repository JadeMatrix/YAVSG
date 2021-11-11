#include <yavsg/tasking/task.hpp>


namespace JadeMatrix::yavsg
{
    task::~task() {}
    
    task_flags_type task::flags() const
    {
        return task_flag::NONE;
    }
}
