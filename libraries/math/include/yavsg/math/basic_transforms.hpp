#pragma once


#include "matrix.hpp"
#include "quaternion.hpp"
#include "trigonometry.hpp"
#include "vector.hpp"

#include <yavsg/units/angular.hpp>


// TODO: Return values decltype()ed once unit implementation is finished


namespace JadeMatrix::yavsg // Rotation operations /////////////////////////////
{
    template< typename M, typename V >
    constexpr
    square_matrix< M, 4 > rotation( const versor< V >& v )
    {
        auto m = identity_matrix< M, 4 >();
        
        m[ 0 ][ 0 ] = 1 - 2 * v[ J ] * v[ J ] - 2 * v[ K ] * v[ K ];
        m[ 1 ][ 0 ] =     2 * v[ I ] * v[ J ] - 2 * v[ K ] * v[ W ];
        m[ 2 ][ 0 ] =     2 * v[ I ] * v[ K ] + 2 * v[ J ] * v[ W ];
        
        m[ 0 ][ 1 ] =     2 * v[ I ] * v[ J ] + 2 * v[ K ] * v[ W ];
        m[ 1 ][ 1 ] = 1 - 2 * v[ I ] * v[ I ] - 2 * v[ K ] * v[ K ];
        m[ 2 ][ 1 ] =     2 * v[ J ] * v[ K ] - 2 * v[ I ] * v[ W ];
        
        m[ 0 ][ 2 ] =     2 * v[ I ] * v[ K ] - 2 * v[ J ] * v[ W ];
        m[ 1 ][ 2 ] =     2 * v[ J ] * v[ K ] + 2 * v[ I ] * v[ W ];
        m[ 2 ][ 2 ] = 1 - 2 * v[ I ] * v[ I ] - 2 * v[ J ] * v[ J ];
        
        return m;
    }
    
    // Assumes axis is already a unit vector for performance
    template< typename M, typename R, typename A >
    constexpr
    square_matrix< M, 4 > rotation(
        const R& r,
        const vector< A, 3 >& axis
    )
    {
        return rotation< M >( versor< M >::from_euler( r, axis ) );
    }
    
    template< typename M, typename R, typename P, typename Y >
    constexpr
    square_matrix< M, 4 > rotation(
        const R& roll,
        const P& pitch,
        const Y& yaw
    )
    {
        return rotation< M >( versor< M >::from_rpy( roll, pitch, yaw ) );
    }
    
    // // NOTE: Doesn't work yet because units implementation is incomplete
    // template< typename M, typename R >
    // constexpr
    // square_matrix< M, 4 > rotation(
    //     const vector< radians< R >, 3 >& rpy
    // )
    // {
    //     return rotation( versor_from_rpy( rpy[ 0 ], rpy[ 1 ], rpy[ 2 ] ) );
    // }
}


namespace JadeMatrix::yavsg // Scaling operations //////////////////////////////
{
    template< typename M, typename V >
    constexpr
    square_matrix< M, 4 > scaling( const vector< V, 3 >& v )
    {
        auto m = identity_matrix< M, 4 >();
        m[ 0 ][ 0 ] = static_cast< M >( v[ 0 ] );
        m[ 1 ][ 1 ] = static_cast< M >( v[ 1 ] );
        m[ 2 ][ 2 ] = static_cast< M >( v[ 2 ] );
        return m;
    }
    
    template< typename M, typename V >
    constexpr
    square_matrix< M, 4 > scaling( const V& v )
    {
        auto m = identity_matrix< M, 4 >();
        m[ 3 ][ 3 ] = static_cast< M >( v );
        return m;
    }
}


namespace JadeMatrix::yavsg // Translation operations //////////////////////////
{
    template< typename M, typename V >
    constexpr
    square_matrix< M, 4 > translation( const vector< V, 3 >& v )
    {
        auto m = identity_matrix< M, 4 >();
        m[ 3 ][ 0 ] = static_cast< M >( v[ 0 ] );
        m[ 3 ][ 1 ] = static_cast< M >( v[ 1 ] );
        m[ 3 ][ 2 ] = static_cast< M >( v[ 2 ] );
        return m;
    }
}
