#pragma once


#include <yavsg/math/matrix.hpp>
#include <yavsg/math/vector.hpp>
#include <yavsg/math/basic_transforms.hpp>
#include <yavsg/math/common_transforms.hpp>
#include <yavsg/math/quaternion.hpp>
#include <yavsg/units/angular.hpp>

#include <mutex>    // recursive_mutex, unique_lock


namespace JadeMatrix::yavsg
{
    class camera
    {
    public:
        camera(
            vector< float, 3 > const& position,
            float  near_point,
            float focal_point,
            float   far_point
        );
        camera(
            vector< float, 3 > const& position,
            vector< float, 3 > const& points
        ) : camera( position, points[ 0 ], points[ 1 ], points[ 2 ] ) {}
        
        vector< float, 3 > position(                           ) const;
        vector< float, 3 > position( vector< float, 3 > const& )      ;
        vector< float, 3 >     move( vector< float, 3 > const& )      ;
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
            R const& aspect_ratio
        ) const;
        
        void look_at(
            vector< float, 3 > const& point,
            bool adjust_focal = true
        );
        
    protected:
        vector< float, 3 > position_;
        vector< float, 3 > relative_focus_;
        float near_point_;
        float  far_point_;
        degrees< float > fov_;
        
        // Simpler to use a recursive mutex as some functions call others here
        std::recursive_mutex mutable mutex_;
    };
}


namespace JadeMatrix::yavsg // Template member function implementations ////////
{
    template< typename T > vector< T, 3 > camera::points() const
    {
        std::unique_lock lock( mutex_ );
        return {
             near_point(),
            focal_point(),
              far_point()
        };
    }
    
    template< typename T > square_matrix< T, 4 > camera::view() const
    {
        std::unique_lock lock( mutex_ );
        return yavsg::look_at< T >(
            position_,
            position_ + relative_focus_,
            yavsg::vector< T, 3 >(  0.0f,  0.0f,  1.0f )
        );
    }
    
    template<
        typename T,
        typename R
    > square_matrix< T, 4 > camera::projection( const R& aspect_ratio ) const
    {
        std::unique_lock lock( mutex_ );
        return perspective< T >(
            fov_,
            aspect_ratio,
            near_point_,
            far_point_
        );
    }
}
