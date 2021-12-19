#include <yavsg/tasking/task.hpp>


JadeMatrix::yavsg::task::~task() {}

JadeMatrix::yavsg::task_flags_type JadeMatrix::yavsg::task::flags() const
{
    return task_flag::none;
}
