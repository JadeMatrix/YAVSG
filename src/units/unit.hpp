#pragma once
#ifndef YAVSG_UNITS_UNIT_HPP
#define YAVSG_UNITS_UNIT_HPP


#include <string>
#include <type_traits>


namespace yavsg // Base unit classes ///////////////////////////////////////////
{
    template< typename T, class Traits > class unit
    {
    public:
        using  value_type = T;
        using traits_type = Traits;
        
    protected:
        value_type value;
        
    public:
        constexpr unit() {}
        constexpr unit( const T& v ) : value( v ) {}
        
        // template< typename O > unit(
        //     const unit< O, Traits >& o
        // ) : value( ( T )o ) {}
        
        template< typename O, class Other_Traits >
        constexpr
        unit( const unit< O, Other_Traits >& o ) :
            value( traits_type::convert_from( o ) )
        {}
        
        explicit constexpr operator T () const
        {
            return value;
        }
        template< typename O > explicit constexpr operator O () const
        {
            return ( O )value;
        }
    };
    
    template<
        typename T,
        class Numer_Traits,
        class Denom_Traits
    > struct per_traits
    {
        // TODO: recursive specialization to reduce the number of parentheses
        // needed?
        static std::string unit_name()
        {
            return (
                "("
                + Numer_Traits::unit_name()
                + ")/("
                + Denom_Traits::unit_name()
                + ")"
            );
        }
        static std::string unit_symbol()
        {
            return (
                "("
                + Numer_Traits::unit_symbol()
                + ")/("
                + Denom_Traits::unit_symbol()
                + ")"
            );
        }
        
        template<
            typename O,
            class Other_Numer_Traits,
            class Other_Denom_Traits
        >
        static constexpr
        T convert_from(
            const unit<O, per_traits< O, Other_Numer_Traits, Other_Denom_Traits > >& from
        )
        {
            return (
                Numer_Traits::convert_from(
                    unit< O, Other_Numer_Traits >( ( O )from )
                ) /
                Denom_Traits::convert_from(
                    unit< O, Other_Denom_Traits >( 1 )
                )
            );
        }
    };
    
    template<
        typename T,
        class Left_Traits,
        class Right_Traits
    > struct by_traits
    {
        // TODO: recursive specialization to reduce the number of parentheses
        // needed?
        static std::string unit_name()
        {
            return (
                "("
                + Left_Traits::unit_name()
                + ")*("
                + Right_Traits::unit_name()
                + ")"
            );
        }
        static std::string unit_symbol()
        {
            return (
                "("
                + Left_Traits::unit_symbol()
                + ")*("
                + Right_Traits::unit_symbol()
                + ")"
            );
        }
        
        template<
            typename O,
            class Other_Left_Traits,
            class Other_Right_Traits
        >
        static constexpr
        T convert_from(
            const unit<O, by_traits< O, Other_Left_Traits, Other_Right_Traits > >& from
        )
        {
            return (
                Left_Traits::convert_from(
                    unit< O, Other_Left_Traits >( ( O )from )
                ) *
                Right_Traits::convert_from(
                    unit< O, Other_Right_Traits >( 1 )
                )
            );
        }
    };
    
    template< typename T > struct ratio_traits
    {
        static constexpr const char* unit_name()
        {
            return "";
        }
        static constexpr const char* unit_symbol()
        {
            return "";
        }
        
        template< typename O >
        static constexpr
        T convert_from( const O& from )
        {
            // O must have an implicit conversion to T
            return from;
        }
    };
    
    template<
        typename T,
        class Numer_Traits,
        class Denom_Traits
    > using per = unit< T, per_traits< T, Numer_Traits, Denom_Traits > >;
    template<
        typename T,
        class Left_Traits,
        class Right_Traits
    > using by = unit< T, by_traits< T, Left_Traits, Right_Traits > >;
    template< typename T > using ratio = unit< T, ratio_traits< T > >;
    
    // unit<> specialization for ratios to add implicit cast to other types
    template< typename T > class unit< T, ratio_traits< T > >
    {
    public:
        using  value_type = T;
        using traits_type = ratio_traits< T >;
        
    protected:
        value_type value;
        
    public:
        constexpr unit() {}
        constexpr unit( const T& v ) : value( v ) {}
        
        template< typename O, class Other_Traits >
        constexpr
        unit( const unit< O, Other_Traits >& o ) :
            value( traits_type::convert_from( o ) )
        {}
        
        constexpr operator T () const
        {
            return value;
        }
        template< typename O > explicit constexpr operator O () const
        {
            return ( O )value;
        }
    };
}


