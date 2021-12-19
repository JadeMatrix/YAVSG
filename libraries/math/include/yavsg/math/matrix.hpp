#pragma once


#include <array>
#include <initializer_list>
#include <ostream>
#include <sstream>


namespace JadeMatrix::yavsg
{
    template< typename T, unsigned int Rows, unsigned int Cols > class matrix
    {
    public:
        using    this_type = matrix< T, Rows, Cols >;
        using   value_type = T;
        using storage_type = std::array< std::array< T, Rows >, Cols >;
        template< typename O > using same_dimensions_type = matrix<
            O,
            Rows,
            Cols
        >;
        template< typename O > using transposed_type = matrix< O, Cols, Rows >;
        
    protected:
        storage_type values;
        
        constexpr matrix() {}
        
    public:
        template< typename O > constexpr matrix(
            const std::array< std::array< O, Cols >, Rows >& a
        )
        {
            for( unsigned int i = 0; i < Rows; ++i )
                for( unsigned int j = 0; j < Cols; ++j )
                    values[ j ][ i ] = static_cast< T >( a[ j ][ i ] );
        }
        
        template< typename O > constexpr matrix(
            const same_dimensions_type< O >& o
        )
        {
            for( unsigned int i = 0; i < Rows; ++i )
                for( unsigned int j = 0; j < Cols; ++j )
                    values[ j ][ i ] = static_cast< T >( o[ j ][ i ] );
        }
        
        constexpr matrix(
            const std::initializer_list< std::initializer_list< T > > il
        )
        {
            // Implements same semantics as partial array initialization via
            // brace-enclosed lists (uninitialized members become 0)
            auto col_iter = il.begin();
            for( unsigned int j = 0; j < Cols; ++j )
            {
                if( col_iter == il.end() )
                    for( unsigned int i = 0; i < Rows; ++i )
                        values[ j ][ i ] = 0;
                else
                {
                    auto row_iter = col_iter -> begin();
                    for( unsigned int i = 0; i < Rows; ++i )
                        if( row_iter == col_iter -> end() )
                            values[ j ][ i ] = 0;
                        else
                        {
                            values[ j ][ i ] = *row_iter;
                            ++row_iter;
                        }
                    ++col_iter;
                }
            }
        }
        
        static constexpr this_type make_filled( const T& fill )
        {
            this_type m;
            for( auto& row : m )
                row.fill( fill );
            return m;
        }
        
        // Returns the ith _column_
        constexpr typename storage_type::value_type& operator[](
            unsigned int i
        )
        {
            return values[ i ];
        }
        constexpr const typename storage_type::value_type& operator[](
            unsigned int i
        ) const
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
        
        // Partially because I'm lazy, but it makes creating uninitialized
        // vectors explicit rather than implicit with a default constructor
        static constexpr matrix< T, Rows, Cols > make_uninitialized()
        {
            return matrix< T, Rows, Cols >();
        }
        
        constexpr transposed_type< T > transposed()
        {
            auto m = transposed_type< T >::make_uninitialized();
            for( unsigned int i = 0; i < Rows; ++i )
                for( unsigned int j = 0; j < Cols; ++j )
                    m[ j ][ i ] = values[ j ][ i ];
            return m;
        }
        
        template< // Matrix multiplication /////////////////////////////////////
            typename R,
            unsigned int RCols
        >
        constexpr
        auto operator*(
            const matrix< R, Cols, RCols >& rhs
        ) -> matrix<
            decltype( values[ 0 ][ 0 ] * rhs[ 0 ][ 0 ] ),
            Rows,
            RCols
        >
        {
            auto m = matrix<
                decltype( values[ 0 ][ 0 ] * rhs[ 0 ][ 0 ] ),
                Rows,
                RCols
            >::make_filled( 0 );
            // Iterate over j, then k, then i for cache friendliness:
            //   * j first as it appears as the first index twice
            //   * k next as it is the other first index
            //   * i last as it appears as the second index twice and only
            for( unsigned int j = 0; j < RCols; ++j )
                for( unsigned int k = 0; k < Cols; ++k )
                    for( unsigned int i = 0; i < Rows; ++i )
                        m[ j ][ i ] += values[ k ][ i ] * rhs[ j ][ k ];
            return m;
        }
        
        const constexpr T* data() const
        {
            return values[ 0 ].data();
        }
    };
    
    template<
        typename T,
        unsigned int D
    > using square_matrix = matrix< T, D, D >;
    
    template< typename T, unsigned int D >
    constexpr
    square_matrix< T, D > identity_matrix()
    {
        auto m = square_matrix< T, D >::make_filled( 0 );
        for( unsigned int i = 0; i < D; ++i )
            m[ i ][ i ] = 1;
        return m;
    }
}


