#pragma once
#ifndef YAVSG_MATH_TRIGONOMETRY_HPP
#define YAVSG_MATH_TRIGONOMETRY_HPP


#include "../units/angular.hpp"

#include <cmath>


namespace yavsg
{
    template< typename T > ratio< T > sin( const radians< T >& r )
    {
        return std::sin( static_cast< T >( r ) );
    }
    template< typename T > ratio< T > cos( const radians< T >& r )
    {
        return std::cos( static_cast< T >( r ) );
    }
    template< typename T > ratio< T > tan( const radians< T >& r )
    {
        return std::tan( static_cast< T >( r ) );
    }
    
    template< typename T > radians< T > arcsin( const ratio< T >& r )
    {
        return std::asin( static_cast< T >( r ) );
    }
    template< typename T > radians< T > arccos( const ratio< T >& r )
    {
        return std::acos( static_cast< T >( r ) );
    }
    template< typename T > radians< T > arctan( const ratio< T >& r )
    {
        return std::atan( static_cast< T >( r ) );
    }
    template< typename T > radians< T > arctan(
        const ratio< T >& r1,
        const ratio< T >& r2
    )
    {
        return std::atan2( static_cast< T >( r1 ), static_cast< T >( r2 ) );
    }
}


#endif
