#pragma once
#ifndef YAVSG_MATH_VECTOR_HPP
#define YAVSG_MATH_VECTOR_HPP


#include "matrix.hpp"
#include "scalar_operations.hpp"

#include <array>
#include <initializer_list>
#include <ostream>


namespace yavsg
{
    template< typename T, unsigned int D > class vector
    {
    public:
        using storage_type = std::array< T, D >;
        
    protected:
        constexpr vector() {}
        
        storage_type values;
        
    public:
        template< typename O > constexpr vector(
            const std::array< O, D >& a
        )
        {
            for( unsigned int i = 0; i < D; ++i )
                values[ i ] = static_cast< T >( a[ i ] );
        }
        
        template< typename O > constexpr vector(
            const vector< O, D >& o
        )
        {
            for( unsigned int i = 0; i < D; ++i )
                values[ i ] = static_cast< T >( o[ i ] );
        }
        
        template< typename... O > constexpr vector(
            O... args
        ) : values( { args... } )
        {}
        
        constexpr vector( std::initializer_list< T > il )
        {
            // Implements same semantics as partial array initialization via
            // brace-enclosed lists (uninitialized members become 0)
            auto iter = il.begin();
            for( unsigned int i = 0; i < D; ++i )
                if( iter == il.end() )
                    values[ i ] = 0;
                else
                {
                    values[ i ] = *iter;
                    ++iter;
                }
        }
        
        static constexpr vector< T, D > make_filled( const T& fill )
        {
            vector< T, D > v;
            v.values.fill( fill );
            return v;
        }
        
        constexpr T& operator[]( unsigned int i )
        {
            return values[ i ];
        }
        constexpr const T& operator[]( unsigned int i ) const
        {
            return values[ i ];
        }
        
        // Can be constexpr in C++17
        typename storage_type::iterator begin()
        {
            return values.begin();
        }
        typename storage_type::iterator end()
        {
            return values.end();
        }
        const typename storage_type::const_iterator begin() const
        {
            return values.begin();
        }
        const typename storage_type::const_iterator end() const
        {
            return values.end();
        }
        
        // WARNING: As unit division is currently incomplete, these functions
        // make vector<unit<...>,...> fail pretty spectacularly
        
        template< typename O >
        constexpr auto dot( const vector< O, D >& o ) const
            -> decltype( values[ 0 ] * o[ 0 ] )
        {
            decltype( values[ 0 ] * o[ 0 ] ) sum = 0;
            for( unsigned int i = 0; i < D; ++i )
                sum += values[ i ] * o[ i ];
            return sum;
        }
        
        constexpr auto magnitude() const
            -> decltype( sqrt( dot( *this ) ) )
        {
            return sqrt( dot( *this ) );
        }
        
        constexpr auto unit() const
            -> decltype( *this / magnitude() )
        {
            return *this / magnitude();
        }
        
        const constexpr T* data() const
        {
            return values.data();
        }
        
        // Partially because I'm lazy, but it makes creating uninitialized
        // vectors explicit rather than implicit with a default constructor
        static constexpr vector< T, D > make_uninitialized()
        {
            return vector< T, D >();
        }
    };
    
    // TODO: Make generic to n dimensions
    template< typename L, typename R >
    constexpr
    auto cross_product(
        const vector< L, 3 >& u,
        const vector< R, 3 >& v
    ) -> vector< decltype( u[ 0 ] * v[ 0 ] ), 3 >
    {
        return vector< decltype( u[ 0 ] * v[ 0 ] ), 3 >{
            u[ 1 ] * v[ 2 ] - u[ 3 ] * v[ 1 ],
            u[ 2 ] * v[ 0 ] - u[ 0 ] * v[ 2 ],
            u[ 0 ] * v[ 1 ] - u[ 1 ] * v[ 0 ]
        };
    }
}


namespace yavsg // Binary scalar operators /////////////////////////////////////
{
    #define DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND( OPERAND ) \
    template< typename L, unsigned int LD, typename R > \
    constexpr \
    auto operator OPERAND ( const vector< L, LD >& lhs, const R& rhs ) \
        -> vector< decltype( lhs[ 0 ] OPERAND rhs ), LD > \
    { \
        auto v = vector< \
            decltype( lhs[ 0 ] OPERAND rhs ), \
            LD \
        >::make_uninitialized(); \
        for( unsigned int i = 0; i < LD; ++i ) \
            v[ i ] = lhs[ i ] OPERAND rhs; \
        return v; \
    } \
     \
    template< typename L, typename R, unsigned int RD > \
    constexpr \
    auto operator OPERAND ( const L& lhs, const vector< R, RD >& rhs ) \
        -> vector< decltype( lhs OPERAND rhs[ 0 ] ), RD > \
    { \
        auto v = vector< \
            decltype( lhs OPERAND rhs[ 0 ] ), \
            RD \
        >::make_uninitialized(); \
        for( unsigned int i = 0; i < RD; ++i ) \
            v[ i ] = lhs OPERAND rhs[ i ]; \
        return v; \
    }
    
    DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND( + )
    DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND( - )
    DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND( * )
    DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND( / )
    DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND( % )
    
    #undef DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND
}


namespace yavsg // Assignment scalar operators /////////////////////////////////
{
    #define DEFINE_OPERATORS_FOR_ASSIGNMENT_SCALAR_OPERAND( OPERAND ) \
    template< typename L, unsigned int LD, typename R > \
    constexpr \
    vector< L, LD >& operator OPERAND##= ( \
        vector< L, LD >& lhs, \
        const R& rhs \
    ) \
    { \
        for( auto& e : lhs ) \
            e OPERAND##= rhs; \
        return lhs; \
    }
    
    DEFINE_OPERATORS_FOR_ASSIGNMENT_SCALAR_OPERAND( + )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_SCALAR_OPERAND( - )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_SCALAR_OPERAND( * )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_SCALAR_OPERAND( / )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_SCALAR_OPERAND( % )
    
    #undef DEFINE_OPERATORS_FOR_ASSIGNMENT_SCALAR_OPERAND
}


