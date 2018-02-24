#include "units/angular.hpp"
#include "../../include/math/quaternion.hpp"
#include "../../include/math/transforms.hpp"

#include <iostream>
#include <sstream>


template< typename T > void format_some(
    std::ostream& out,
    const T* data,
    unsigned int count
)
{
    for( unsigned int i = 0; i < count; ++i )
    {
        out << data[ i ];
        if( i + 1 < count )
            out << ", ";
    }
}


int main( int argc, char* argv[] )
{
    yavsg::quaternion< double > q1( { 1.0f, 0.0f, 0.0f, 0.0f } );
    yavsg::quaternion< double > q2( { 1.0f, 0.7f, 9.0f, 3.5f } );
    
    std::cout
        << q1
        << std::endl
        << q2
        << std::endl
    ;
    
    std::cout
        << q1
        << " * "
        << q2
        << " => "
        << ( q1 * q2 )
        << std::endl
    ;
    
    std::cout
        << "norm("
        << ( q1 * q2 )
        << ") => "
        << ( q1 * q2 ).norm()
        << std::endl
    ;
    
    yavsg::quaternion< int > q3 = q2;
    yavsg::versor< double >  v1 = ( yavsg::versor< double > )q2;
    
    std::cout
        << q3
        << std::endl
        << v1
        << std::endl
    ;
    
    std::cout
        << v1
        << " => "
        << yavsg::rotation< float >( v1 )
        << std::endl
    ;
    
    yavsg::degrees< double > rotate_degrees = 10;
    yavsg::degrees< double > rotate_0       = 0;
    
    auto rotate_versor = yavsg::versor_from_rpy< double, double, double, double >( rotate_0, rotate_0, rotate_degrees );
    auto rotate_matrix = yavsg::rotation< double, double, double, double >( rotate_0, rotate_0, rotate_degrees );
    
    std::cout
        << rotate_degrees
        << " yaw = "
        << rotate_versor
        << " = "
        << rotate_matrix
        << std::endl
    ;
    
    std::cout
        << "matrix data: "
    ;
    format_some( std::cout, rotate_matrix.data(), 4 * 4 );
    std::cout
        << std::endl
        << "vector data: "
    ;
    format_some( std::cout, yavsg::vector< int, 2 >( { 1, 2 } ).data(), 2 );
    
    // yavsg::matrix< int, 2 > passthrough_init_matrix{
    //         { 2, 9 },
    //         { 0, 3 }
    //     };
    
    return 0;
}
