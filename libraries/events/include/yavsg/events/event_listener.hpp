#pragma once
#ifndef YAVSG_EVENTS_EVENT_LISTENER_HPP
#define YAVSG_EVENTS_EVENT_LISTENER_HPP


#include <yavsg/tasking/task.hpp>

#include <functional>   // std::function
#include <utility>      // std::size_t


/*
Internally, the event listener system uses a task to poll events.  This task
loops through the currently available events and submits a task for each
callback registered to that event type.  This prevents event polling from
causing hangups due to a long-running event callback and from it taking up too
much time in its worker in general.  Additionally, it prevents deadlocks when an
event callback wishes to change the event handling system.
*/


namespace yavsg
{
    using listener_id = std::size_t;
    
    template< class EventType > class event_listener
    {
    protected:
        listener_id _id;
        
    public:
        event_listener(
            std::function< void( const EventType& ) >,
            task_flags_type flags = task_flag::NONE
        );
        
        event_listener( event_listener&& ) = default;
        event_listener( const event_listener& ) = delete;
        
        ~event_listener();
        
        event_listener& operator=( event_listener&& o )
        {
            std::swap( _id, o._id );
            return *this;
        }
    };
}


#endif
