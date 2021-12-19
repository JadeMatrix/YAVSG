#pragma once


#include "task.hpp"

#include <functional>   // function


namespace JadeMatrix::yavsg
{
    // This just triggers worker shutdown internally -- stop_task_system() must
    // still be called as normal
    class stop_task_system_task : public task
    {
    public:
        // Implemented in "src/tasking/tasking.cpp"
        bool operator()() override;
        task_flags_type flags() const override;
    };
    
    // General-purpose callback-based task for lambdas etc; typically an
    // overload of `yavsg::task` should be used
    class callback_task : public task
    {
    public:
        using callback_type = std::function< bool( void ) >;
        
    protected:
        callback_type callback_;
        task_flags_type flags_;
        
    public:
        callback_task( callback_type c, task_flags_type f ) :
            callback_( c ),
            flags_( f )
        {}
        
        task_flags_type flags() const override
        {
            return flags_;
        }
        
        bool operator()() override
        {
            return callback_();
        }
    };
}
