#pragma once
#ifndef YAVSG_TASKING_UTILITY_TASKS_HPP
#define YAVSG_TASKING_UTILITY_TASKS_HPP


#include "task.hpp"

#include <functional>   // std::function


namespace JadeMatrix::yavsg
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
    class callback_task : public task
    {
    public:
        using callback_type = std::function< bool( void ) >;
        
    protected:
        callback_type _callback;
        task_flags_type _flags;
        
    public:
        callback_task( callback_type c, task_flags_type f ) :
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
