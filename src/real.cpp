#include "real.hpp"

// // http://en.cppreference.com/w/cpp/header/cmath
// #include <cmath>
// #include <cstdint>


namespace yavsg
{
    // Constructors ////////////////////////////////////////////////////////////
    
    real::real( const real& o ) : _value( o._value )
    {}
    
    // real::real( const __int128& v ) : _value( v )
    // {}
    
    real::real( int v ) : _value( v )
    {
        for( int i = 0; i < DECIMAL_PLACES; ++i )
            _value *= 10;
    }
    
    real::real( long long v ) : _value( v )
    {
        for( int i = 0; i < DECIMAL_PLACES; ++i )
            _value *= 10;
    }
    
    real::real( long double v ) : _value( v )
    {
        for( int i = 0; i < DECIMAL_PLACES; ++i )
            _value *= 10;
    }
    
    // real::real( const std::string& )
    // {
    //     // IMPLEMENT:
    // }
    
    // General operators ///////////////////////////////////////////////////////
    
    bool real::operator==( const real& o ) const
    {
        return _value == o._value;
    }
    
    bool real::operator!=( const real& o ) const
    {
        return !( *this == o );
    }
    
    bool real::operator<( const real& o ) const
    {
        return _value < o._value;
    }
    
    bool real::operator>( const real& o ) const
    {
        return _value > o._value;
    }
    
    bool real::operator<=( const real& o ) const
    {
        return _value <= o._value;
    }
    
    bool real::operator>=( const real& o ) const
    {
        return _value >= o._value;
    }
    
    real& real::operator= ( const real& o )
    {
        _value = o._value;
        return *this;
    }
    
    real& real::operator+=( const real& o )
    {
        _value += o._value;
        return *this;
    }
    
    real& real::operator-=( const real& o )
    {
        _value -= o._value;
        return *this;
    }
    
    real& real::operator*=( const real& o )
    {
        _value *= o._value;
        return *this;
    }
    
    real& real::operator/=( const real& o )
    {
        // _value /= o._value;
        _value = ( _value + ( o._value / 2 ) ) / o._value;
        return *this;
    }
    
    real& real::operator%=( const real& o )
    {
        _value %= o._value;
        return *this;
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
    
    real& real::operator<<=( int p )
    {
        _value <<= p;
        return *this;
    }
    
    real& real::operator>>=( int p )
    {
        _value >>= p;
        return *this;
    }
    
    real real::operator<<( int p )
    {
        real result( *this );
        result <<= p;
        return result;
    }
    
    real real::operator>>( int p )
    {
        real result( *this );
        result >>= p;
        return result;
    }
    
    // Small-integer optimizations /////////////////////////////////////////////
    
    // bool real::operator==( int v ) const
    // {
    //     return (
    //            _repr[ 1 ] == 0
    //         && _repr[ 2 ] == 0
    //         && _repr[ 0 ] == v
    //     );
    // }
    
    // bool real::operator!=( int v ) const
    // {
    //     return !( *this == v );
    // }
    
    // bool real::operator<( int v ) const
    // {
        
    // }
    
    // bool real::operator>( int v ) const
    // {
        
    // }
    
    // bool real::operator<=( int v ) const
    // {
        
    // }
    
    // bool real::operator>=( int v ) const
    // {
        
    // }
    
    // // real& real::operator= ( int v )
    // // {
        
    // // }
    
    // real& real::operator*=( int v )
    // {
    //     switch( v )
    //     {
    //     case 0:
    //         _repr = { 0, 0, 0 };
    //         break;
    //     case 1:
    //         // Do nothing
    //         break;
    //     case -1:
    //         flip_sign();
    //         break;
    //     case 2:
    //         *this << v;
    //         break;
    //     case -2:
    //         flip_sign();
    //         *this << v;
    //         break;
    //     default:
    //         *this *= real( v );
    //         break;
    //     }
    //     return *this;
    // }
    
    // real& real::operator/=( int v )
    // {
    //     switch( v )
    //     {
    //     case 0:
    //         _repr = { 0, 0, 0 };
    //         break;
    //     case 1:
    //         // Do nothing
    //         break;
    //     case -1:
    //         flip_sign();
    //         break;
    //     case 2:
    //         *this >> v;
    //         break;
    //     case -2:
    //         flip_sign();
    //         *this >> v;
    //         break;
    //     default:
    //         *this /= real( v );
    //         break;
    //     }
    //     return *this;
    // }
    
    // real real::operator*( int v ) const
    // {
    //     real result( *this );
    //     result *= v;
    //     return result;
    // }
    
    // real real::operator/( int v ) const
    // {
    //     real result( *this );
    //     result /= v;
    //     return result;
    // }
    
    // real& real::operator%=( int v )
    // {
    //     // return 
    // }
    
    // real real::operator%( int v ) const
    // {
    //     real result( *this );
    //     result %= v;
    //     return result;
    // }
    
    // Serialization ///////////////////////////////////////////////////////////
    
    // std::string to_string( const real& o, int base )
    // {
    //     // for( unsigned int i = 0; i < real::INTEGERS; ++i )
    //     //     if( o._repr[ 0 ] == 0 )
    //     //         return false;
        
    //     if( o == 0 )
    //         return "0";
    //     else if(
            
    //     )
    //         NULL;
    //     else
    //         NULL;
    // }
}


std::ostream& operator<<( std::ostream& out, yavsg::real v )
{
    // Uses v._value instead of v because yavsg::real uses rounding division
    
    // __int128 a = v._value >> yavsg::real::DECIMAL_BITS;
    // __int128 b = v._value & ( ~( ( __int128 )0 ) >> ( sizeof( __int128 ) * 8 - yavsg::real::DECIMAL_BITS ) );
    
    if( v._value == 0 )
        out << "0";
    else if( v._value == ( __int128 )0x01 << ( sizeof( __int128 ) * 8 - 1 ) )
        out << "-170141183460469231731687303715884105728";
    // else if( v == ~( ( __int128 )0x01 << 127 ) )
    //     out << "170141183460469231731687303715884105727";
    else
    {
        if( v._value < 0 )
        {
            out << "-";
            v._value *= -1;
        }
        
        std::string val_str;
        for( int i = 0; v._value > 0; ++i )
        {
            if( i == yavsg::real::DECIMAL_PLACES )
                val_str = "." + val_str;
            val_str = std::to_string( ( int )( v._value % 10 ) ) + val_str;
            v._value /= 10;
        }
        
        out << val_str;
    }
    return out;
}
