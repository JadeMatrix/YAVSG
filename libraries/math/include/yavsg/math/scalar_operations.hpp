#pragma once
#ifndef YAVSG_MATH_SCALAR_OPERATIONS_HPP
#define YAVSG_MATH_SCALAR_OPERATIONS_HPP


// http://en.cppreference.com/w/cpp/header/cmath
#include <cmath>
#include <cstdint>
#include <type_traits>


namespace yavsg // Power functions /////////////////////////////////////////////
{
    template< typename T > T sqrt( const T& value )
    {
        return std::sqrt( value );
    }
    
    template< typename B, typename E > B power(
        const B& base,
        const E& exponent
    )
    {
        return std::pow( base, exponent );
    }
    
    
    
    
    
    // template< typename V, typename E > constexpr T power(
    //     const V& value,
    //     const E& exponent
    // );
    // template< typename V, typename E > constexpr T root(
    //     const V& value,
    //     const E& exponent,
    //     long max_iter = -1
    // );
    
    
    // template< typename V > constexpr T power< T, int >(
    //     const V& value,
    //     const int& exponent
    // )
    // {
    //     // Shortcut for non-constexpr use
    //     if( exponent == 1 )
    //         return value;
    //     else
    //     {
    //         V powered( 1 );
    //         if( exponent < 0 )
    //             for( int i = 0; i > exponent; --i )
    //                 powered /= value;
    //         else
    //             for( int i = 0; i < exponent; ++i )
    //                 powered *= value;
    //         return powered;
    //     }
    // }
    // template<
    //     template V,
    //     typename std::enable_if< std::is_integral< V >::value, V >::type = 0
    // > constexpr T power< V, int >(
    //     const V& value,
    //     const int& exponent
    // )
    // {
    //     if( value == 2 )
    //         return value << ( exponent - 1 );
    //     else if( exponent == 1 )
    //         return value;
    //     else
    //     {
    //         V powered( 1 );
    //         if( exponent < 0 )
    //             for( int i = 0; i > exponent; --i )
    //                 powered /= value;
    //         else
    //             for( int i = 0; i < exponent; ++i )
    //                 powered *= value;
    //         return powered;
    //     }
    // }
    // // template< typename V > constexpr T root< T, int >(
    // //     const V& value,
    // //     const int& exponent,
    // //     long max_iter = -1
    // // )
    // // {
        
    // // }
    
    
    // // template< typename V > constexpr T sqrt(
    // //     const V& value,
    // //     long max_iter = -1
    // // )
    // // {
    // //     return root< V, int >( value, 2, max_iter );
    // // }
    
    
    // template< typename V, typename E > constexpr T newtons_method(
    //     const V& value,
    //     const E& exponent,
    //     long max_iter = -1
    // )
    // {
    //     // T current_value = ( T )1 << ( sizeof( T ) * 8 / 2 );
    //     V current_guess = ( V )( 1 << ( sizeof( V ) * 8 / 2 ) );
        
    //     for( int i = 0; max_iter < 0 || i < max_iter; ++i )
    //     {
    //         V numerator  = power< V >( current_guess, exponent ) - value;
    //         V derivative = exponent * power< V >( current_guess, exponent - 1 );
            
    //         V next_guess = current_guess - ( numerator / derivative );
            
    //         if( next_guess >= current_guess )
    //             return current_guess;
    //         current_guess = next_guess;
    //     }
    // }
    
    
    
    
    // // template< typename T > T root( const T&, const T& );
    // template< typename T > T sqrt( const T& );
    
    
    // // Defaults for built-in types /////////////////////////////////////////////
    
    
    // template< typename T > T sqrt( const T& v ) { return std::sqrt( v ); }
    
    // template< typename T > T newtons_method( const T& v )
    // {
    //     // T result;
    //     // T x_k = v;
    //     // T diff;
        
    //     // do
    //     // {
    //     //     result = ( x_k + v / x_k ) / 2;
    //     //     diff = result - x_k;
    //     //     x_k = result;
    //     // } while( diff <= -1 && diff >= 1 );
        
    //     // return result;
        
    //     if( v <= 0 )
    //         return 0;
        
    //     T current_value = ( T )1 << ( sizeof( T ) * 8 / 2 );
        
    //     while( true )
    //     // for( int i = 0; i < 2 /*256*/; ++i )
    //     {
    //         T next_value = ( current_value + ( v / current_value ) ) / 2;
    //         if( next_value >= current_value )
    //             return current_value;
    //         current_value = next_value;
    //     }
        
    //     return current_value;
    // }
    
    // template<> inline int_fast64_t sqrt< int_fast64_t >( const int_fast64_t& v )
    // {
    //     return newtons_method< int_fast64_t >( v );
    // }
    // template<> inline __int128 sqrt< __int128 >( const __int128& v )
    // {
    //     return newtons_method< __int128 >( v );
    // }
}


#endif
