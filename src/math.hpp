#pragma once
#ifndef YAVSG_MATH_HPP
#define YAVSG_MATH_HPP


#include "math.hpp"

// http://en.cppreference.com/w/cpp/header/cmath
#include <cmath>
#include <cstdint>


namespace yavsg
{
    // template< typename T > T root( const T&, const T& );
    template< typename T > T sqrt( const T& );
    
    
    // Defaults for built-in types /////////////////////////////////////////////
    
    
    template< typename T > T sqrt( const T& v ) { return std::sqrt( v ); }
    
    template< typename T > T newtons_method( const T& v )
    {
        // T result;
        // T x_k = v;
        // T diff;
        
        // do
        // {
        //     result = ( x_k + v / x_k ) / 2;
        //     diff = result - x_k;
        //     x_k = result;
        // } while( diff <= -1 && diff >= 1 );
        
        // return result;
        
        if( v <= 0 )
            return 0;
        
        T current_value = ( T )1 << ( sizeof( T ) * 8 / 2 );
        
        while( true )
        // for( int i = 0; i < 2 /*256*/; ++i )
        {
            T next_value = ( current_value + ( v / current_value ) ) / 2;
            if( next_value >= current_value )
                return current_value;
            current_value = next_value;
        }
        
        return current_value;
    }
    
    template<> inline int_fast64_t sqrt< int_fast64_t >( const int_fast64_t& v )
    {
        return newtons_method< int_fast64_t >( v );
    }
    template<> inline __int128 sqrt< __int128 >( const __int128& v )
    {
        return newtons_method< __int128 >( v );
    }
}


#endif
