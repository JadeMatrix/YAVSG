#pragma once


#include "task.hpp"

#include <cstddef>  // size_t
#include <memory>   // unique_ptr


namespace JadeMatrix::yavsg
{
    void submit_task( std::unique_ptr< task > );
    
    using task_worker_flags_type = std::size_t;
    namespace task_worker_flag
    {
        static const task_worker_flags_type        none = 0x00;
        
        // Thread is the main thread
        static const task_worker_flags_type main_thread = 0x01 << 0;
        
        // Thread is the/a thread communicating with the GPU
        static const task_worker_flags_type  gpu_thread = 0x01 << 1;
    }
    
    void become_task_worker( task_worker_flags_type = task_worker_flag::none );
    
    void initialize_task_system( bool main_will_task = true );
    void initialize_task_system( std::size_t worker_count );
    // Synchronous, must be called before main thread exits, and after
    // `become_task_worker()` exits on the calling thread (if that function was
    // called)
    void       stop_task_system( bool dequeue_all = false );
}
