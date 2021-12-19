#pragma once


#include <cstddef>  // size_t


namespace JadeMatrix::yavsg
{
    using task_flags_type = std::size_t;
    namespace task_flag
    {
        static const task_flags_type          none = 0x00;
        
        // Task must run on the main thread
        static const task_flags_type   main_thread = 0x01 << 0;
        
        // Task must run on the thread communicating with the GPU (usually but
        // not necessarily the main thread)
        static const task_flags_type    gpu_thread = 0x01 << 1;
        
        // Any thread that can execute this task should do so before any other
        // task not also marked high priority
        static const task_flags_type high_priority = 0x01 << 2;
    }
    
    class task
    {
    public:
        virtual ~task();
        virtual task_flags_type flags() const;
        virtual bool operator()() = 0;
    };
}
