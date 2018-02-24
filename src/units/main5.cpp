// DEBUG:
#include <iostream>

#include "../../include/math/matrix.hpp"
#include "../../include/math/common_transforms.hpp"

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
    // std::cout
    //     << "creating matrix"
    //     << std::endl
    // ;
    
    // yavsg::matrix< int, 2, 3 > m1 = {
    //     { 1, 2, 3, 4 },
    //     { 1, 2, 3, 4 },
    //     { 1, 2, 3, 4 },
    //     { 1, 2, 3, 4 }
    // };
    
    // // std::cout
    // //     << "  row 0 col 0 = " << m1[ 0 ][ 0 ]
    // //     << std::endl
    // //     << "  row 1 col 0 = " << m1[ 0 ][ 1 ]
    // //     << std::endl
    // //     << "  row 0 col 1 = " << m1[ 1 ][ 0 ]
    // //     << std::endl
    // //     << "  row 1 col 1 = " << m1[ 1 ][ 1 ]
    // //     << std::endl
    // //     << "  row 0 col 2 = " << m1[ 2 ][ 0 ]
    // //     << std::endl
    // //     << "  row 1 col 2 = " << m1[ 2 ][ 1 ]
    // //     << std::endl
    // // ;
    
    // std::cout
    //     << "printing matrix"
    //     << std::endl
    // ;
    
    // std::cout
    //     << yavsg::pretty_string( m1 )
    //     << std::endl
    // ;
    
    // yavsg::matrix< float, 2, 2 > m2 = {
    //     { 0.111009f, -987.9f },
    //     { 1.4f, 99135342.888f }
    // };
    
    // std::cout
    //     << yavsg::pretty_string( m2 )
    //     << std::endl
    // ;
    
    
    
    
    
    // auto zeros = yavsg::vector< float, 3 >(  0.0f,  0.0f,  0.0f );
    // auto  twos = yavsg::vector< float, 3 >(  2.2f,  2.2f,  2.2f );
    // // auto ntwos = yavsg::vector< float, 3 >( -2.2f, -2.2f, -2.2f );
    
    // auto position = yavsg::vector< float, 3 >(  2.2f,  2.2f,  2.2f );//twos;
    // auto    focus = yavsg::vector< float, 3 >(  0.0f,  0.0f,  0.0f );//zeros;
    // auto       up = yavsg::vector< float, 3 >(  0.0f,  0.0f,  1.0f );
    
    // std::cout
    //     <<  "position ==  twos: "
    //     << ( position ==  twos   )
    //     << std::endl
    //     <<  "   focus == zeros: "
    //     << (    focus == zeros   )
    //     << std::endl
    //     // <<  "position == twos: "
    //     // << ( position == twos )
    //     // << std::endl
    //     << std::endl
    // ;
    
    // std::cout
    //     << "position = "
    //     << position
    //     << std::endl
    //     << "   focus = "
    //     << focus
    //     << std::endl
    //     << std::endl
    // ;
    
    // auto camera_axis =              (         focus - position ).unit();
    // auto           s = cross_product( camera_axis, up          ).unit();
    // auto           u = cross_product(           s, camera_axis ).unit();
    
    // std::cout
    //     << "camera_axis = "
    //     << camera_axis
    //     << std::endl
    //     << "          s = "
    //     << s
    //     << std::endl
    //     << "          u = "
    //     << u
    //     << std::endl
    //     << std::endl
    // ;
    
    // auto ds = -(           s.dot( position ) );
    // auto du = -(           u.dot( position ) );
    // auto dc =  ( camera_axis.dot( position ) );
    
    // std::cout
    //     << "ds = "
    //     << ds
    //     << std::endl
    //     << "du = "
    //     << du
    //     << std::endl
    //     << "dc = "
    //     << dc
    //     << std::endl
    //     << std::endl
    // ;
    
    // yavsg::square_matrix< float, 4 > m = {
    //     {            s[ 0 ],            s[ 1 ],            s[ 2 ],   ds },
    //     {            u[ 0 ],            u[ 1 ],            u[ 2 ],   du },
    //     { -camera_axis[ 0 ], -camera_axis[ 1 ], -camera_axis[ 2 ],   dc },
    //     {              0.0f,              0.0f,              0.0f, 1.0f }
    // };
    
    // std::cout
    //     << yavsg::pretty_string( m )
    //     << std::endl
    //     << std::endl
    // ;
    
    
    auto position = yavsg::vector< float, 3 >(  2.2f,  2.2f,  2.2f );
    auto    focus = yavsg::vector< float, 3 >(  0.0f,  0.0f,  0.0f );
    auto       up = yavsg::vector< float, 3 >(  0.0f,  0.0f,  1.0f );
    auto focus_minus_position = focus - position;
    
    std::cout
        << "focus"
        << focus
        << " - position"
        << position
        << " => "
        << ( focus - position )
        << " == "
        << focus_minus_position
        << std::endl
        << std::endl
        
        << yavsg::pretty_string(
            yavsg::look_at< float >(
                yavsg::vector< float, 3 >( -2.2f, -2.2f, -2.2f ), // Look-at point
                yavsg::vector< float, 3 >(  0.0f,  0.0f,  1.0f )  // Up vector
            ) * yavsg::translation< float >(
                yavsg::vector< float, 3 >( -2.2f, -2.2f, -2.2f )  // Move scene
            )
        )
        << std::endl
        << yavsg::pretty_string( yavsg::look_at< float >(
            yavsg::vector< float, 3 >( 2.2f, 2.2f, 2.2f ), // Camera position
            yavsg::vector< float, 3 >( 0.0f, 0.0f, 0.0f ), // Look-at point
            yavsg::vector< float, 3 >( 0.0f, 0.0f, 1.0f )  // Up vector
            // position,
            // focus,
            // up
            // yavsg::vector< float, 3 >(  2.2f,  2.2f,  2.2f ),
            // yavsg::vector< float, 3 >(  0.0f,  0.0f,  0.0f ),
            // yavsg::vector< float, 3 >(  0.0f,  0.0f,  1.0f )
            // yavsg::vector< float, 3 >(twos),
            // yavsg::vector< float, 3 >(zeros),
            // yavsg::vector< float, 3 >(up)
        ) )
        << std::endl
    ;
    
    return 0;
}
