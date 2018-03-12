#pragma once
#ifndef YAVSG_MATH_QUATERNION_HPP
#define YAVSG_MATH_QUATERNION_HPP


#include "vector.hpp"
#include "scalar_operations.hpp"
#include "trigonometry.hpp"
#include "../units/angular.hpp"

#include <initializer_list>
#include <ostream>


namespace yavsg
{
    template< typename T > class quaternion;
    template< typename T > class versor;
    
    enum quaternion_dimension
    {
        W = 0,
        I = 1,
        J = 2,
        K = 3
    };
    
    template< typename T > class quaternion
    {
    protected:
        vector< T, 4 > values;
        
    public:
        template< typename O > constexpr quaternion(
            const quaternion< O >& o
        ) : values( {
            static_cast< T >( o[ W ] ),
            static_cast< T >( o[ I ] ),
            static_cast< T >( o[ J ] ),
            static_cast< T >( o[ K ] )
        } )
        {}
        
        template< typename O > constexpr quaternion(
            const vector< O, 4 >& o
        ) : values( {
            static_cast< T >( o[ W ] ),
            static_cast< T >( o[ I ] ),
            static_cast< T >( o[ J ] ),
            static_cast< T >( o[ K ] )
        } )
        {}
        
        // TODO: template< typename... O > ... std::initializer_list< O > ...
        constexpr quaternion( std::initializer_list< T > il ) :
            values( vector< T, 4 >::make_uninitialized() )
        {
            // Implements same semantics as partial array initialization via
            // brace-enclosed lists (uninitialized members become 0)
            auto iter = il.begin();
            for( unsigned int i = 0; i < 4; ++i )
                if( iter == il.end() )
                    values[ i ] = 0;
                else
                {
                    values[ i ] = *iter;
                    ++iter;
                }
        }
        
        constexpr const T& operator[]( quaternion_dimension i ) const
        {
            return values[ i ];
        }
        constexpr const T& operator[]( int i ) const
        {
            return values[ i ];
        }
        
        constexpr auto norm() -> decltype(
            sqrt( values[ 0 ] * values[ 0 ] + values[ 0 ] * values[ 0 ] )
        )
        {
            return sqrt(
                  values[ W ] * values[ W ]
                + values[ I ] * values[ I ]
                + values[ J ] * values[ J ]
                + values[ K ] * values[ K ]
            );
        }
        
        template< typename O > constexpr explicit operator versor< O >() const;
    };
    
    template< typename T > class versor : public quaternion< T >
    {
    public:
        using q_type = quaternion< T >;
        
        template< typename O > constexpr versor(
            const versor< O >& o
        ) : q_type( { o[ W ], o[ I ], o[ J ], o[ K ] } )
        {
            q_type::values = q_type::values.unit();
        }
        
        template< typename O > constexpr versor(
            const vector< O, 4 >& o
        ) : q_type( { o[ 0 ], o[ 1 ], o[ 2 ], o[ 3 ] } )
        {
            q_type::values = q_type::values.unit();
        }
        
        constexpr versor( std::initializer_list< T > il ) :
            q_type( il )
        {
            q_type::values = q_type::values.unit();
        }
        
        template< typename R, typename A >
        static constexpr versor< T > from_euler(
            // const radians< R >& r,
            const R& r,
            const vector< A, 3 >& axis
        )
        {
            auto rads = radians< T >{ r };
            return {
                // static_cast< T >( axis[ 0 ] * sin( r / 2 ) ),
                // static_cast< T >( axis[ 1 ] * sin( r / 2 ) ),
                // static_cast< T >( axis[ 2 ] * sin( r / 2 ) ),
                // static_cast< T >(             cos( r / 2 ) )
                static_cast< T >(             cos( rads / 2 ) ),
                static_cast< T >( axis[ 0 ] * sin( rads / 2 ) ),
                static_cast< T >( axis[ 1 ] * sin( rads / 2 ) ),
                static_cast< T >( axis[ 2 ] * sin( rads / 2 ) )
            };
        }
        
        template< typename R, typename P, typename Y >
        static constexpr versor< T > from_rpy(
            const radians< R >& roll,
            const radians< P >& pitch,
            const radians< Y >& yaw
        )
        {
            return {
                static_cast< T >( cos( roll / 2 ) * cos( pitch / 2 ) * cos( yaw / 2 ) + sin( roll / 2 ) * sin( pitch / 2 ) * sin( yaw / 2 ) ),
                static_cast< T >( sin( roll / 2 ) * cos( pitch / 2 ) * cos( yaw / 2 ) - cos( roll / 2 ) * sin( pitch / 2 ) * sin( yaw / 2 ) ),
                static_cast< T >( cos( roll / 2 ) * sin( pitch / 2 ) * cos( yaw / 2 ) + sin( roll / 2 ) * cos( pitch / 2 ) * sin( yaw / 2 ) ),
                static_cast< T >( cos( roll / 2 ) * cos( pitch / 2 ) * sin( yaw / 2 ) - sin( roll / 2 ) * sin( pitch / 2 ) * cos( yaw / 2 ) )
            };
        }
        
        constexpr auto norm() -> decltype( sqrt(
              q_type::values[ 0 ] * q_type::values[ 0 ]
            + q_type::values[ 0 ] * q_type::values[ 0 ]
        ) )
        {
            return 1;
        }
        
        template< typename O > constexpr operator quaternion< O >()
        {
            return q_type::values;
        }
    };
    
    template< typename T >
    template< typename O >
    constexpr
    quaternion< T >::operator versor< O >() const
    {
        return values;
    }
}


namespace yavsg // Quaternion multiplication ///////////////////////////////////
{
    template< typename L, typename R >
    constexpr
    auto operator*( const quaternion< L >& lhs, const quaternion< R >& rhs )
        -> quaternion< decltype( lhs[ 0 ] * rhs[ 0 ] ) >
    {
        return quaternion< decltype( lhs[ 0 ] * rhs[ 0 ] ) >( {
            ( lhs[ W ] * rhs[ W ] - lhs[ I ] * rhs[ I ] - lhs[ J ] * rhs[ J ] - lhs[ K ] * rhs[ K ] ),
            ( lhs[ I ] * rhs[ W ] + lhs[ W ] * rhs[ I ] - lhs[ K ] * rhs[ J ] + lhs[ J ] * rhs[ K ] ),
            ( lhs[ J ] * rhs[ W ] + lhs[ K ] * rhs[ I ] + lhs[ W ] * rhs[ J ] - lhs[ I ] * rhs[ K ] ),
            ( lhs[ K ] * rhs[ W ] - lhs[ J ] * rhs[ I ] + lhs[ I ] * rhs[ J ] + lhs[ W ] * rhs[ K ] )
        } );
    }
}


namespace yavsg // Stream formatting operator //////////////////////////////////
{
    template< typename T >
    std::ostream& operator<<( std::ostream& out, const quaternion< T >& q )
    {
        return out
            << "("
            << q[ W ] << " + "
            << q[ I ] << "i + "
            << q[ J ] << "j + "
            << q[ K ] << "k"
            << ")"
        ;
    }
}


#endif
