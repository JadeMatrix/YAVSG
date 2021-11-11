#pragma once


#include <yavsg/math/matrix.hpp>
#include <yavsg/math/vector.hpp>
#include <yavsg/math/basic_transforms.hpp>
#include <yavsg/math/common_transforms.hpp>
#include <yavsg/math/quaternion.hpp>
#include <yavsg/units/angular.hpp>

#include <mutex>


namespace JadeMatrix::yavsg
{
    class camera
    {
    protected:
        vector< float, 3 > _position;
        vector< float, 3 > _relative_focus;
        float _near_point;
        float  _far_point;
        degrees< float > _fov;
        
        // Simpler to use a recursive mutex as some functions call others here
        std::recursive_mutex _mutex;
        
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
        degrees< float > pitch(                  ) const;
        degrees< float >   yaw(                  ) const;
        degrees< float >   fov( degrees< float >, bool focal_relative = false );
        degrees< float > pitch( degrees< float > )      ;
        degrees< float >   yaw( degrees< float > )      ;
        void pitch_yaw( degrees< float >, degrees< float > );
        
        // Thread-safe incremental pitch & yaw
        degrees< float > increment_pitch( degrees< float > );
        degrees< float > increment_yaw(   degrees< float > );
        void increment_pitch_yaw(
            degrees< float >,
            degrees< float >
        );
        
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


namespace JadeMatrix::yavsg // Template member function implementations ////////
{
    template< typename T > vector< T, 3 > camera::points() const
    {
        std::lock_guard< std::recursive_mutex > _lock(
            const_cast< std::recursive_mutex& >( _mutex )
        );
        return {
             near_point(),
            focal_point(),
              far_point()
        };
    }
    
    template< typename T > square_matrix< T, 4 > camera::view() const
    {
        std::lock_guard< std::recursive_mutex > _lock(
            const_cast< std::recursive_mutex& >( _mutex )
        );
        return yavsg::look_at< T >(
            _position,
            _position + _relative_focus,
            yavsg::vector< T, 3 >(  0.0f,  0.0f,  1.0f )
        );
    }
    
    template<
        typename T,
        typename R
    > square_matrix< T, 4 > camera::projection( const R& aspect_ratio ) const
    {
        std::lock_guard< std::recursive_mutex > _lock(
            const_cast< std::recursive_mutex& >( _mutex )
        );
        return perspective< T >(
            _fov,
            aspect_ratio,
            _near_point,
            _far_point
        );
    }
}
