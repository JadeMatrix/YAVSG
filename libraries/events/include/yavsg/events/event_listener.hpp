#pragma once


#include <yavsg/tasking/task.hpp>

#include <functional>   // function
#include <utility>      // size_t, swap


/*
Internally, the event listener system uses a task to poll events.  This task
loops through the currently available events and submits a task for each
callback registered to that event type.  This prevents event polling from
causing hangups due to a long-running event callback and from it taking up too
much time in its worker in general.  Additionally, it prevents deadlocks when an
event callback wishes to change the event handling system.
*/


namespace JadeMatrix::yavsg
{
    using listener_id = std::size_t;
    
    template< class EventType > class event_listener
    {
    public:
        event_listener(
            std::function< void( EventType const& ) >,
            task_flags_type flags = task_flag::none
        );
        
        event_listener( event_listener     && ) = default;
        event_listener( event_listener const& ) = delete;
        
        ~event_listener();
        
        event_listener& operator=( event_listener&& o )
        {
            std::swap( id_, o.id_ );
            return *this;
        }
        
    private:
        listener_id id_;
    };
}