/* NOTE:
The pattern
    return ( L )lhs OPERATOR ( R )LTraits< R >::convert_from( rhs );
makes a conversion to the left-hand unit for the operation while keeping the
right-hand side the same value type.
*/


namespace yavsg // Binary operators ////////////////////////////////////////////
{
    #define DEFINE_OPERATORS_FOR_BINARY_OPERAND_NONUNIT( OPERAND ) \
    template< \
        typename L, \
        template< typename > class LTraits, \
        typename R, \
        typename std::enable_if< std::is_arithmetic< R >::value, R >::type = 0 \
    > \
    constexpr \
    auto operator OPERAND( const unit< L, LTraits< L > >& lhs, const R& rhs ) \
        -> unit< \
                     decltype( ( L )lhs OPERAND ( L )rhs ), \
            LTraits< decltype( ( L )lhs OPERAND ( L )rhs ) > \
        > \
    { \
        return ( L )lhs OPERAND ( L )rhs; \
    } \
     \
    template< \
        typename L, \
        typename R, \
        template< typename > class RTraits, \
        typename std::enable_if< std::is_arithmetic< L >::value, L >::type = 0 \
    > \
    constexpr \
    auto operator OPERAND( const L& lhs, const unit< R, RTraits< R > >& rhs ) \
        -> unit< \
                     decltype( ( R )lhs OPERAND ( R )rhs ), \
            RTraits< decltype( ( R )lhs OPERAND ( R )rhs ) > \
        > \
    { \
        return ( R )lhs OPERAND ( R )rhs; \
    }
    
    #define DEFINE_OPERATORS_FOR_BINARY_OPERAND_UNIT( OPERAND ) \
    template< \
        typename L, \
        template< typename > class LTraits, \
        typename R, \
        class RTraits \
    > \
    constexpr \
    auto operator OPERAND( \
        const unit< L, LTraits< L > >& lhs, \
        const unit< R, RTraits      >& rhs \
    ) -> unit< \
                 decltype( ( L )lhs OPERAND ( R )LTraits< R >::convert_from( rhs ) ), \
        LTraits< decltype( ( L )lhs OPERAND ( R )LTraits< R >::convert_from( rhs ) ) > \
    > \
    { \
        return ( L )lhs OPERAND ( R )LTraits< R >::convert_from( rhs ); \
    }
    
    DEFINE_OPERATORS_FOR_BINARY_OPERAND_NONUNIT( + )
    DEFINE_OPERATORS_FOR_BINARY_OPERAND_NONUNIT( - )
    DEFINE_OPERATORS_FOR_BINARY_OPERAND_NONUNIT( * )
    DEFINE_OPERATORS_FOR_BINARY_OPERAND_NONUNIT( / )
    DEFINE_OPERATORS_FOR_BINARY_OPERAND_NONUNIT( % )
    DEFINE_OPERATORS_FOR_BINARY_OPERAND_NONUNIT( & )
    DEFINE_OPERATORS_FOR_BINARY_OPERAND_NONUNIT( | )
    DEFINE_OPERATORS_FOR_BINARY_OPERAND_NONUNIT( ^ )
    
    DEFINE_OPERATORS_FOR_BINARY_OPERAND_UNIT( + )
    DEFINE_OPERATORS_FOR_BINARY_OPERAND_UNIT( - )
    // DEFINE_OPERATORS_FOR_BINARY_OPERAND_UNIT( * ) Special case below
    // DEFINE_OPERATORS_FOR_BINARY_OPERAND_UNIT( / ) Special case below
    // DEFINE_OPERATORS_FOR_BINARY_OPERAND_UNIT( % ) No way to preserve units
    // DEFINE_OPERATORS_FOR_BINARY_OPERAND_UNIT( & ) No way to preserve units
    // DEFINE_OPERATORS_FOR_BINARY_OPERAND_UNIT( | ) No way to preserve units
    // DEFINE_OPERATORS_FOR_BINARY_OPERAND_UNIT( ^ ) No way to preserve units
    
    #undef DEFINE_OPERATORS_FOR_BINARY_OPERAND_NONUNIT
    #undef DEFINE_OPERATORS_FOR_BINARY_OPERAND_UNIT
    
