#include "units/angular.hpp"
#include "../../include/math/matrix.hpp"
#include "../../include/math/vector.hpp"
#include "../../include/math/trigonometry.hpp"

#include <iostream>
#include <sstream>


int main( int argc, char* argv[] )
{
    // Currently broken because incomplete unit<> implementation
    // yavsg::vector< yavsg::degrees< float >, 3 > deg_vec3( { 0.0f, 0.4f, 127.f } );
    // std::cout
    //     << deg_vec3
    //     << std::endl
    // ;
    
    auto vec3_a = yavsg::vector< double, 3 >::make_filled( 3.1f );
    auto vec3_b = yavsg::vector<    int, 3 >::make_filled( 2    );
    
    std::cout
        << vec3_a
        << " • "
        << vec3_b
        << " => "
        << vec3_a.dot( vec3_b )
        << std::endl
    ;
    
    std::cout
        << "|"
        << vec3_a
        << "| => "
        << vec3_a.magnitude()
        << std::endl
    ;
    
    std::cout
        << "unit("
        << vec3_a
        << ") => "
        << vec3_a.unit()
        << std::endl
    ;
    
    vec3_a *= 7.15f;
    
    std::cout
        << vec3_a
        << std::endl
    ;
    
    std::cout
        << vec3_a
        << " - "
        << vec3_b
        << " => "
        << ( vec3_a - vec3_b )
        << std::endl
    ;
    
    yavsg::matrix< double, 4, 4 > transform_matrix{
        { 8.5f, 2.0f, 2.3f, 3.2f },
        { 7.1f, 1.9f, 7.9f, 1.1f },
        { 0.3f, 2.2f, 0.7f, 4.5f },
        { 0.1f, 0.9f, 8.1f, 1.0f }
    };
    yavsg::vector< double, 4 > transformable_vector{
        3.2009f, 7.17778f, 0.11, 1.06f
    };
    
    std::cout
        << transform_matrix
        << " * "
        << transformable_vector
        << " => "
        << ( transform_matrix * transformable_vector )
        << std::endl
    ;
    
    auto rotate_transform = yavsg::identity_matrix< double, 4 >();
    yavsg::degrees< double > rotate_by = 10;
    // Explicit double needed to convert degres -> radians; should not be a
    // problem in most use cases as there will be an explicit base type in use
    rotate_transform[ 0 ][ 0 ] =  yavsg::cos< double >( rotate_by );
    rotate_transform[ 0 ][ 1 ] =  yavsg::sin< double >( rotate_by );
    rotate_transform[ 1 ][ 0 ] = -yavsg::sin< double >( rotate_by );
    rotate_transform[ 1 ][ 1 ] =  yavsg::cos< double >( rotate_by );
    
    std::cout
        << "rotate "
        << transformable_vector
        << " by "
        << rotate_by
        << ": "
        << rotate_transform
        << " * "
        << transformable_vector
        << " => "
        << ( rotate_transform * transformable_vector )
        << std::endl
    ;
    
    return 0;
}