namespace yavsg // Binary vector operators /////////////////////////////////////
{
    #define DEFINE_OPERATORS_FOR_BINARY_VECTOR_OPERAND( OPERAND ) \
    template< typename L, typename R, unsigned int D > \
    constexpr \
    auto operator OPERAND ( \
        const vector< L, D >& lhs, \
        const vector< R, D >& rhs \
    ) -> vector< decltype( lhs[ 0 ] OPERAND rhs[ 0 ] ), D > \
    { \
        auto v = vector< \
            decltype( lhs[ 0 ] OPERAND rhs[ 0 ] ), \
            D \
        >::make_uninitialized(); \
        for( unsigned int i = 0; i < D; ++i ) \
            v[ i ] = lhs[ i ] OPERAND rhs[ i ]; \
        return v; \
    }
    
    DEFINE_OPERATORS_FOR_BINARY_VECTOR_OPERAND( + )
    DEFINE_OPERATORS_FOR_BINARY_VECTOR_OPERAND( - )
    
    #undef DEFINE_OPERATORS_FOR_BINARY_VECTOR_OPERAND
}


namespace yavsg // Assignment vector operators /////////////////////////////////
{
    #define DEFINE_OPERATORS_FOR_ASSIGNMENT_VECTOR_OPERAND( OPERAND ) \
    template< typename L, typename R, unsigned int D > \
    constexpr \
    vector< L, D >& operator OPERAND##= ( \
              vector< L, D >& lhs, \
        const vector< R, D >& rhs \
    ) \
    { \
        for( unsigned int i = 0; i < D; ++i ) \
            lhs[ i ] OPERAND##= rhs[ i ]; \
        return lhs; \
    }
    
    DEFINE_OPERATORS_FOR_ASSIGNMENT_VECTOR_OPERAND( + )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_VECTOR_OPERAND( - )
    
    #undef DEFINE_OPERATORS_FOR_ASSIGNMENT_VECTOR_OPERAND
}


namespace yavsg // Equality operators //////////////////////////////////////////
{
    template< typename L, typename R, unsigned int D >
    constexpr
    auto operator==(
        const vector< L, D >& lhs,
        const vector< R, D >& rhs
    ) -> decltype( lhs[ 0 ] == rhs[ 0 ] )
    {
        for( unsigned int i = 0; i < D; ++i )
            if( !( lhs[ i ] == rhs[ i ] ) )
                return lhs[ i ] == rhs[ i ];
        return lhs[ 0 ] == rhs[ 0 ];
    }
    
    template< typename L, typename R, unsigned int D >
    constexpr
    auto operator!=(
        const vector< L, D >& lhs,
        const vector< R, D >& rhs
    ) -> decltype( lhs[ 0 ] != rhs[ 0 ] )
    {
        for( unsigned int i = 0; i < D; ++i )
            if( lhs[ i ] != rhs[ i ] )
                return lhs[ i ] != rhs[ i ];
        return lhs[ 0 ] != rhs[ 0 ];
    }
}


#if 0
namespace yavsg // (Magnitude) comparison operators ////////////////////////////
{
    #define DEFINE_OPERATORS_FOR_COMPARISON_OPERAND( OPERAND ) \
    template< typename L, typename R, unsigned int D > \
    constexpr \
    auto operator OPERAND ( \
        const vector< L, D >& lhs, \
        const vector< R, D >& rhs \
    ) -> decltype( lhs.magnitude() OPERAND rhs.magnitude() ) \
    { \
        return lhs.magnitude() OPERAND rhs.magnitude(); \
    }
    
    DEFINE_OPERATORS_FOR_COMPARISON_OPERAND( <  )
    DEFINE_OPERATORS_FOR_COMPARISON_OPERAND( >  )
    DEFINE_OPERATORS_FOR_COMPARISON_OPERAND( <= )
    DEFINE_OPERATORS_FOR_COMPARISON_OPERAND( >= )
    
    #undef DEFINE_OPERATORS_FOR_COMPARISON_OPERAND
}
#endif


namespace yavsg // Matrix-vector multiplication ////////////////////////////////
{
    // If unmatching dimensions are required, use pure matrix multiplication
    template<
        typename L,
        typename R,
        unsigned int D
    >
    constexpr
    auto operator*(
        const matrix< L, D, D >& lhs,
        const vector< R, D    >& rhs
    ) -> vector< decltype( lhs[ 0 ][ 0 ] * rhs[ 0 ] ), D >
    {
        auto v = vector<
            decltype( lhs[ 0 ][ 0 ] * rhs[ 0 ] ),
            D
        >::make_filled( 0 );
        for( unsigned int i = 0; i < D; ++i )
            for( unsigned int j = 0; j < D; ++j )
                v[ i ] += lhs[ i ][ j ] * rhs[ j ];
        return v;
    }
}


namespace yavsg // Stream formatting operator //////////////////////////////////
{
    template< typename T, unsigned int D >
    std::ostream& operator<<( std::ostream& out, const vector< T, D >& v )
    {
        out << "<";
        for( unsigned int i = 0; i < D; ++i )
        {
            out << v[ i ];
            if( i + 1 < D )
                out << ",";
        }
        out << ">";
        return out;
    }
}


#endif
