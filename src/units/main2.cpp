#include "units/angular.hpp"
#include "units/prefixes.hpp"
#include "../../include/math/matrix.hpp"

#include <iostream>
#include <sstream>


int main( int argc, char* argv[] )
{
    yavsg::degrees< double > some_degrees = 39.081f;
    
    std::cout
        << some_degrees
        << " ("
        << ( double )some_degrees
        << " "
        << decltype( some_degrees )::unit_name()
        << "(s))"
        << std::endl
    ;
    
    auto degrees_ratio = some_degrees / some_degrees;
    
    std::cout
        << degrees_ratio
        << " ("
        << ( double )degrees_ratio
        << " "
        << decltype( degrees_ratio )::unit_name()
        << "(s) aka ratio)"
        << std::endl
    ;
    
    yavsg::arcminutes< double > some_arcminutes = 389.1f;
    auto degrees_per_arcminute = some_degrees / some_arcminutes;
    
    std::cout
        << degrees_per_arcminute
        << " ("
        << ( double )degrees_per_arcminute
        << " "
        << decltype( degrees_per_arcminute )::unit_name()
        << "(s))"
        << std::endl
    ;
    
    auto degrees_squared = some_degrees * some_degrees;
    
    std::cout
        << degrees_squared
        << " ("
        << ( double )degrees_squared
        << " "
        << decltype( degrees_squared )::unit_name()
        << "(s))"
        << std::endl
    ;
    
    auto degrees_cubed = degrees_squared * some_degrees;
    
    std::cout
        << degrees_cubed
        << " ("
        << ( double )degrees_cubed
        << " "
        << decltype( degrees_cubed )::unit_name()
        << "(s))"
        << std::endl
    ;
    
    auto degrees_quad_power = degrees_squared * degrees_squared;
    
    std::cout
        << degrees_quad_power
        << " ("
        << ( double )degrees_quad_power
        << " "
        << decltype( degrees_quad_power )::unit_name()
        << "(s))"
        << std::endl
    ;
    
    // yavsg::vector< yavsg::degrees< float >, 3 > deg_vec3 = std::array< float, 3 >( { 0.0f, 0.4f, 127.f } );
    
    // std::cout
    //     << deg_vec3
    //     << std::endl
    // ;
    
    auto a_matrix = yavsg::matrix< float, 4, 4 >::make_filled( 0.0f );
    auto col_vector = yavsg::matrix< float, 4, 1 >::make_filled( 0.0f );
    auto row_vector = yavsg::matrix< float, 1, 4 >::make_filled( 0.0f );
    
    std::cout
        << a_matrix
        << std::endl
        << col_vector
        << std::endl
        << row_vector
        << std::endl
    ;
    
    std::cout
        << ( a_matrix + 3 )
        << std::endl
        << ( 3 * yavsg::square_matrix< float, 4 >::make_filled( 1.0f ) )
        << std::endl
    ;
    
    std::cout
        << yavsg::identity_matrix< int, 3 >()
        << std::endl
    ;
    
    yavsg::matrix< float, 2, 4 > matrix_2_4 = {
        { 1.3f, 7.3f, 0.2f, 4.0f },
        { 0.9f, 1.1f, 3.1f, 5.4f }
    };
    yavsg::matrix< float, 4, 2 > matrix_4_2 = {
        { 8.5f, 2.0f },
        { 7.1f, 1.9f },
        { 0.3f, 2.2f },
        { 0.1f, 0.9f }
    };
    
    std::cout
        << matrix_2_4
        << " * "
        << matrix_4_2
        << " => "
        << ( matrix_2_4 * matrix_4_2 )
        << std::endl
        
        << matrix_4_2
        << " * "
        << matrix_2_4
        << " => "
        << ( matrix_4_2 * matrix_2_4 )
        << std::endl
        
        << matrix_2_4
        << "^-1 => "
        << matrix_2_4.transposed()
        << std::endl
    ;
    
    std::cout
        << yavsg::pretty_string( matrix_4_2 * matrix_2_4 )
        << std::endl
    ;
    
    std::cout
        << ( matrix_2_4 == matrix_2_4 )
        << std::endl
        << ( matrix_2_4 != matrix_2_4 )
        << std::endl
    ;
    
    auto mat4 = yavsg::square_matrix< float, 4 >::make_filled( 1.0f );
    mat4 = yavsg::square_matrix< int, 4 >::make_filled( 1 );
    
    return 0;
}
