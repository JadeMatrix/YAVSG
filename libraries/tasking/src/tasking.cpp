#include <yavsg/tasking/tasking.hpp>

#include <yavsg/logging.hpp>
#include <yavsg/tasking/utility_tasks.hpp>  // stop_task_system_task
#include <yavsg/gl/error.hpp>               // summary_error

#include <fmt/ostream.h>    // std::thread::id support

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>
#include <list>


namespace
{
    using namespace std::string_view_literals;
    
    auto const log_ = JadeMatrix::yavsg::log_handle();
    
    // Holds only the workers belonging to the tasking subsystem; there may be
    // other threads running as workers via `become_task_worker()`, such as the
    // main thread.
    std::vector< std::thread > slaved_workers;
    
    // Subsystem & queues access control variables
    std::atomic< bool > task_system_running{ false };
    std::mutex queues_mutex;
    std::condition_variable queues_condition;
    
    using task_ptr_type   = std::unique_ptr< JadeMatrix::yavsg::task >;
    using task_queue_type = std::list< task_ptr_type >;
    task_queue_type queue_hipri_main;
    task_queue_type queue_lopri_main;
    task_queue_type queue_hipri_general;
    task_queue_type queue_lopri_general;
}


void JadeMatrix::yavsg::submit_task( std::unique_ptr< task > t )
{
    std::unique_lock lock( queues_mutex );
    
    if(
           t->flags() & task_flag::main_thread
        || t->flags() & task_flag:: gpu_thread
    )
    {
        if( t->flags() & task_flag::high_priority )
        {
            queue_hipri_main.emplace_back( std::move( t ) );
        }
        else
        {
            queue_lopri_main.emplace_back( std::move( t ) );
        }
    }
    else
    {
        if( t->flags() & task_flag::high_priority )
        {
            queue_hipri_general.emplace_back( std::move( t ) );
        }
        else
        {
            queue_lopri_general.emplace_back( std::move( t ) );
        }
    }
    
    // If the notified worker is unable to execute the task due to flags,
    // notifications will chain until one can
    queues_condition.notify_one();
}

void JadeMatrix::yavsg::become_task_worker(
    task_worker_flags_type worker_flags
)
{
    // Main thread and GPU thread are the same for now
    if( worker_flags & task_worker_flag::main_thread )
    {
        worker_flags |= task_worker_flag::gpu_thread;
    }
    else if( worker_flags & task_worker_flag::gpu_thread )
    {
        worker_flags |= task_worker_flag::main_thread;
    }
    
    // List of queues to check, in order of priority
    std::vector< task_queue_type* > queues;
    
    if( worker_flags & task_worker_flag::main_thread )
    {
        queues.push_back( &queue_hipri_main );
        queues.push_back( &queue_lopri_main );
    }
    queues.push_back( &queue_hipri_general );
    queues.push_back( &queue_lopri_general );
    
    while( true )
    {
        std::unique_lock lock( queues_mutex );
        
        if( !task_system_running )
        {
            break;
        }
        
        task_ptr_type task;
        
        for( auto& queue : queues )
        {
            for(
                auto task_iter = queue->begin();
                task_iter != queue->end();
                ++task_iter
            )
            {
                auto const task_flags = ( *task_iter )->flags();
                
                auto const main_req_met = !(
                            task_flags &        task_flag::main_thread
                    && !( worker_flags & task_worker_flag::main_thread )
                );
                auto const gpu_req_met = !(
                            task_flags &        task_flag::gpu_thread
                    && !( worker_flags & task_worker_flag::gpu_thread )
                );
                
                if( main_req_met && gpu_req_met )
                {
                    std::swap( task, *task_iter );
                    queue->erase( task_iter );
                    goto done_searching;
                }
            }
        }
    done_searching:
        
        std::size_t total_tasks_enqueued = (
              queue_hipri_main   .size()
            + queue_lopri_main   .size()
            + queue_hipri_general.size()
            + queue_lopri_general.size()
        );
        
        bool encountered_error = false;
        
        if( total_tasks_enqueued > 0 )
        {
            // Let some other worker pop
            queues_condition.notify_one();
        }
        
        if( task )
        {
            lock.unlock();
            
            try
            {
                if( ( *task )() )
                {
                    submit_task( std::move( task ) );
                }
                
                // Dispose of task if it wasn't requeued
                task = nullptr;
            }
            catch( const yavsg::gl::summary_error& e )
            {
                std::stringstream codes;
                yavsg::gl::print_summary_error_codes( codes, e );
                log_.error(
                    "Worker thread {} exiting due to OpenGL error: {}; "
                        "codes:{}"sv,
                    std::this_thread::get_id(),
                    e.what(),
                    codes.str()
                );
                encountered_error = true;
            }
            catch( const std::exception& e )
            {
                log_.error(
                    "Worker thread {} exiting due to uncaught "
                        "std::exception: {}"sv,
                    std::this_thread::get_id(),
                    e.what()
                );
                encountered_error = true;
            }
            catch( ... )
            {
                log_.error(
                    "Worker thread {} exiting due to uncaught "
                        "non-std::exception"sv,
                    std::this_thread::get_id()
                );
                encountered_error = true;
            }
        }
        else
        {
            queues_condition.wait( lock );
        }
        
        if( encountered_error )
        {
            task_system_running = false;
            queues_condition.notify_all();
        }
    }
}

void JadeMatrix::yavsg::initialize_task_system( bool main_will_task )
{
    std::size_t num_workers = std::thread::hardware_concurrency();
    if( main_will_task )
    {
        num_workers -= 1;
    }
    initialize_task_system( num_workers );
}

void JadeMatrix::yavsg::initialize_task_system( std::size_t worker_count )
{
    task_system_running = true;
    for( std::size_t i = 0; i < worker_count; ++i )
    {
        slaved_workers.emplace_back(
            become_task_worker,
            task_worker_flag::none
        );
    }
}

void JadeMatrix::yavsg::stop_task_system( bool dequeue_all )
{
    task_system_running = false;
    queues_condition.notify_all();
    for( auto& worker : slaved_workers )
    {
        worker.join();
    }
    if( dequeue_all )
    {
        queue_hipri_main   .clear();
        queue_lopri_main   .clear();
        queue_hipri_general.clear();
        queue_lopri_general.clear();
    }
}


// stop_task_system_task ///////////////////////////////////////////////////////

bool JadeMatrix::yavsg::stop_task_system_task::operator()()
{
    task_system_running = false;
    queues_condition.notify_all();
    return false;
}

JadeMatrix::yavsg::task_flags_type
JadeMatrix::yavsg::stop_task_system_task::flags() const
{
    return task_flag::high_priority;
}