namespace JadeMatrix::yavsg // Binary scalar operators /////////////////////////
{
    #define DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND( OPERAND ) \
    template< \
        typename L, \
        unsigned int Rows, \
        unsigned int Cols, \
        typename R \
    > \
    constexpr \
    auto operator OPERAND ( \
        const matrix< L, Rows, Cols >& lhs, \
        const R& rhs \
    ) -> matrix< decltype( lhs[ 0 ][ 0 ] OPERAND rhs ), Rows, Cols > \
    { \
        auto m = matrix< \
            decltype( lhs[ 0 ][ 0 ] OPERAND rhs ), \
            Rows, \
            Cols \
        >::make_uninitialized(); \
        for( unsigned int i = 0; i < Rows; ++i ) \
            for( unsigned int j = 0; j < Cols; ++j ) \
                m[ j ][ i ] = lhs[ j ][ i ] OPERAND rhs; \
        return m; \
    } \
     \
    template< \
        typename L, \
        unsigned int Rows, \
        unsigned int Cols, \
        typename R \
    > \
    constexpr \
    auto operator OPERAND ( \
        const L& lhs, \
        const matrix< R, Rows, Cols >& rhs \
    ) -> matrix< decltype( lhs OPERAND rhs[ 0 ][ 0 ] ), Rows, Cols > \
    { \
        auto m = matrix< \
            decltype( lhs[ 0 ][ 0 ] OPERAND rhs ), \
            Rows, \
            Cols \
        >::make_uninitialized(); \
        for( unsigned int i = 0; i < Rows; ++i ) \
            for( unsigned int j = 0; j < Cols; ++j ) \
                m[ j ][ i ] = lhs OPERAND rhs[ j ][ i ]; \
        return m; \
    }
    
    DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND( + )
    DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND( - )
    DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND( * )
    DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND( / )
    DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND( % )
    
    #undef DEFINE_OPERATORS_FOR_BINARY_SCALAR_OPERAND
}


namespace JadeMatrix::yavsg // Assignment scalar operators /////////////////////
{
    #define DEFINE_OPERATORS_FOR_ASSIGNMENT_SCALAR_OPERAND( OPERAND ) \
    template< \
        typename L, \
        unsigned int Rows, \
        unsigned int Cols, \
        typename R \
    > \
    constexpr \
    matrix< L, Rows, Cols >& operator OPERAND##= ( \
        matrix< L, Rows, Cols >& lhs, \
        const R& rhs \
    ) \
    { \
        for( auto& row : lhs ) \
            for( auto& e : row ) \
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


namespace JadeMatrix::yavsg // Binary matrix operators /////////////////////////
{
    #define DEFINE_OPERATORS_FOR_BINARY_MATRIX_OPERAND( OPERAND ) \
    template< \
        typename L, \
        typename R, \
        unsigned int Rows, \
        unsigned int Cols \
    > \
    constexpr \
    auto operator OPERAND ( \
        const matrix< L, Rows, Cols >& lhs, \
        const matrix< R, Rows, Cols >& rhs \
    ) -> matrix< decltype( lhs[ 0 ][ 0 ] OPERAND rhs[ 0 ][ 0 ] ), Rows, Cols > \
    { \
        auto m = matrix< \
            decltype( lhs[ 0 ][ 0 ] OPERAND rhs[ 0 ][ 0 ] ), \
            Rows, \
            Cols \
        >::make_uninitialized(); \
        for( unsigned int i = 0; i < Rows; ++i ) \
            for( unsigned int j = 0; j < Cols; ++j ) \
                m[ j ][ i ] = lhs[ j ][ i ] OPERAND rhs[ j ][ i ]; \
        return m; \
    }
    
    DEFINE_OPERATORS_FOR_BINARY_MATRIX_OPERAND( + )
    DEFINE_OPERATORS_FOR_BINARY_MATRIX_OPERAND( - )
    
    #undef DEFINE_OPERATORS_FOR_BINARY_MATRIX_OPERAND
}


namespace JadeMatrix::yavsg // Assignment matrix operators /////////////////////
{
    #define DEFINE_OPERATORS_FOR_ASSIGNMENT_MATRIX_OPERAND( OPERAND ) \
    template< \
        typename L, \
        unsigned int Rows, \
        unsigned int Cols, \
        typename R \
    > \
    constexpr \
    matrix< L, Rows, Cols >& operator OPERAND##= ( \
        matrix< L, Rows, Cols >& lhs, \
        matrix< R, Rows, Cols >&& rhs \
    ) \
    { \
        for( auto& row : lhs ) \
            for( auto& scalar : row ) \
                scalar OPERAND##= rhs; \
        return lhs; \
    }
    
    DEFINE_OPERATORS_FOR_ASSIGNMENT_MATRIX_OPERAND( + )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_MATRIX_OPERAND( - )
    
    #undef DEFINE_OPERATORS_FOR_ASSIGNMENT_MATRIX_OPERAND
}


