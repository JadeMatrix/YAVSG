#pragma once
#ifndef GL_MATH_SCALARS_HPP
#define GL_MATH_SCALARS_HPP


#include "type_base.hpp"


namespace gl
{
    namespace constants
    {
        static const double pi             = 3.141592653589793f;
        static const int    circle_degrees = 360;
    }
    
    template< typename T > class degrees;
    template< typename T > class radians;
    template< typename T > class arcminutes;
    
    template< typename T > std::string to_string( const degrees< T >& v );
    template< typename T > std::string to_string( const radians< T >& v );
    template< typename T > std::string to_string( const arcminutes< T >& v );
    
    template< typename T > class degrees
    {
        SCALAR_TYPE_INSIDE_CLASS_DEF( degrees, T );
        
        template< typename > friend class radians;
        template< typename > friend class arcminutes;
        friend std::string to_string< T >( const degrees< T >& );
        
    public:
        template< typename O > degrees( const radians< O >& o )
            : value(
                o.value
                * ( ( T )constants::circle_degrees / ( T )2 )
                / constants::pi
            )
        {}
        
        template< typename O > degrees( const arcminutes< O >& o )
            : value( o.value / ( T )60 )
        {}
        
        // TODO:
        template< typename O > constexpr operator radians< O >() const; // { return ( O )( value ); }
    };
    
    template< typename T > class radians
    {
        SCALAR_TYPE_INSIDE_CLASS_DEF( radians, T );
        
        template< typename > friend class degrees;
        template< typename > friend class arcminutes;
        friend std::string to_string< T >( const radians< T >& );
        
    public:
        template< typename O > radians( const degrees< O >& o )
            : value(
                o.value
                * constants::pi
                / ( ( T )constants::circle_degrees / ( T )2 )
            )
        {}
        
        template< typename O > radians( const arcminutes< O >& o )
            : value( radians< T >( degrees< T >( o ) ) / ( T )60 )
        {}
    };
    
    template< typename T > class arcminutes
    {
        SCALAR_TYPE_INSIDE_CLASS_DEF( arcminutes, T );
        
        template< typename > friend class radians;
        template< typename > friend class degrees;
        friend std::string to_string< T >( const arcminutes< T >& );
        
    public:
        template< typename O > arcminutes( const degrees< O >& o )
            : value( o.value * ( T )60 )
        {}
        
        template< typename O > arcminutes( const radians< O >& o )
            : value( degrees< T >( o ).value * ( T )60 )
        {}
    };
    
    template< typename T > std::string to_string( const degrees< T >& v )
    {
        return std::to_string( v.value ) + "°";
    }
    
    template< typename T > std::string to_string( const radians< T >& v )
    {
        return std::to_string( v.value ) + "r";
    }
    
    template< typename T > std::string to_string( const arcminutes< T >& v )
    {
        return std::to_string( v.value ) + "moa";
    }
    
    SCALAR_TYPE_OUTSIDE_CLASS_DEF( degrees );
    SCALAR_TYPE_OUTSIDE_CLASS_DEF( radians );
    SCALAR_TYPE_OUTSIDE_CLASS_DEF( arcminutes );
}


#endif
