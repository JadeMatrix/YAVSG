#pragma once
#ifndef YAVSG_EVENTS_EVENT_LISTENER_HPP
#define YAVSG_EVENTS_EVENT_LISTENER_HPP


#include <functional>   // std::function
#include <utility>      // std::size_t


namespace yavsg
{
    using listener_id = std::size_t;
    
    template< class EventType > class event_listener
    {
    protected:
        listener_id _id;
        
    public:
        event_listener( std::function< void( const EventType& ) > );
        
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
