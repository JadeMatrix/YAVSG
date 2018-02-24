#pragma once
#ifndef YAVSG_REAL_HPP
#define YAVSG_REAL_HPP


#include "math.hpp"

// #include <array>
#include <string>
#include <ostream>


namespace yavsg
{
    class real;
}


std::ostream& operator<<( std::ostream&, yavsg::real );


namespace yavsg
{
    class real
    {
        friend real sqrt< real >( const real& );
        // friend std::string to_string( const real& );
        friend std::ostream& ::operator<<( std::ostream&, yavsg::real );
        
    protected:
        const static int DECIMAL_PLACES = 4;
        
        __int128 _value;
        
    public:
        // real(                    );
        real( const real       & );
        // real( const __int128   & );
        real( int                );
        real( long long          );
        real( long double        );
        // real( const std::string& );
        
        bool  operator== ( const real& ) const;
        bool  operator!= ( const real& ) const;
        bool  operator<  ( const real& ) const;
        bool  operator>  ( const real& ) const;
        bool  operator<= ( const real& ) const;
        bool  operator>= ( const real& ) const;
        
        real& operator=  ( const real& )      ;
        
        real& operator+= ( const real& )      ;
        real& operator-= ( const real& )      ;
        real& operator*= ( const real& )      ;
        real& operator/= ( const real& )      ;
        real& operator%= ( const real& )      ;
        
        real  operator+  ( const real& ) const;
        real  operator-  ( const real& ) const;
        real  operator*  ( const real& ) const;
        real  operator/  ( const real& ) const;
        real  operator%  ( const real& ) const;
        
        real& operator<<=( int         )      ;
        real& operator>>=( int         )      ;
        real  operator<< ( int         )      ;
        real  operator>> ( int         )      ;
        
        // // Optimizations for small integers (typically 0, 1, -1, or 2)
        // bool  operator== ( int         ) const;
        // bool  operator!= ( int         ) const;
        // bool  operator<  ( int         ) const;
        // bool  operator>  ( int         ) const;
        // bool  operator<= ( int         ) const;
        // bool  operator>= ( int         ) const;
        // // real& operator=  ( int         )      ;
        // real& operator*= ( int         )      ;
        // real& operator/= ( int         )      ;
        // real& operator%= ( int         )      ;
        // real  operator*  ( int         ) const;
        // real  operator/  ( int         ) const;
        // real  operator%  ( int         ) const;
    };
    
    template<> inline real sqrt< real >( const real& v )
    {
        // return newtons_method< __int128 >( v._value );
        return newtons_method< real >( v );
    }
    
    // std::string to_string( const real& o, int base = 10 );
}


#endif
