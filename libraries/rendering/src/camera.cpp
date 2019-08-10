#include <yavsg/rendering/camera.hpp>

#include <yavsg/math/trigonometry.hpp>


namespace
{
    static const auto max_biaxis_pitch = yavsg::degrees< float >{ 89.999f };
    
    yavsg::degrees< float > clamp_pitch( yavsg::degrees< float > pitch )
    {
        if( pitch > max_biaxis_pitch )
            return max_biaxis_pitch;
        else if( pitch < -max_biaxis_pitch )
            return -max_biaxis_pitch;
        else
            return pitch;
    }
}


namespace yavsg
{
    camera::camera(
        const vector< float, 3 >& position,
        float  near_point,
        float focal_point,
        float   far_point
    ) :
          _position(    position ),
        _near_point(  near_point ),
         _far_point(   far_point ),
               _fov(          90 ),
        _relative_focus( focal_point * vector< float, 3 >{ 1, 0, 0 } )
    {}
    
    vector< float, 3 > camera::position() const
    {
        std::lock_guard< std::recursive_mutex > _lock(
            const_cast< std::recursive_mutex& >( _mutex )
        );
        return _position;
    }
    
    vector< float, 3 > camera::position( const vector< float, 3 >& p )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        return ( _position = p );
    }
    
    vector< float, 3 > camera::move( const vector< float, 3 >& by )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        return ( _position += by );
    }
    
    vector< float, 3 > camera::move( float by )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        return ( _position += ( by * _relative_focus.unit() ) );
    }
    
    float camera::near_point() const
    {
        std::lock_guard< std::recursive_mutex > _lock(
            const_cast< std::recursive_mutex& >( _mutex )
        );
        return _near_point;
    }
    
    float camera::focal_point() const
    {
        std::lock_guard< std::recursive_mutex > _lock(
            const_cast< std::recursive_mutex& >( _mutex )
        );
        return _relative_focus.magnitude();
    }
    
    float camera::far_point() const
    {
        std::lock_guard< std::recursive_mutex > _lock(
            const_cast< std::recursive_mutex& >( _mutex )
        );
        return _far_point;
    }
    
    float camera::near_point( float p )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        return ( _near_point = p );
    }
    
    float camera::focal_point( float p )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        _relative_focus = _relative_focus.unit() * p;
        return _relative_focus.magnitude();
    }
    
    float camera::far_point( float p )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        return ( _far_point = p );
    }
    
    degrees< float > camera::fov() const
    {
        std::lock_guard< std::recursive_mutex > _lock(
            const_cast< std::recursive_mutex& >( _mutex )
        );
        return _fov;
    }
    
    degrees< float > camera::pitch() const
    {
        std::lock_guard< std::recursive_mutex > _lock(
            const_cast< std::recursive_mutex& >( _mutex )
        );
        return arctan< float >(
            _relative_focus[ 2 ],
            vector< float, 2 >{
                _relative_focus[ 0 ],
                _relative_focus[ 1 ]
            }.magnitude()
        );
    }
    
    degrees< float > camera::yaw() const
    {
        std::lock_guard< std::recursive_mutex > _lock(
            const_cast< std::recursive_mutex& >( _mutex )
        );
        return arctan< float >(
            _relative_focus[ 1 ],
            _relative_focus[ 0 ]
        );
    }
    
    degrees< float > camera::fov(
        degrees< float > angle,
        bool focal_relative
    )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        
        if( focal_relative )
        {
            auto old_focus_abs = _relative_focus + _position;
            move(
                focal_point()
                * ( 1 - (
                      tan< float >(  _fov / 2 )
                    / tan< float >( angle / 2 )
                ) )
            );
            look_at( old_focus_abs );
        }
        
        return ( _fov = angle );
    }
    
    degrees< float > camera::pitch( degrees< float > a )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        pitch_yaw( a, yaw() );
        return yaw();
    }
    
    degrees< float > camera::yaw( degrees< float > a )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        pitch_yaw( pitch(), a );
        return yaw();
    }
    
    void camera::pitch_yaw(
        degrees< float > pitch_angle,
        degrees< float > yaw_angle
    )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        
        auto new_focus = rotation< float >(
            clamp_pitch( pitch_angle ),
            vector< float, 3 >{ 0.0f, 1.0f, 0.0f }
        ) * rotation< float >(
            yaw_angle,
            vector< float, 3 >{ 0.0f, 0.0f, 1.0f }
        ) * vector< float, 4 >{
            focal_point(),
            0.0f,
            0.0f,
            0.0f
        };
        
        _relative_focus = vector< float, 3 >{
            new_focus[ 0 ],
            new_focus[ 1 ],
            new_focus[ 2 ]
        };
    }
    
    degrees< float > camera::increment_pitch( degrees< float > a )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        return pitch( a + pitch() );
    }
    
    degrees< float > camera::increment_yaw( degrees< float > a )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        return yaw( a + yaw() );
    }
    
    void camera::increment_pitch_yaw(
        degrees< float > pitch_angle,
        degrees< float > yaw_angle
    )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        
        pitch_yaw(
            pitch_angle + pitch(),
              yaw_angle +   yaw()
        );
    }
    
    vector< float, 3 > camera::direction() const
    {
        std::lock_guard< std::recursive_mutex > _lock(
            const_cast< std::recursive_mutex& >( _mutex )
        );
        return _relative_focus;
    }
    
    void camera::look_at(
        const vector< float, 3 >& point,
        bool adjust_focal
    )
    {
        std::lock_guard< std::recursive_mutex > _lock( _mutex );
        auto fp = focal_point();
        _relative_focus = point - _position;
        if( !adjust_focal )
            focal_point( fp );
    }
}
