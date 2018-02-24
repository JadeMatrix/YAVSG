#pragma once
#ifndef YAVSG_UNITS_STRING_CONVERSIONS_HPP
#define YAVSG_UNITS_STRING_CONVERSIONS_HPP


#include "unit.hpp"

#include <string>


namespace yavsg
{
    template< typename T >
    std::string to_string( const T& v )
    {
        using std::to_string;
        return std::to_string( v );
    }
    
    template< typename T, class Traits >
    std::string to_string( const unit< T, Traits >& v )
    {
        return to_string( ( T )v ) + Traits::unit_symbol();
    }
}


#endif
