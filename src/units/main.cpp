#include "units/angular.hpp"
// #include "units/string_conversions.hpp"
#include "units/prefixes.hpp"

#include <iostream>
#include <sstream>


template< typename T, typename V > std::string degrees_string( const V& d )
{
    // return /*yavsg::to_string*/( yavsg::degrees< T >( d ) );
    std::stringstream ss;
    ss << yavsg::degrees< T >( d );
    return ss.str();
}

template< typename T > using deg_per_rad = yavsg::unit<
    T,
    yavsg::per_traits<
        T,
        yavsg::degree_traits< T >,
        yavsg::radian_traits< T >
    >
>;
template< typename T > using rad_per_rad = yavsg::unit<
    T,
    yavsg::per_traits<
        T,
        yavsg::radian_traits< T >,
        yavsg::radian_traits< T >
    >
>;


int main( int argc, char* argv[] )
{
    yavsg::   degrees< double > some_degrees    = 57.829f;
    yavsg::   radians< double > some_radians    = 57.829f;
    yavsg::arcminutes< double > some_arcminutes = 57.829f;
    
    std::cout
        << /*yavsg::to_string*/( yavsg::   degrees< double >( some_degrees ) )
        << " = "
        << /*yavsg::to_string*/( yavsg::   radians< double >( some_degrees ) )
        << " = "
        << /*yavsg::to_string*/( yavsg::arcminutes< double >( some_degrees ) )
        << std::endl
        
        << /*yavsg::to_string*/( yavsg::   degrees< double >( some_radians ) )
        << " = "
        << /*yavsg::to_string*/( yavsg::   radians< double >( some_radians ) )
        << " = "
        << /*yavsg::to_string*/( yavsg::arcminutes< double >( some_radians ) )
        << std::endl
        
        << /*yavsg::to_string*/( yavsg::   degrees< double >( some_arcminutes ) )
        << " = "
        << /*yavsg::to_string*/( yavsg::   radians< double >( some_arcminutes ) )
        << " = "
        << /*yavsg::to_string*/( yavsg::arcminutes< double >( some_arcminutes ) )
        << std::endl
    ;
    
    some_arcminutes = some_degrees;
    std::cout
        << /*yavsg::to_string*/( some_arcminutes )
        << std::endl
    ;
    
    std::cout
        << degrees_string< int >( some_arcminutes )
        << std::endl
    ;
    
    std::cout
        << /*yavsg::to_string*/( some_degrees )
        << " == "
        << /*yavsg::to_string*/( some_radians )
        << " => "
        << ( some_degrees == some_radians )
        << std::endl
    ;
    
    std::cout
        << "-"
        << /*yavsg::to_string*/( some_degrees )
        << " => "
        << /*yavsg::to_string*/( -some_degrees )
        << std::endl
    ;
    
    deg_per_rad< double > dpr = 3.582;
    std::cout
        << /*yavsg::to_string*/( dpr )
        << " == "
        << /*yavsg::to_string*/( rad_per_rad< double >( dpr ) )
        << std::endl
    ;
    
    auto ampd = some_arcminutes / some_degrees;
    auto am_divided = some_arcminutes / 2;
    auto some_ratio = some_arcminutes / yavsg::arcminutes< double >( some_degrees );
    std::cout
        << /*yavsg::to_string*/( some_arcminutes )
        << " / "
        << /*yavsg::to_string*/( some_degrees )
        << " => "
        << /*yavsg::to_string*/( ampd )
        << std::endl
        
        << /*yavsg::to_string*/( some_arcminutes )
        << " / 2 "
        << " => "
        << /*yavsg::to_string*/( am_divided )
        << std::endl
        
        << /*yavsg::to_string*/( some_arcminutes )
        << " / "
        << /*yavsg::to_string*/( yavsg::arcminutes< double >( some_degrees ) )
        << " => "
        << /*yavsg::to_string*/( some_ratio )
        << std::endl
    ;
    
    // Ratios can implicitly convert to other types
    float float_ration = some_ratio;
    
    yavsg::decaarcminutes< double > asdf = some_arcminutes;
    // yavsg::deca< float, yavsg::arcminute_traits< float > > asdf = some_arcminutes;
    std::cout
        << /*yavsg::to_string*/( asdf )
        << " ("
        << ( double )asdf
        << " "
        << decltype( asdf )::traits_type::unit_name()
        << ")"
        << std::endl
    ;
    
    return 0;
}
