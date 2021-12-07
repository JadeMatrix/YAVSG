#include <yavsg/rendering/camera.hpp>

#include <yavsg/math/trigonometry.hpp>


namespace
{
    constexpr auto max_biaxis_pitch = JadeMatrix::yavsg::degrees< float >{
        89.999f
    };
    
    JadeMatrix::yavsg::degrees< float > clamp_pitch(
        JadeMatrix::yavsg::degrees< float > pitch
    )
    {
             if( pitch >  max_biaxis_pitch ) return  max_biaxis_pitch;
        else if( pitch < -max_biaxis_pitch ) return -max_biaxis_pitch;
        else                                 return  pitch;
    }
}


JadeMatrix::yavsg::camera::camera(
    vector< float, 3 > const& position,
    float  near_point,
    float focal_point,
    float   far_point
) :
    position_( position ),
    relative_focus_( focal_point * vector< float, 3 >{ 1, 0, 0 } ),
    near_point_( near_point ),
    far_point_ (  far_point ),
    fov_( 90 )
{}

JadeMatrix::yavsg::vector< float, 3 >
JadeMatrix::yavsg::camera::position() const
{
    std::unique_lock lock( mutex_ );
    return position_;
}

JadeMatrix::yavsg::vector< float, 3 > JadeMatrix::yavsg::camera::position(
    vector< float, 3 > const& p
)
{
    std::unique_lock lock( mutex_ );
    return ( position_ = p );
}

JadeMatrix::yavsg::vector< float, 3 > JadeMatrix::yavsg::camera::move(
    vector< float, 3 > const& by
)
{
    std::unique_lock lock( mutex_ );
    return ( position_ += by );
}

JadeMatrix::yavsg::vector< float, 3 > JadeMatrix::yavsg::camera::move(
    float by
)
{
    std::unique_lock lock( mutex_ );
    return ( position_ += ( by * relative_focus_.unit() ) );
}

float JadeMatrix::yavsg::camera::near_point() const
{
    std::unique_lock lock( mutex_ );
    return near_point_;
}

float JadeMatrix::yavsg::camera::focal_point() const
{
    std::unique_lock lock( mutex_ );
    return relative_focus_.magnitude();
}

float JadeMatrix::yavsg::camera::far_point() const
{
    std::unique_lock lock( mutex_ );
    return far_point_;
}

float JadeMatrix::yavsg::camera::near_point( float p )
{
    std::unique_lock lock( mutex_ );
    return ( near_point_ = p );
}

float JadeMatrix::yavsg::camera::focal_point( float p )
{
    std::unique_lock lock( mutex_ );
    relative_focus_ = relative_focus_.unit() * p;
    return relative_focus_.magnitude();
}

float JadeMatrix::yavsg::camera::far_point( float p )
{
    std::unique_lock lock( mutex_ );
    return ( far_point_ = p );
}

JadeMatrix::yavsg::degrees< float > JadeMatrix::yavsg::camera::fov() const
{
    std::unique_lock lock( mutex_ );
    return fov_;
}

JadeMatrix::yavsg::degrees< float > JadeMatrix::yavsg::camera::pitch() const
{
    std::unique_lock lock( mutex_ );
    return arctan< float >(
        relative_focus_[ 2 ],
        vector< float, 2 >{
            relative_focus_[ 0 ],
            relative_focus_[ 1 ]
        }.magnitude()
    );
}

JadeMatrix::yavsg::degrees< float > JadeMatrix::yavsg::camera::yaw() const
{
    std::unique_lock lock( mutex_ );
    return arctan< float >(
        relative_focus_[ 1 ],
        relative_focus_[ 0 ]
    );
}

JadeMatrix::yavsg::degrees< float > JadeMatrix::yavsg::camera::fov(
    degrees< float > angle,
    bool             focal_relative
)
{
    std::unique_lock lock( mutex_ );
    
    if( focal_relative )
    {
        auto old_focus_abs = relative_focus_ + position_;
        move(
            focal_point()
            * ( 1 - (
                  tan< float >(  fov_ / 2 )
                / tan< float >( angle / 2 )
            ) )
        );
        look_at( old_focus_abs );
    }
    
    return ( fov_ = angle );
}

JadeMatrix::yavsg::degrees< float > JadeMatrix::yavsg::camera::pitch(
    degrees< float > a
)
{
    std::unique_lock lock( mutex_ );
    pitch_yaw( a, yaw() );
    return yaw();
}

JadeMatrix::yavsg::degrees< float > JadeMatrix::yavsg::camera::yaw(
    degrees< float > a
)
{
    std::unique_lock lock( mutex_ );
    pitch_yaw( pitch(), a );
    return yaw();
}

void JadeMatrix::yavsg::camera::pitch_yaw(
    degrees< float > pitch_angle,
    degrees< float > yaw_angle
)
{
    std::unique_lock lock( mutex_ );
    
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
    
    relative_focus_ = vector< float, 3 >{
        new_focus[ 0 ],
        new_focus[ 1 ],
        new_focus[ 2 ]
    };
}

JadeMatrix::yavsg::degrees< float > JadeMatrix::yavsg::camera::increment_pitch(
    degrees< float > a
)
{
    std::unique_lock lock( mutex_ );
    return pitch( a + pitch() );
}

JadeMatrix::yavsg::degrees< float > JadeMatrix::yavsg::camera::increment_yaw(
    degrees< float > a
)
{
    std::unique_lock lock( mutex_ );
    return yaw( a + yaw() );
}

void JadeMatrix::yavsg::camera::increment_pitch_yaw(
    degrees< float > pitch_angle,
    degrees< float > yaw_angle
)
{
    std::unique_lock lock( mutex_ );
    
    pitch_yaw(
        pitch_angle + pitch(),
          yaw_angle +   yaw()
    );
}

JadeMatrix::yavsg::vector< float, 3 >
JadeMatrix::yavsg::camera::direction() const
{
    std::unique_lock lock( mutex_ );
    return relative_focus_;
}

void JadeMatrix::yavsg::camera::look_at(
    vector< float, 3 > const& point,
    bool adjust_focal
)
{
    std::unique_lock lock( mutex_ );
    auto fp = focal_point();
    relative_focus_ = point - position_;
    if( !adjust_focal )
    {
        focal_point( fp );
    }
}