    template< // Multiplying two units produces a by unit //////////////////////
        typename L,
        template< typename > class LTraits,
        typename R,
        class RTraits
    >
    constexpr
    auto operator *(
        const unit< L, LTraits< L > >& lhs,
        const unit< R, RTraits      >& rhs
    ) -> by<
                 decltype( ( L )lhs * ( R )LTraits< R >::convert_from( rhs ) ),
        LTraits< decltype( ( L )lhs * ( R )LTraits< R >::convert_from( rhs ) ) >,
        RTraits
    >
    {
        return ( L )lhs * ( R )LTraits< R >::convert_from( rhs );
    }
    
    template< // Multiplying two ratios produces a ratio ///////////////////////
        typename L,
        typename R
    >
    constexpr
    auto operator *(
        const ratio< L >& lhs,
        const ratio< R >& rhs
    ) -> ratio< decltype( ( L )lhs * ( R )rhs ) >
    {
        return ( L )lhs * ( R )rhs;
    }
    
    template< // Dividing two units produces a per unit ////////////////////////
        typename L,
        template< typename > class LTraits,
        typename R,
        class RTraits
    >
    constexpr
    auto operator /(
        const unit< L, LTraits< L > >& lhs,
        const unit< R, RTraits      >& rhs
    ) -> per<
                 decltype( ( L )lhs / ( R )LTraits< R >::convert_from( rhs ) ),
        LTraits< decltype( ( L )lhs / ( R )LTraits< R >::convert_from( rhs ) ) >,
        RTraits
    >
    {
        return ( L )lhs / ( R )LTraits< R >::convert_from( rhs );
    }
    
    template< // Dividing two of the same unit produces a ratio unit ///////////
        typename L,
        typename R,
        template< typename > class Traits
    >
    constexpr
    auto operator /(
        const unit< L, Traits< L > >& lhs,
        const unit< R, Traits< R > >& rhs
    ) -> ratio< decltype( ( L )lhs / ( R )Traits< R >::convert_from( rhs ) ) >
    {
        return ( L )lhs / ( R )Traits< R >::convert_from( rhs );
    }
}


namespace yavsg // Comparison operators ////////////////////////////////////////
{
    #define DEFINE_OPERATORS_FOR_COMPARISON_OPERAND( OPERAND ) \
    template< \
        typename L, \
        class LTraits, \
        typename R, \
        typename std::enable_if< std::is_arithmetic< R >::value, R >::type = 0 \
    > \
    constexpr \
    auto operator OPERAND( const unit< L, LTraits >& lhs, const R& rhs ) \
        -> decltype( ( L )lhs OPERAND rhs ) \
    { \
        return ( L )lhs OPERAND rhs; \
    } \
     \
    template< \
        typename L, \
        typename R, \
        class RTraits, \
        typename std::enable_if< std::is_arithmetic< L >::value, L >::type = 0 \
    > \
    constexpr \
    auto operator OPERAND( const L& lhs, const unit< R, RTraits >& rhs ) \
        -> decltype( lhs OPERAND ( R )rhs ) \
    { \
        return lhs OPERAND ( R )rhs; \
    } \
     \
    template< \
        typename L, \
        template< typename > class LTraits, \
        typename R, \
        class RTraits \
    > \
    constexpr \
    auto operator OPERAND( \
        const unit< L, LTraits< L > >& lhs, \
        const unit< R, RTraits      >& rhs \
    ) -> decltype( ( L )lhs OPERAND ( R )LTraits< R >::convert_from( rhs ) ) \
    { \
        return ( L )lhs OPERAND ( R )LTraits< R >::convert_from( rhs ); \
    }
    
    DEFINE_OPERATORS_FOR_COMPARISON_OPERAND( == )
    DEFINE_OPERATORS_FOR_COMPARISON_OPERAND( != )
    DEFINE_OPERATORS_FOR_COMPARISON_OPERAND( <  )
    DEFINE_OPERATORS_FOR_COMPARISON_OPERAND( >  )
    DEFINE_OPERATORS_FOR_COMPARISON_OPERAND( <= )
    DEFINE_OPERATORS_FOR_COMPARISON_OPERAND( >= )
    
    #undef DEFINE_OPERATORS_FOR_COMPARISON_OPERAND
}


