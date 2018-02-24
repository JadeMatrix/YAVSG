#pragma once
#ifndef YAVSG_VECTOR_HPP
#define YAVSG_VECTOR_HPP


#include "math.hpp"

#include <array>


namespace yavsg
{
    template< typename T, unsigned int D > class vector
    {
    protected:
        vector() {}
        
        // T values[ D ];
        std::array< T, D > values;
        
    public:
        vector( T* );
        // template< typename Args... > vector( Args... args ) : values( { args } ) {};
        vector( const std::array< T, D >& );
        vector( const vector& );
        
        T magnitude() const;
        
        T dot( const vector< T, D >& ) const;
        
        // static vector< T, D > unit();
        vector< T, D > unit();
        
                                             T& operator[]( unsigned int          )      ;
                                       const T& operator[]( unsigned int          ) const;
                                          bool  operator==( const vector< T, D >& ) const;
                                          bool  operator!=( const vector< T, D >& ) const;
        
                                vector< T, D >& operator= ( const vector< T, D >& )      ;
        
                                vector< T, D >& operator+=( const vector< T, D >& )      ;
                                vector< T, D >& operator-=( const vector< T, D >& )      ;
        template< typename T2 > vector< T, D >& operator*=( const T2&             )      ;
        template< typename T2 > vector< T, D >& operator/=( const T2&             )      ;
        
                                vector< T, D >  operator+ ( const vector< T, D >& ) const;
                                vector< T, D >  operator- ( const vector< T, D >& ) const;
                                vector< T, D >  operator/ ( const vector< T, D >& ) const;
        template< typename T2 > vector< T, D >  operator* ( const T2&             ) const;
        template< typename T2 > vector< T, D >  operator/ ( const T2&             ) const;
    };
    
    template< typename T > vector< T, 3 > cross_product(
        const vector< T, 3 >& u,
        const vector< T, 3 >& v
    );
    
    
    // Implementations /////////////////////////////////////////////////////////
    
    
    // Vector ------------------------------------------------------------------
    
    template< typename T, unsigned int D > vector< T, D >::vector( T* a )
    {
        for( unsigned int i = 0; i < D; ++i )
            values[ i ] = a[ i ];
    }
    
    template< typename T, unsigned int D > vector< T, D >::vector(
        const std::array< T, D >& v
    ) : values( v )
    {}
    
    template< typename T, unsigned int D > vector< T, D >::vector(
        const vector& o
    ) : values( o.values )
    {}
    
    template< typename T, unsigned int D > T vector< T, D >::magnitude() const
    {
        return yavsg::sqrt( dot( *this ) );
    }
    
    template< typename T, unsigned int D > T vector< T, D >::dot(
        const vector< T, D >& o
    ) const
    {
        T sum = 0;
        for( unsigned int i = 0; i < D; ++i )
            sum += ( *this )[ i ] * o[ i ];
        return sum;
    }
    
    template< typename T, unsigned int D > vector< T, D > vector< T, D >::unit()
    {
        // vector unit_vector;
        // for( auto& v : unit_vector.values )
        //     v = 1;
        // unit_vector /= unit_vector.magnitude();
        // return unit_vector;
        return *this / this -> magnitude();
    }
    
    template< typename T, unsigned int D > T& vector< T, D >::operator[](
        unsigned int i
    )
    {
        return values[ i ];
    }
    
    template< typename T, unsigned int D > const T& vector< T, D >::operator[](
        unsigned int i
    ) const
    {
        return values[ i ];
    }
    
    template< typename T, unsigned int D > bool vector< T, D >::operator==(
        const vector< T, D >& o
    ) const
    {
        return values == o.values;
    }
    
    template< typename T, unsigned int D > bool vector< T, D >::operator!=(
        const vector< T, D >& o
    ) const
    {
        return values != o.values;
    }
    
    template< typename T, unsigned int D > vector< T, D >& vector< T, D >::operator=(
        const vector< T, D >& o
    )
    {
        values = o.values;
        return *this;
    }
    
    template< typename T, unsigned int D > vector< T, D >& vector< T, D >::operator+=(
        const vector< T, D >& o
    )
    {
        for( unsigned int i = 0; i < D; ++i )
            values[ i ] += o.values[ i ];
        return *this;
    }
    
    template< typename T, unsigned int D > vector< T, D >& vector< T, D >::operator-=(
        const vector< T, D >& o
    )
    {
        for( unsigned int i = 0; i < D; ++i )
            values[ i ] -= o.values[ i ];
        return *this;
    }
    
    template< typename T, unsigned int D >
    template< typename T2 >
    vector< T, D >& vector< T, D >::operator*=( const T2& s )
    {
        for( auto& v : values )
            v *= s;
        return *this;
    }
    
    template< typename T, unsigned int D >
    template< typename T2 >
    vector< T, D >& vector< T, D >::operator/=( const T2& s )
    {
        for( auto& v : values )
            v /= s;
        return *this;
    }
    
    template< typename T, unsigned int D > vector< T, D > vector< T, D >::operator+(
        const vector< T, D >& o
    ) const
    {
        vector< T, D > copy( *this );
        copy += o;
        return copy;
    }
    
    template< typename T, unsigned int D > vector< T, D > vector< T, D >::operator-(
        const vector< T, D >& o
    ) const
    {
        vector< T, D > copy( *this );
        copy -= o;
        return copy;
    }
    
    template< typename T, unsigned int D >
    template< typename T2 >
    vector< T, D > vector< T, D >::operator*( const T2& s ) const
    {
        vector< T, D > copy( *this );
        copy *= s;
        return copy;
    }
    
    template< typename T, unsigned int D >
    template< typename T2 >
    vector< T, D > vector< T, D >::operator/( const T2& s ) const
    {
        vector< T, D > copy( *this );
        copy /= s;
        return copy;
    }
    
    // Cross product -----------------------------------------------------------
    
    template< typename T > vector< T, 3 > cross_product(
        const vector< T, 3 >& u,
        const vector< T, 3 >& v
    )
    {
        return vector< T, 3 >( std::array< T, 3 >( {
            u[ 1 ] * v[ 2 ] - u[ 3 ] * v[ 1 ],
            u[ 2 ] * v[ 0 ] - u[ 0 ] * v[ 2 ],
            u[ 0 ] * v[ 1 ] - u[ 1 ] * v[ 0 ]
        } ) );
    }
}


#endif
