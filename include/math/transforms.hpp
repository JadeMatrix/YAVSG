#pragma once
#ifndef YAVSG_MATH_TRANSFORMS_HPP
#define YAVSG_MATH_TRANSFORMS_HPP


#include "matrix.hpp"
#include "quaternion.hpp"
#include "trigonometry.hpp"
#include "vector.hpp"
#include "../units/angular.hpp"


// TODO: Return values decltype()ed once unit implementation is finished


namespace yavsg // Rotation models to matrix conversions ///////////////////////
{
    // Assumes axis is already a unit vector for performance
    template< typename V, typename R, typename A >
    constexpr
    versor< V > versor_from_euler(
        const radians< R >& r,
        const vector< A, 3 >& axis
    )
    {
        return {
            ( V )( axis[ 0 ] * sin( r / 2 ) ),
            ( V )( axis[ 1 ] * sin( r / 2 ) ),
            ( V )( axis[ 2 ] * sin( r / 2 ) ),
            ( V )(             cos( r / 2 ) )
        };
    }
    
    template< typename V, typename R_Roll, typename R_Pitch, typename R_Yaw >
    constexpr
    versor< V > versor_from_rpy(
        const radians< R_Roll  >& roll,
        const radians< R_Pitch >& pitch,
        const radians< R_Yaw   >& yaw
    )
    {
        return {
            ( V )( cos( roll / 2 ) * cos( pitch / 2 ) * cos( yaw / 2 ) + sin( roll / 2 ) * sin( pitch / 2 ) * sin( yaw / 2 ) ),
            ( V )( sin( roll / 2 ) * cos( pitch / 2 ) * cos( yaw / 2 ) - cos( roll / 2 ) * sin( pitch / 2 ) * sin( yaw / 2 ) ),
            ( V )( cos( roll / 2 ) * sin( pitch / 2 ) * cos( yaw / 2 ) + sin( roll / 2 ) * cos( pitch / 2 ) * sin( yaw / 2 ) ),
            ( V )( cos( roll / 2 ) * cos( pitch / 2 ) * sin( yaw / 2 ) - sin( roll / 2 ) * sin( pitch / 2 ) * cos( yaw / 2 ) )
        };
    }
}


namespace yavsg // Rotation operations /////////////////////////////////////////
{
    template< typename M, typename V >
    constexpr
    square_matrix< M, 4 > rotation( const versor< V >& v )
    {
        auto m = identity_matrix< M, 4 >();
        
        m[ 0 ][ 0 ] = 1 - 2 * v[ J ] * v[ J ] - 2 * v[ K ] * v[ K ];
        m[ 1 ][ 0 ] =     2 * v[ I ] * v[ J ] - 2 * v[ K ] * v[ W ];
        m[ 2 ][ 0 ] =     2 * v[ I ] * v[ K ] - 2 * v[ J ] * v[ W ];
        
        m[ 0 ][ 1 ] =     2 * v[ I ] * v[ J ] - 2 * v[ K ] * v[ W ];
        m[ 1 ][ 1 ] = 1 - 2 * v[ I ] * v[ I ] - 2 * v[ K ] * v[ K ];
        m[ 2 ][ 1 ] =     2 * v[ J ] * v[ K ] - 2 * v[ I ] * v[ W ];
        
        m[ 0 ][ 2 ] =     2 * v[ I ] * v[ K ] - 2 * v[ J ] * v[ W ];
        m[ 1 ][ 2 ] =     2 * v[ J ] * v[ K ] - 2 * v[ I ] * v[ W ];
        m[ 2 ][ 2 ] = 1 - 2 * v[ I ] * v[ I ] - 2 * v[ J ] * v[ J ];
        
        return m;
    }
    
    // Assumes axis is already a unit vector for performance
    template< typename M, typename R, typename A >
    constexpr
    square_matrix< M, 4 > rotation(
        const radians< R >& r,
        const vector< A, 3 >& axis
    )
    {
        return rotation< M, M >( versor_from_euler( r, axis ) );
    }
    
    template< typename M, typename R_Roll, typename R_Pitch, typename R_Yaw >
    constexpr
    square_matrix< M, 4 > rotation(
        const radians< R_Roll  >& roll,
        const radians< R_Pitch >& pitch,
        const radians< R_Yaw   >& yaw
    )
    {
        return rotation< M, M >( versor_from_rpy< M >( roll, pitch, yaw ) );
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


namespace yavsg // Scaling operations //////////////////////////////////////////
{
    template< typename M, typename V >
    constexpr
    square_matrix< M, 4 > scaling( const vector< V, 3 >& v )
    {
        auto m = identity_matrix< M, 4 >();
        m[ 0 ][ 0 ] = ( V )v[ 0 ];
        m[ 1 ][ 1 ] = ( V )v[ 1 ];
        m[ 2 ][ 2 ] = ( V )v[ 2 ];
        return m;
    }
    
    template< typename M, typename V >
    constexpr
    square_matrix< M, 4 > scaling( const V& v )
    {
        auto m = identity_matrix< M, 4 >();
        m[ 3 ][ 3 ] = v;
        return m;
    }
}


namespace yavsg // Translation operations //////////////////////////////////////
{
    template< typename M, typename V >
    constexpr
    square_matrix< M, 4 > translation( const vector< V, 3 >& v )
    {
        auto m = identity_matrix< M, 4 >();
        m[ 3 ][ 0 ] = ( V )v[ 0 ];
        m[ 3 ][ 1 ] = ( V )v[ 1 ];
        m[ 3 ][ 2 ] = ( V )v[ 2 ];
        return m;
    }
}


#endif
