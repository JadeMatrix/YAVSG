#pragma once
#ifndef YAVSG_MATH_COMMON_TRANSFORMS_HPP
#define YAVSG_MATH_COMMON_TRANSFORMS_HPP


#include "transforms.hpp"


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
            { ( M )(            s[ 0 ] ), ( M )(            s[ 1 ] ), ( M )(            s[ 2 ] ), 0 },
            { ( M )(            u[ 0 ] ), ( M )(            u[ 1 ] ), ( M )(            u[ 2 ] ), 0 },
            { ( M )( -camera_axis[ 0 ] ), ( M )( -camera_axis[ 1 ] ), ( M )( -camera_axis[ 2 ] ), 0 },
            {                          0,                          0,                          0, 1 }
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
            { ( M )(            s[ 0 ] ), ( M )(            s[ 1 ] ), ( M )(            s[ 2 ] ), ( M )ds },
            { ( M )(            u[ 0 ] ), ( M )(            u[ 1 ] ), ( M )(            u[ 2 ] ), ( M )du },
            { ( M )( -camera_axis[ 0 ] ), ( M )( -camera_axis[ 1 ] ), ( M )( -camera_axis[ 2 ] ), ( M )dc },
            {                          0,                          0,                          0,       1 }
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
        const ratio< R >& aspect_ratio,
        const      TNear& near,
        const       TFar& far
    )
    {
        auto w = 1 / aspect_ratio;
        auto d = -2 / ( far - near );
        auto r = -( ( far + near ) / ( far - near ) );
        return {
            { ( M )w,      0,      0,      0 },
            {      0,      1,      0,      0 },
            {      0,      0, ( M )d, ( M )r },
            {      0,      0,      0,      1 }
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
            { ( M )w,      0,      0, ( M )tx },
            {      0, ( M )h,      0, ( M )ty },
            {      0,      0, ( M )d, ( M )tz },
            {      0,      0,     -1,       0 }
        };
    }
    
    // Perspective with independent vertical & horizontal FoV 
    template<
        typename M,
        typename Vx,
        typename Vy,
        typename TNear,
        typename TFar
    >
    constexpr
    square_matrix< M, 4 > perspective(
        const radians< Vx >& fov_horizontal,
        const radians< Vy >& fov_vertical,
        const         TNear& near,
        const          TFar& far
    )
    {
        auto fx = 1 / tan( fov_horizontal / 2 );
        auto fy = 1 / tan( fov_vertical   / 2 );
        auto z1 = (     near + far ) / ( near - far );
        auto z2 = ( 2 * near * far ) / ( near - far );
        return {
            { ( M )fx,       0,       0,       0 },
            {       0, ( M )fy,       0,       0 },
            {       0,       0, ( M )z1, ( M )z2 },
            {       0,       0,      -1,       0 }
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
        const radians< VX >& fov_horizontal,
        const    ratio< R >& aspect_ratio,
        const         TNear& near,
        const          TFar& far
    )
    {
        auto f  = 1 / tan( fov_horizontal / 2 );
        auto z1 = (     near + far ) / ( near - far );
        auto z2 = ( 2 * near * far ) / ( near - far );
        return {
            { ( M )f,                        0,       0,       0 },
            {      0, ( M )( f * aspect_ratio),       0,       0 },
            {      0,                        0, ( M )z1, ( M )z2 },
            {      0,                        0,      -1,       0 }
        };
    }
}


#endif
