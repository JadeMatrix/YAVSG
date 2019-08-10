#pragma once
#ifndef YAVSG_MATH_COMMON_TRANSFORMS_HPP
#define YAVSG_MATH_COMMON_TRANSFORMS_HPP


#include "matrix.hpp"
#include "quaternion.hpp"
#include "trigonometry.hpp"
#include "vector.hpp"

#include <yavsg/units/angular.hpp>


namespace yavsg // Views ///////////////////////////////////////////////////////
{
    // Look-at with camera at 0,0,0 that expects the world to be transformed
    // separately
    template< typename M, typename V1, typename V2 >
    constexpr
    square_matrix< M, 4 > look_at(
        const vector< V1, 3 >& focus,
        const vector< V2, 3 >& up
    )
    {
        auto camera_axis =                                     focus.unit();
        auto           s = cross_product( camera_axis, up          ).unit();
        auto           u = cross_product(           s, camera_axis ).unit();
        
        return {
            { static_cast< M >( s[ 0 ] ), static_cast< M >( u[ 0 ] ), static_cast< M >( -camera_axis[ 0 ] ), 0 },
            { static_cast< M >( s[ 1 ] ), static_cast< M >( u[ 1 ] ), static_cast< M >( -camera_axis[ 1 ] ), 0 },
            { static_cast< M >( s[ 2 ] ), static_cast< M >( u[ 2 ] ), static_cast< M >( -camera_axis[ 2 ] ), 0 },
            {                          0,                          0,                                     0, 1 }
        };
    }
    
    // Look-at transformation similar to gluLookAt()
    template< typename M, typename V1, typename V2, typename V3 >
    constexpr
    square_matrix< M, 4 > look_at(
        const vector< V1, 3 >& position,
        const vector< V2, 3 >& focus,
        const vector< V3, 3 >& up
    )
    {
        auto camera_axis =              (         focus - position ).unit();
        auto           s = cross_product( camera_axis, up          ).unit();
        auto           u = cross_product(           s, camera_axis ).unit();
        
        auto ds = -(           s.dot( position ) );
        auto du = -(           u.dot( position ) );
        auto dc =  ( camera_axis.dot( position ) );
        
        return {
            { static_cast< M >( s[ 0 ] ), static_cast< M >( u[ 0 ] ), static_cast< M >( -camera_axis[ 0 ] ), 0 },
            { static_cast< M >( s[ 1 ] ), static_cast< M >( u[ 1 ] ), static_cast< M >( -camera_axis[ 1 ] ), 0 },
            { static_cast< M >( s[ 2 ] ), static_cast< M >( u[ 2 ] ), static_cast< M >( -camera_axis[ 2 ] ), 0 },
            { static_cast< M >(     ds ), static_cast< M >(     du ), static_cast< M >(                dc ), 1 }
        };
    }
}


namespace yavsg // Projections /////////////////////////////////////////////////
{
    // Basic orthographic with 0,0 at the center
    template<
        typename M,
        typename R,
        typename TNear,
        typename TFar
    >
    constexpr
    square_matrix< M, 4 > orthographic(
        const     R& aspect_ratio,
        const TNear& near,
        const  TFar& far
    )
    {
        auto a = static_cast< ratio< M > >( aspect_ratio );
        auto w = 1 / a;
        auto d = -2 / ( far - near );
        auto r = -( ( far + near ) / ( far - near ) );
        return {
            { static_cast< M >( w ),      0,                     0,      0 },
            {                     0,      1,                     0,      0 },
            {                     0,      0, static_cast< M >( d ),      0 },
            {                     0,      0, static_cast< M >( r ),      1 }
        };
    }
    
    // Orthographic with arbitrary center similar to glOrtho()
    template<
        typename M,
        typename TLeft,
        typename TRight,
        typename TBottom,
        typename TTop,
        typename TNear,
        typename TFar
    >
    constexpr
    square_matrix< M, 4 > orthographic(
        const TLeft  & left,
        const TRight & right,
        const TBottom& bottom,
        const TTop   & top,
        const TNear  & near,
        const TFar   & far
    )
    {
        auto w =  2 / ( right - left   );
        auto h =  2 / (   top - bottom );
        auto d = -2 / (   far - near   );
        auto tx = -( ( right + left   ) / ( right - left   ) );
        auto ty = -( (   top + bottom ) / (   top - bottom ) );
        auto tz = -( (   far + near   ) / (   far - near   ) );
        return {
            { static_cast< M >(  w ),                      0,                      0,  0 },
            {                      0, static_cast< M >(  h ),                      0,  0 },
            {                      0,                      0, static_cast< M >(  d ), -1 },
            { static_cast< M >( tx ), static_cast< M >( ty ), static_cast< M >( tz ),  0 }
        };
    }
    
    // Similar to gluPerspective(), but with horizontal (x) FoV instead of y
    template<
        typename M,
        typename VX,
        typename R,
        typename TNear,
        typename TFar
    >
    constexpr
    square_matrix< M, 4 > perspective(
        const    VX& fov_horizontal,
        const     R& aspect_ratio,
        const TNear& near,
        const  TFar& far
    )
    {
        auto ar = static_cast< ratio< M > >( aspect_ratio );
        auto f  = 1 / tan< M >( fov_horizontal / 2 );
        auto z1 = (     near + far ) / ( near - far );
        auto z2 = ( 2 * near * far ) / ( near - far );
        return {
            { static_cast< M >( f ),                          0,                      0,  0 },
            {                     0, static_cast< M >( f * ar ),                      0,  0 },
            {                     0,                          0, static_cast< M >( z1 ), -1 },
            {                     0,                          0, static_cast< M >( z2 ),  0 }
        };
    }
}


#endif
