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
        virtual task_flags_type flags() const;
    };
    
    // General-purpose callback-based task for lambdas etc; typically an
    // overload of `yavsg::task` should be used
    template< class Callable > class callback_task : public task
    {
    protected:
        Callable _callback;
        task_flags_type _flags;
    public:
        callback_task( Callable c, task_flags_type f ) :
            _callback( c ),
            _flags( f )
        {}
        
        virtual task_flags_type flags() const
        {
            return _flags;
        }
        
        virtual bool operator()()
        {
            return _callback();
        }
    };
}


#endif
