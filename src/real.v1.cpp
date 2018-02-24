#include "real.hpp"

// // http://en.cppreference.com/w/cpp/header/cmath
// #include <cmath>
// #include <cstdint>


namespace yavsg
{
    /*
    0xFF       * 0xFF       = 0xFE01
    0xFFFF     * 0xFFFF     = 0xFFFE0001
    0xFFFFFF   * 0xFFFFFF   = 0xFFFFFE000001
    0xFFFFFFFF * 0xFFFFFFFF = 0xFFFFFFFE00000001
    */
    
    // Utility /////////////////////////////////////////////////////////////////
    
    void real::flip_sign()
    {
        
    }
    
    // Constructors ////////////////////////////////////////////////////////////
    
    real::real() : _repr{ 0, 0, 0 }
    {}
    
    real::real( const real& o ) : _repr( o._repr )
    {}
    
    real::real( int v )
    {
        
    }
    
    real::real( long long v )
    {
        _repr[ 0 ] = *( unsigned long long* )( &v );
        if( v < 0 )
            _repr[ 1 ] = ~0ll;
        else
            _repr[ 1 ] = 0;
        _repr[ 2 ] = _repr[ 1 ];
    }
    
    real::real( long double v )
    {
        
    }
    
    real::real( const std::string& )
    {
        
    }
    
    // General operators ///////////////////////////////////////////////////////
    
    bool real::operator==( const real& o ) const
    {
        for( unsigned int i = 0; i < INTEGERS; ++i )
            if( _repr[ 0 ] != o._repr[ 0 ] )
                return false;
        return true;
    }
    
    bool real::operator!=( const real& o ) const
    {
        return !( *this == o );
    }
    
    bool real::operator<( const real& o ) const
    {
        
    }
    
    bool real::operator>( const real& o ) const
    {
        
    }
    
    bool real::operator<=( const real& o ) const
    {
        
    }
    
    bool real::operator>=( const real& o ) const
    {
        
    }
    
    real& real::operator= ( const real& o )
    {
        _repr = o._repr;
        return *this;
    }
    
    real& real::operator+=( const real& o )
    {
        
    }
    
    real& real::operator-=( const real& o )
    {
        
    }
    
    real& real::operator*=( const real& o )
    {
        
    }
    
    real& real::operator/=( const real& o )
    {
        
    }
    
    real& real::operator%=( const real& o )
    {
        
    }
    
    real real::operator+( const real& o ) const
    {
        real result( *this );
        result += o;
        return result;
    }
    
    real real::operator-( const real& o ) const
    {
        real result( *this );
        result -= o;
        return result;
    }
    
    real real::operator*( const real& o ) const
    {
        real result( *this );
        result *= o;
        return result;
    }
    
    real real::operator/( const real& o ) const
    {
        real result( *this );
        result /= o;
        return result;
    }
    
    real real::operator%( const real& o ) const
    {
        real result( *this );
        result %= o;
        return result;
    }
    
    real real::operator<<=( int p )
    {
        
    }
    
    real real::operator>>=( int p )
    {
        
    }
    
    real& real::operator<<( int p )
    {
        real result( *this );
        result << p;
        return result;
    }
    
    real& real::operator>>( int p )
    {
        real result( *this );
        result >> p;
        return result;
    }
    
    bool real::operator==( int v ) const
    {
        return (
               _repr[ 1 ] == 0
            && _repr[ 2 ] == 0
            && _repr[ 0 ] == v
        );
    }
    
    // Small-integer optimizations /////////////////////////////////////////////
    
    bool real::operator!=( int v ) const
    {
        return !( *this == v );
    }
    
    bool real::operator<( int v ) const
    {
        
    }
    
    bool real::operator>( int v ) const
    {
        
    }
    
    bool real::operator<=( int v ) const
    {
        
    }
    
    bool real::operator>=( int v ) const
    {
        
    }
    
    // real& real::operator= ( int v )
    // {
        
    // }
    
    real& real::operator*=( int v )
    {
        switch( v )
        {
        case 0:
            _repr = { 0, 0, 0 };
            break;
        case 1:
            // Do nothing
            break;
        case -1:
            flip_sign();
            break;
        case 2:
            *this << v;
            break;
        case -2:
            flip_sign();
            *this << v;
            break;
        default:
            *this *= real( v );
            break;
        }
        return *this;
    }
    
    real& real::operator/=( int v )
    {
        switch( v )
        {
        case 0:
            _repr = { 0, 0, 0 };
            break;
        case 1:
            // Do nothing
            break;
        case -1:
            flip_sign();
            break;
        case 2:
            *this >> v;
            break;
        case -2:
            flip_sign();
            *this >> v;
            break;
        default:
            *this /= real( v );
            break;
        }
        return *this;
    }
    
    real real::operator*( int v ) const
    {
        real result( *this );
        result *= v;
        return result;
    }
    
    real real::operator/( int v ) const
    {
        real result( *this );
        result /= v;
        return result;
    }
    
    real& real::operator%=( int v )
    {
        // return 
    }
    
    real real::operator%( int v ) const
    {
        real result( *this );
        result %= v;
        return result;
    }
    
    // Serialization ///////////////////////////////////////////////////////////
    
    std::string to_string( const real& o, int base )
    {
        // for( unsigned int i = 0; i < real::INTEGERS; ++i )
        //     if( o._repr[ 0 ] == 0 )
        //         return false;
        
        if( o == 0 )
            return "0";
        else if(
            
        )
            NULL;
        else
            NULL;
    }
}
