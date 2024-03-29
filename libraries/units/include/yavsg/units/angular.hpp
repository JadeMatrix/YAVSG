#pragma once


#include "constants.hpp"
#include "unit.hpp"
#include "prefixes.hpp"

#include <string>


namespace JadeMatrix::yavsg // Declarations ////////////////////////////////////
{
    template< typename T > struct    degree_traits;
    template< typename T > struct    radian_traits;
    template< typename T > struct arcminute_traits;
    
    template< typename T > using    degrees = unit< T,    degree_traits< T > >;
    template< typename T > using    radians = unit< T,    radian_traits< T > >;
    template< typename T > using arcminutes = unit< T, arcminute_traits< T > >;
}


namespace JadeMatrix::yavsg // Degrees /////////////////////////////////////////
{
    template< typename T > struct degree_traits
    {
        using value_type = T;
        
        static const std::string& unit_name()
        {
            static const std::string s{ "degree" };
            return s;
        }
        static const std::string& unit_symbol()
        {
            static const std::string s{ "°" };
            return s;
        }
        
        template< typename O >
        static constexpr
        T convert_from( const degrees< O >& from )
        {
            return static_cast< O >( from );
        }
        
        // Conversions from other units ////////////////////////////////////////
        
        template< typename O >
        static constexpr
        T convert_from( const radians< O >& from )
        {
            return (
                static_cast< O >( from )
                * ( constants::circle_degrees< T > / 2 )
                / constants::pi< T >
            );
        }
        
        template< typename O >
        static constexpr
        T convert_from( const arcminutes< O >& from )
        {
            return static_cast< O >( from ) / constants::hour_minutes< T >;
        }
    };
}


namespace JadeMatrix::yavsg // Radians /////////////////////////////////////////
{
    template< typename T > struct radian_traits
    {
        using value_type = T;
        
        static const std::string& unit_name()
        {
            static const std::string s{ "radian" };
            return s;
        }
        static const std::string& unit_symbol()
        {
            static const std::string s{ "r" };
            return s;
        }
        
        template< typename O >
        static constexpr
        T convert_from( const radians< O >& from )
        {
            return static_cast< O >( from );
        }
        
        // Conversions from other units ////////////////////////////////////////
        
        template< typename O >
        static constexpr
        T convert_from( const degrees< O >& from )
        {
            return (
                static_cast< O >( from )
                * constants::pi< T >
                / ( constants::circle_degrees< T > / 2 )
            );
        }
        
        template< typename O >
        static constexpr
        T convert_from( const arcminutes< O >& from )
        {
            return static_cast< O >( radians< O >( degrees< O >( from ) ) );
        }
    };
}


namespace JadeMatrix::yavsg // Arcminutes //////////////////////////////////////
{
    template< typename T > struct arcminute_traits
    {
        using value_type = T;
        
        static const std::string& unit_name()
        {
            static const std::string s{ "arcminute" };
            return s;
        }
        static const std::string& unit_symbol()
        {
            static const std::string s{ "moa" };
            return s;
        }
        
        template< typename O >
        static constexpr
        T convert_from( const arcminutes< O >& from )
        {
            return static_cast< O >( from );
        }
        
        // Conversions from other units ////////////////////////////////////////
        
        template< typename O >
        static constexpr
        T convert_from( const degrees< O >& from )
        {
            return static_cast< O >( from ) * constants::hour_minutes< T >;
        }
        
        template< typename O >
        static constexpr
        T convert_from( const radians< O >& from )
        {
            return static_cast< O >(
                degrees< O >( from ) * constants::hour_minutes< T >
            );
        }
    };
}


namespace JadeMatrix::yavsg // Prefixed versions of angular units //////////////
{
    ALL_PREFIXES_FOR_UNIT( degree   , degrees    )
    ALL_PREFIXES_FOR_UNIT( radian   , radians    )
    ALL_PREFIXES_FOR_UNIT( arcminute, arcminutes )
}
