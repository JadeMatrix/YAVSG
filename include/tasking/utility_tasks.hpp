#pragma once
#ifndef YAVSG_TASKING_UTILITY_TASKS_HPP
#define YAVSG_TASKING_UTILITY_TASKS_HPP


#include "task.hpp"


namespace yavsg
{
    // This just triggers worker shutdown internally -- stop_task_system() must
    // still be called as normal
    class stop_task_system_task : public task
    {
    public:
        // Implemented in "src/tasking/tasking.cpp"
        virtual bool operator()();
    };
    
    // General-purpose callback-based task for lambdas etc; typically an
    // overload of `yavsg::task` should be used
    template< class Callable > class callback_task : public task
    {
    protected:
        Callable callback;
    public:
        callback_task( Callable c ) : callback( c ) {}
        
        virtual bool operator()()
        {
            return callback();
        }
    };
}


#endif