namespace yavsg // Assignment operators ////////////////////////////////////////
{
    #define DEFINE_OPERATORS_FOR_ASSIGNMENT_OPERAND( OPERAND, BASE_OPERAND ) \
    template< \
        typename L, \
        class LTraits, \
        typename R, \
        typename std::enable_if< std::is_arithmetic< R >::value, R >::type = 0 \
    > \
    unit< L, LTraits >& operator OPERAND( \
        unit< L, LTraits >& lhs, \
        const R& rhs \
    ) \
    { \
        lhs = ( L )lhs BASE_OPERAND rhs; \
        return lhs; \
    } \
     \
    template< \
        typename L, \
        typename R, \
        class RTraits, \
        typename std::enable_if< std::is_arithmetic< L >::value, L >::type = 0 \
    > \
    L& operator OPERAND( L& lhs, const unit< R, RTraits >& rhs ) \
    { \
        return lhs OPERAND ( R )rhs; \
    }
    
    DEFINE_OPERATORS_FOR_ASSIGNMENT_OPERAND( +=, + )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_OPERAND( -=, - )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_OPERAND( *=, * )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_OPERAND( /=, / )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_OPERAND( %=, % )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_OPERAND( &=, & )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_OPERAND( |=, | )
    DEFINE_OPERATORS_FOR_ASSIGNMENT_OPERAND( ^=, ^ )
    
    #undef DEFINE_OPERATORS_FOR_ASSIGNMENT_OPERAND
}


namespace yavsg // Unary operators /////////////////////////////////////////////
{
    #define DEFINE_OPERATORS_FOR_UNARY_OPERAND( OPERAND ) \
    template< typename R, template< typename > class RTraits > \
    constexpr \
    auto operator OPERAND( const unit< R, RTraits< R > >& rhs ) \
        -> unit< \
                     decltype( OPERAND( R )rhs ), \
            RTraits< decltype( OPERAND( R )rhs ) > \
        > \
    { \
        return OPERAND( R )rhs; \
    }
    
    DEFINE_OPERATORS_FOR_UNARY_OPERAND( + )
    DEFINE_OPERATORS_FOR_UNARY_OPERAND( - )
    DEFINE_OPERATORS_FOR_UNARY_OPERAND( ~ )
    
    #undef DEFINE_OPERATORS_FOR_UNARY_OPERAND
}


// namespace yavsg // Shift operators /////////////////////////////////////////////
// {
//     #define DEFINE_OPERATORS_FOR_BINARY_SHIFT_OPERAND( OPERAND ) \
//     template< typename L, template< typename > class LTraits > \
//     constexpr \
//     auto operator OPERAND( const unit< L, LTraits< L > >& lhs, int rhs ) \
//         -> unit< \
//                      decltype( ( L )lhs OPERAND rhs ), \
//             LTraits< decltype( ( L )lhs OPERAND rhs ) > \
//         > \
//     { \
//         return ( L )lhs OPERAND rhs; \
//     }
    
//     DEFINE_OPERATORS_FOR_BINARY_SHIFT_OPERAND( << )
//     DEFINE_OPERATORS_FOR_BINARY_SHIFT_OPERAND( >> )
    
//     #undef DEFINE_OPERATORS_FOR_BINARY_SHIFT_OPERAND
    
//     #define DEFINE_OPERATORS_FOR_ASSIGNMENT_SHIFT_OPERAND( \
//         OPERAND, \
//         BASE_OPERAND \
//     ) \
//     template< typename L, class LTraits > \
//     unit< L, LTraits >& operator OPERAND( unit< L, LTraits >& lhs, int rhs ) \
//     { \
//         lhs = ( L )lhs BASE_OPERAND rhs; \
//         return lhs; \
//     }
    
//     DEFINE_OPERATORS_FOR_ASSIGNMENT_SHIFT_OPERAND( <<=, << )
//     DEFINE_OPERATORS_FOR_ASSIGNMENT_SHIFT_OPERAND( >>=, >> )
    
//     #undef DEFINE_OPERATORS_FOR_ASSIGNMENT_SHIFT_OPERAND
// }


namespace yavsg // Stream formatting operator //////////////////////////////////
{
    template< typename T, class Traits >
    std::ostream& operator<<( std::ostream& out, const unit< T, Traits >& u )
    {
        return out << ( T )u << Traits::unit_symbol();
    }
}


#endif
