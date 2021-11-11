#pragma once


#include "task.hpp"

#include <memory>   // std::unique_ptr
#include <utility>  // std::size_t

// TODO: tasking.hpp, task.hpp, then general utility tast files


namespace JadeMatrix::yavsg
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
    // Synchronous, must be called before main thread exits, and after
    // become_task_worker() exits on the calling thread (if that function was
    // called)
    void       stop_task_system( bool dequeue_all = false );
}