namespace JadeMatrix::yavsg // Equality operators //////////////////////////////
{
    template<
        typename L,
        unsigned int Rows,
        unsigned int Cols,
        typename R
    >
    constexpr
    auto operator == (
        const matrix< L, Rows, Cols >& lhs,
        const matrix< R, Rows, Cols >& rhs
    )
        -> decltype( lhs[ 0 ][ 0 ] == rhs[ 0 ][ 0 ] )
    {
        for( unsigned int i = 0; i < Rows; ++i )
            for( unsigned int j = 0; j < Cols; ++j )
                if( !( lhs[ j ][ i ] == rhs[ j ][ i ] ) )
                    return lhs[ j ][ i ] == rhs[ j ][ i ];
        return lhs[ 0 ][ 0 ] == rhs[ 0 ][ 0 ];
    }
    
    template<
        typename L,
        unsigned int Rows,
        unsigned int Cols,
        typename R
    >
    constexpr
    auto operator != (
        const matrix< L, Rows, Cols >& lhs,
        const matrix< R, Rows, Cols >& rhs
    )
        -> decltype( lhs[ 0 ][ 0 ] != rhs[ 0 ][ 0 ] )
    {
        for( unsigned int i = 0; i < Rows; ++i )
            for( unsigned int j = 0; j < Cols; ++j )
                if( lhs[ j ][ i ] != rhs[ j ][ i ] )
                    return lhs[ j ][ i ] != rhs[ j ][ i ];
        return lhs[ 0 ][ 0 ] != rhs[ 0 ][ 0 ];
    }
}


namespace JadeMatrix::yavsg // Stream formatting operators & pretty-string /////
{
    template< typename T, unsigned int Rows, unsigned int Cols >
    std::ostream& operator<<(
        std::ostream& out, const matrix< T, Rows, Cols >& m
    )
    {
        out << "[";
        for( unsigned int i = 0; i < Rows; ++i )
        {
            out << "<";
            for( unsigned int j = 0; j < Cols; ++j )
            {
                out << m[ j ][ i ];
                if( j + 1 < Cols )
                    out << ",";
            }
            out << ">";
            if( i + 1 < Rows )
                out << ",";
        }
        return out << "]";
    }
    
    // Not terribly efficient, but useful for debugging purposes
    template< typename T, unsigned int Rows, unsigned int Cols >
    std::string pretty_string( const matrix< T, Rows, Cols >& m )
    {
        std::string matrix_string;
        
        std::array< std::array< std::string, Rows >, Cols > strings;
        std::string::size_type max_pad_before = 0;
        std::string::size_type max_pad_after  = 0;
        bool has_any_decimals = false;
        
        for( unsigned int i = 0; i < Rows; ++i )
            for( unsigned int j = 0; j < Cols; ++j )
            {
                std::stringstream ss;
                ss << m[ j ][ i ];
                strings[ j ][ i ] = ss.str();
                
                std::string::size_type before = strings[ j ][ i ].find( "." );
                std::string::size_type after;
                
                if( before == std::string::npos )
                {
                    before = strings[ j ][ i ].size();
                    after  = 0;
                }
                else
                {
                    has_any_decimals = true;
                    after = strings[ j ][ i ].size() - ( before + 1 );
                }
                
                if( before > max_pad_before )
                    max_pad_before = before;
                if( after > max_pad_after )
                    max_pad_after = after;
            }
        
        for( unsigned int i = 0; i < Rows; ++i )
        {
            // if( i == 0 )
            //     matrix_string += "[ < ";
            // else
            //     matrix_string += "  < ";
            if( i == 0 )
                matrix_string += "⎡ ";
            else if( i + 1 < Rows )
                matrix_string += "⎢ ";
            else
                matrix_string += "⎣ ";
            
            for( unsigned int j = 0; j < Cols; ++j )
            {
                auto decimal = (
                    has_any_decimals
                    ? strings[ j ][ i ].find( "." )
                    : std::string::npos
                );
                
                if( decimal == std::string::npos )
                    matrix_string += std::string(
                        max_pad_before - strings[ j ][ i ].size(),
                        ' '
                    );
                else
                    matrix_string += std::string(
                        max_pad_before - decimal,
                        ' '
                    );
                
                matrix_string += strings[ j ][ i ];
                
                if( has_any_decimals )
                {
                    if( decimal == std::string::npos )
                        matrix_string += std::string(
                            max_pad_after + 1,
                            ' '
                        );
                    else
                        matrix_string += std::string(
                            max_pad_after - (
                                strings[ j ][ i ].size()
                                - decimal
                                - 1
                            ),
                            ' '
                        );
                }
                
                if( j + 1 < Cols )
                    matrix_string += ", ";
            }
            
            // if( i + 1 < Rows )
            //     matrix_string += " >,\n";
            // else
            //     matrix_string += " > ]";
            if( i == 0 )
                matrix_string += " ⎤\n";
            else if( i + 1 < Rows )
                matrix_string += " ⎥\n";
            else
                matrix_string += " ⎦";
        }
        return matrix_string;
    }
}
