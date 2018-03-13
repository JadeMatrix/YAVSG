#include "../../include/rendering/camera.hpp"


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
        _focus( focal_point * (
            _position + vector< float, 3 >{ 1, 0, 0 }
        ) )
    {}
    
    vector< float, 3 > camera::position() const
    {
        return _position;
    }
    
    vector< float, 3 > camera::position( const vector< float, 3 >& p )
    {
        return ( _position = p );
    }
    
    vector< float, 3 > camera::move( const vector< float, 3 >& by )
    {
        return ( _position += by );
    }
    
    vector< float, 3 > camera::move( float by )
    {
        return ( _position += ( by * direction().unit() ) );
    }
    
    float camera::near_point() const
    {
        return _near_point;
    }
    
    float camera::focal_point() const
    {
        return direction().magnitude();
    }
    
    float camera::far_point() const
    {
        return _far_point;
    }
    
    float camera::near_point( float p )
    {
        return ( _near_point = p );
    }
    
    float camera::focal_point( float p )
    {
        _focus = _position + ( direction().unit() * p );
        return direction().magnitude();
    }
    
    float camera::far_point( float p )
    {
        return ( _far_point = p );
    }
    
    degrees< float > camera::fov() const
    {
        return _fov;
    }
    
    // degrees< float > camera::yaw() const
    // {
    //     return _yaw;
    // }
    
    // degrees< float > camera::pitch() const
    // {
    //     return _pitch;
    // }
    
    degrees< float > camera::fov(
        degrees< float > angle,
        bool focal_relative
    )
    {
        // TODO: focal_relative
        return ( _fov = angle );
    }
    
    // degrees< float > camera::yaw( degrees< float > y )
    // {
    //     return ( _yaw = y );
    // }
    
    // degrees< float > camera::pitch( degrees< float > p )
    // {
    //     return ( _pitch = p );
    // }
    
    vector< float, 3 > camera::direction() const
    {
        return _focus - _position;
    }
    
    void camera::look_at(
        const vector< float, 3 >& point,
        bool adjust_focal
    )
    {
        auto fp = focal_point();
        _focus = point;
        if( !adjust_focal )
            focal_point( fp );
    }
}
