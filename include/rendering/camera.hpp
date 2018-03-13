#pragma once
#ifndef YAVSG_RENDERING_CAMERA_HPP
#define YAVSG_RENDERING_CAMERA_HPP


#include "../math/matrix.hpp"
#include "../math/vector.hpp"
#include "../math/basic_transforms.hpp"
#include "../math/common_transforms.hpp"
#include "../math/quaternion.hpp"
#include "../units/angular.hpp"


namespace yavsg
{
    class camera
    {
    protected:
        vector< float, 3 > _position;
        vector< float, 3 > _focus;
        float _near_point;
        float  _far_point;
        degrees< float > _fov;
        
    public:
        camera(
            const vector< float, 3 >& position,
            float  near_point,
            float focal_point,
            float   far_point
        );
        camera(
            const vector< float, 3 >& position,
            const vector< float, 3 >& points
        ) : camera( position, points[ 0 ], points[ 1 ], points[ 2 ] ) {}
        
        vector< float, 3 > position(                           ) const;
        vector< float, 3 > position( const vector< float, 3 >& )      ;
        vector< float, 3 >     move( const vector< float, 3 >& )      ;
        vector< float, 3 >     move(               float       )      ;
        
        // TODO: linear units
        float  near_point(       ) const;
        float focal_point(       ) const;
        float   far_point(       ) const;
        float  near_point( float )      ;
        float focal_point( float )      ;
        float   far_point( float )      ;
        
        degrees< float >   fov(                  ) const;
        // degrees< float > pitch(                  ) const;
        // degrees< float >   yaw(                  ) const;
        degrees< float >   fov( degrees< float >, bool focal_relative = false );
        // degrees< float > pitch( degrees< float > )      ;
        // degrees< float >   yaw( degrees< float > )      ;
        
        // Does not return a unit vector; use direction().unit() if needed
        vector< float, 3 > direction() const;
        
        template< typename T >        vector< T, 3 >     points() const;
        template< typename T > square_matrix< T, 4 >       view() const;
        template< typename T, typename R > square_matrix< T, 4 > projection(
            const R& aspect_ratio
        ) const;
        
        void look_at(
            const vector< float, 3 >& point,
            bool adjust_focal = true
        );
    };
}


namespace yavsg // Template member function implementations ////////////////////
{
    template< typename T > vector< T, 3 > camera::points() const
    {
        return {
             near_point(),
            focal_point(),
              far_point()
        };
    }
    
    template< typename T > square_matrix< T, 4 > camera::view() const
    {
        return yavsg::look_at< T >(
            _position,
            _focus,
            yavsg::vector< T, 3 >(  0.0f,  0.0f,  1.0f )
        );
    }
    
    template<
        typename T,
        typename R
    > square_matrix< T, 4 > camera::projection( const R& aspect_ratio ) const
    {
        auto ar = static_cast< ratio< T > >( aspect_ratio );
        return perspective< T >(
            _fov,
            aspect_ratio,
            _near_point,
            _far_point
        );
    }
}


#endif
