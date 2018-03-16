#include "../../include/tasking/tasking.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <list>


namespace
{
    // Holds only the workers belonging to the tasking subsystem; there may be
    // other threads running as workers via become_task_worker(), such as the
    // main thread.
    std::vector< std::thread > slaved_workers;
    
    // Subsystem & queues access control variables
    std::atomic< bool > task_system_running{ false };
    std::mutex queues_mutex;
    std::condition_variable queues_condition;
    
    using task_ptr_type   = std::unique_ptr< yavsg::task >;
    using task_queue_type = std::list< task_ptr_type >;
    task_queue_type    main_hipri_queue;
    task_queue_type    main_lopri_queue;
    task_queue_type general_hipri_queue;
    task_queue_type general_lopri_queue;
}


namespace yavsg
{
    void submit_task( std::unique_ptr< task > t )
    {
        std::lock_guard< std::mutex > lock( queues_mutex );
        
        if(
               t -> flags() & task_flag::MAIN_THREAD
            || t -> flags() & task_flag:: GPU_THREAD
        )
        {
            if( t -> flags() & task_flag::HIGH_PRIORITY )
                main_hipri_queue.emplace_back( std::move( t ) );
            else
                main_lopri_queue.emplace_back( std::move( t ) );
        }
        else
        {
            if( t -> flags() & task_flag::HIGH_PRIORITY )
                general_hipri_queue.emplace_back( std::move( t ) );
            else
                general_lopri_queue.emplace_back( std::move( t ) );
        }
        
        queues_condition.notify_one();
    }
    
    void become_task_worker( task_worker_flags_type worker_flags )
    {
        // Main thread and GPU thread are the same for now
        if( worker_flags & task_worker_flag::MAIN_THREAD )
            worker_flags |= task_worker_flag::GPU_THREAD;
        
        // List of queues to check, in order of priority
        std::vector< task_queue_type* > queues;
        
        if( worker_flags & task_worker_flag::MAIN_THREAD )
        {
            queues.push_back( &main_hipri_queue );
            queues.push_back( &main_lopri_queue );
        }
        queues.push_back( &general_hipri_queue );
        queues.push_back( &general_lopri_queue );
        
        while( true )
        {
            std::unique_lock< std::mutex > lock( queues_mutex );
            
            if( !task_system_running )
            {
                lock.unlock();
                break;
            }
            
            task_ptr_type task;
            
            for( auto& queue : queues )
                for(
                    auto task_iter = queue -> begin();
                    task_iter != queue -> end();
                    ++task_iter
                )
                {
                    auto task_flags = ( *task_iter ) -> flags();
                    
                    auto main_req_met = !(
                                task_flags &        task_flag::MAIN_THREAD
                        && !( worker_flags & task_worker_flag::MAIN_THREAD )
                    );
                    auto gpu_req_met = !(
                                task_flags &        task_flag::GPU_THREAD
                        && !( worker_flags & task_worker_flag::GPU_THREAD )
                    );
                    
                    if( main_req_met && gpu_req_met )
                    {
                        std::swap( task, *task_iter );
                        queue -> erase( task_iter );
                        goto done_searching;
                    }
                }
        done_searching:
            
            std::size_t total_tasks_enqueued = (
                     main_hipri_queue.size()
                +    main_lopri_queue.size()
                + general_hipri_queue.size()
                + general_lopri_queue.size()
            );
            
            if( total_tasks_enqueued > 0 )
                // Let some other worker pop
                queues_condition.notify_one();
            
            // Execute task and conditionally requeue it
            if( task && ( *task )() )
                submit_task( std::move( task ) );
            task = nullptr;
            
            queues_condition.wait( lock );
        }
    }
    
    void initialize_task_system( bool main_will_task )
    {
        std::size_t num_workers = std::thread::hardware_concurrency();
        if( main_will_task )
            num_workers -= 1;
        initialize_task_system( num_workers );
    }
    
    void initialize_task_system( std::size_t worker_count )
    {
        task_system_running = true;
        for( std::size_t i = 0; i < worker_count; ++i )
            slaved_workers.emplace_back(
                become_task_worker,
                task_worker_flag::NONE
            );
    }
    
    void stop_task_system( bool dequeue_all )
    {
        task_system_running = false;
        queues_condition.notify_all();
        for( auto& worker : slaved_workers )
            worker.join();
        if( dequeue_all )
        {
               main_hipri_queue.clear();
               main_lopri_queue.clear();
            general_hipri_queue.clear();
            general_lopri_queue.clear();
        }
    }
}
