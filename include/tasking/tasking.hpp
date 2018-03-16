#pragma once
#ifndef YAVSG_TASKING_TASKING_HPP
#define YAVSG_TASKING_TASKING_HPP


#include <memory>   // std::unique_ptr
#include <utility>  // std::size_t


namespace yavsg // Tasks ///////////////////////////////////////////////////////
{
    using task_flags_type = std::size_t;
    namespace task_flag
    {
        static const task_flags_type          NONE = 0x00;
        
        // Task must run on the main thread
        static const task_flags_type   MAIN_THREAD = 0x01 << 0;
        
        // Task must run on the thread communicating with the GPU (usually but
        // not necessarily the main thread)
        static const task_flags_type    GPU_THREAD = 0x01 << 1;
        
        // Any thread that can execute this task should do so before any other
        // task not also marked high priority
        static const task_flags_type HIGH_PRIORITY = 0x01 << 2;
    }
    
    class task
    {
    public:
        virtual ~task();
        virtual task_flags_type flags() const = 0;
        virtual task_flags_type flags() const;
        virtual bool operator()() = 0;
    };
}


namespace yavsg // Task system /////////////////////////////////////////////////
{
    void submit_task( std::unique_ptr< task > );
    
    using task_worker_flags_type = std::size_t;
    namespace task_worker_flag
    {
        static const task_worker_flags_type        NONE = 0x00;
        
        // Thread is the main thread
        static const task_worker_flags_type MAIN_THREAD = 0x01 << 0;
        
        // Thread is the/a thread communicating with the GPU
        static const task_worker_flags_type  GPU_THREAD = 0x01 << 1;
    }
    
    void become_task_worker( task_worker_flags_type = task_worker_flag::NONE );
    
    void initialize_task_system( bool main_will_task = true );
    void initialize_task_system( std::size_t worker_count );
    void       stop_task_system( bool dequeue_all = false );
}


#endif
