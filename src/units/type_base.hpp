#pragma once
#ifndef GL_MATH_TYPE_BASE_HPP
#define GL_MATH_TYPE_BASE_HPP


#define SCALAR_TYPE_INSIDE_CLASS_DEF( CLASSNAME, T )                                                                                                                                        \
template< typename > friend class CLASSNAME;                                                                                                                                                \
protected: T value;                                                                                                                                                                         \
                                                                                                                                                                                            \
template< typename L, typename R > friend constexpr           auto  operator== ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value == rhs       ) >; \
template< typename L, typename R > friend constexpr           auto  operator!= ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value != rhs       ) >; \
template< typename L, typename R > friend constexpr           auto  operator<  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value <  rhs       ) >; \
template< typename L, typename R > friend constexpr           auto  operator>  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value >  rhs       ) >; \
template< typename L, typename R > friend constexpr           auto  operator<= ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value <= rhs       ) >; \
template< typename L, typename R > friend constexpr           auto  operator>= ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value >= rhs       ) >; \
template< typename L, typename R > friend constexpr           auto  operator== ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       == rhs.value ) >; \
template< typename L, typename R > friend constexpr           auto  operator!= ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       != rhs.value ) >; \
template< typename L, typename R > friend constexpr           auto  operator<  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       <  rhs.value ) >; \
template< typename L, typename R > friend constexpr           auto  operator>  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       >  rhs.value ) >; \
template< typename L, typename R > friend constexpr           auto  operator<= ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       <= rhs.value ) >; \
template< typename L, typename R > friend constexpr           auto  operator>= ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       >= rhs.value ) >; \
template< typename L, typename R > friend constexpr           auto  operator== ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value == rhs.value ) >; \
template< typename L, typename R > friend constexpr           auto  operator!= ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value != rhs.value ) >; \
template< typename L, typename R > friend constexpr           auto  operator<  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value <  rhs.value ) >; \
template< typename L, typename R > friend constexpr           auto  operator>  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value >  rhs.value ) >; \
template< typename L, typename R > friend constexpr           auto  operator<= ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value <= rhs.value ) >; \
template< typename L, typename R > friend constexpr           auto  operator>= ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value >= rhs.value ) >; \
                                                                                                                                                                                            \
template<             typename R > friend constexpr           auto  operator+  (                            const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( +rhs.value             ) >; \
template<             typename R > friend constexpr           auto  operator-  (                            const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( -rhs.value             ) >; \
                                                                                                                                                                                            \
template< typename L, typename R > friend constexpr           auto  operator+  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value + rhs        ) >; \
template< typename L, typename R > friend constexpr           auto  operator-  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value - rhs        ) >; \
template< typename L, typename R > friend constexpr           auto  operator*  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value * rhs        ) >; \
template< typename L, typename R > friend constexpr           auto  operator/  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value / rhs        ) >; \
template< typename L, typename R > friend constexpr           auto  operator%  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value % rhs        ) >; \
template< typename L, typename R > friend constexpr           auto  operator+  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       + rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator-  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       - rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator*  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       * rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator/  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       / rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator%  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       % rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator+  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value + rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator-  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value - rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator*  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value * rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator/  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value / rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator%  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value % rhs.value  ) >; \
                                                                                                                                                                                            \
template< typename L, typename R > friend           CLASSNAME< L >& operator+= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator-= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator*= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator/= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator%= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                   ; \
template< typename L, typename R > friend                      L  & operator+= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend                      L  & operator-= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend                      L  & operator*= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend                      L  & operator/= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend                      L  & operator%= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator+= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator-= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator*= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator/= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator%= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                   ; \
                                                                                                                                                                                            \
template< typename L             > friend constexpr           auto  operator<< ( const CLASSNAME< L >& lhs, int                   rhs ) -> CLASSNAME< decltype( lhs.value << rhs       ) >; \
template< typename L             > friend constexpr           auto  operator>> ( const CLASSNAME< L >& lhs, int                   rhs ) -> CLASSNAME< decltype( lhs.value >> rhs       ) >; \
template< typename L             > friend           CLASSNAME< L >& operator<<=(       CLASSNAME< L >& lhs, int                   rhs )                                                   ; \
template< typename L             > friend           CLASSNAME< L >& operator>>=(       CLASSNAME< L >& lhs, int                   rhs )                                                   ; \
                                                                                                                                                                                            \
template< typename L             > friend constexpr           auto  operator~  ( const CLASSNAME< L >& lhs                            ) -> CLASSNAME< decltype( ~lhs                   ) >; \
template< typename L, typename R > friend constexpr           auto  operator&  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value & rhs        ) >; \
template< typename L, typename R > friend constexpr           auto  operator|  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value | rhs        ) >; \
template< typename L, typename R > friend constexpr           auto  operator^  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value ^ rhs        ) >; \
template< typename L, typename R > friend constexpr           auto  operator&  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       & rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator|  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       | rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator^  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       ^ rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator&  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value & rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator|  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value | rhs.value  ) >; \
template< typename L, typename R > friend constexpr           auto  operator^  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value ^ rhs.value  ) >; \
                                                                                                                                                                                            \
template< typename L, typename R > friend           CLASSNAME< L >& operator&= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator|= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator^= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                   ; \
template< typename L, typename R > friend                      L  & operator&= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend                      L  & operator|= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend                      L  & operator^= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator&= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator|= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                   ; \
template< typename L, typename R > friend           CLASSNAME< L >& operator^= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                   ; \
public:                                                                                                                                                                                     \
template< typename O > constexpr CLASSNAME( const            O  & o ) : value( o       ) {}                                                                                                 \
                       constexpr CLASSNAME( const CLASSNAME< T >& o ) : value( o.value ) {}                                                                                                 \
template< typename O >           CLASSNAME& operator=  ( const            O  & rhs )       { value = rhs      ; return *this; }                                                             \
                                 CLASSNAME& operator=  ( const CLASSNAME< T >& rhs )       { value = rhs.value; return *this; }                                                             \
template< typename O > constexpr            operator O (                           ) const { return ( O )( value );           }                                                             \
private:


#define SCALAR_TYPE_OUTSIDE_CLASS_DEF( CLASSNAME )                                                                                                                                                                          \
template< typename L, typename R > constexpr           auto  operator== ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value == rhs       ) > { return lhs.value == rhs      ;      } \
template< typename L, typename R > constexpr           auto  operator!= ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value != rhs       ) > { return lhs.value != rhs      ;      } \
template< typename L, typename R > constexpr           auto  operator<  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value <  rhs       ) > { return lhs.value <  rhs      ;      } \
template< typename L, typename R > constexpr           auto  operator>  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value >  rhs       ) > { return lhs.value >  rhs      ;      } \
template< typename L, typename R > constexpr           auto  operator<= ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value <= rhs       ) > { return lhs.value <= rhs      ;      } \
template< typename L, typename R > constexpr           auto  operator>= ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value >= rhs       ) > { return lhs.value >= rhs      ;      } \
template< typename L, typename R > constexpr           auto  operator== ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       == rhs.value ) > { return lhs       == rhs.value;      } \
template< typename L, typename R > constexpr           auto  operator!= ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       != rhs.value ) > { return lhs       != rhs.value;      } \
template< typename L, typename R > constexpr           auto  operator<  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       <  rhs.value ) > { return lhs       <  rhs.value;      } \
template< typename L, typename R > constexpr           auto  operator>  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       >  rhs.value ) > { return lhs       >  rhs.value;      } \
template< typename L, typename R > constexpr           auto  operator<= ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       <= rhs.value ) > { return lhs       <= rhs.value;      } \
template< typename L, typename R > constexpr           auto  operator>= ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       >= rhs.value ) > { return lhs       >= rhs.value;      } \
template< typename L, typename R > constexpr           auto  operator== ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value == rhs.value ) > { return lhs.value == rhs.value;      } \
template< typename L, typename R > constexpr           auto  operator!= ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value != rhs.value ) > { return lhs.value != rhs.value;      } \
template< typename L, typename R > constexpr           auto  operator<  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value <  rhs.value ) > { return lhs.value <  rhs.value;      } \
template< typename L, typename R > constexpr           auto  operator>  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value >  rhs.value ) > { return lhs.value >  rhs.value;      } \
template< typename L, typename R > constexpr           auto  operator<= ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value <= rhs.value ) > { return lhs.value <= rhs.value;      } \
template< typename L, typename R > constexpr           auto  operator>= ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value >= rhs.value ) > { return lhs.value >= rhs.value;      } \
                                                                                                                                                                                                                            \
template<             typename R > constexpr           auto  operator+  (                            const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( +rhs.value             ) > { return +rhs.value;                  } \
template<             typename R > constexpr           auto  operator-  (                            const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( -rhs.value             ) > { return -rhs.value;                  } \
                                                                                                                                                                                                                            \
template< typename L, typename R > constexpr           auto  operator+  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value + rhs        ) > { return lhs.value + rhs      ;       } \
template< typename L, typename R > constexpr           auto  operator-  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value - rhs        ) > { return lhs.value - rhs      ;       } \
template< typename L, typename R > constexpr           auto  operator*  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value * rhs        ) > { return lhs.value * rhs      ;       } \
template< typename L, typename R > constexpr           auto  operator/  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value / rhs        ) > { return lhs.value / rhs      ;       } \
template< typename L, typename R > constexpr           auto  operator%  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value % rhs        ) > { return lhs.value % rhs      ;       } \
template< typename L, typename R > constexpr           auto  operator+  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       + rhs.value  ) > { return lhs       + rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator-  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       - rhs.value  ) > { return lhs       - rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator*  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       * rhs.value  ) > { return lhs       * rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator/  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       / rhs.value  ) > { return lhs       / rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator%  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       % rhs.value  ) > { return lhs       % rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator+  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value + rhs.value  ) > { return lhs.value + rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator-  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value - rhs.value  ) > { return lhs.value - rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator*  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value * rhs.value  ) > { return lhs.value * rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator/  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value / rhs.value  ) > { return lhs.value / rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator%  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value % rhs.value  ) > { return lhs.value % rhs.value;       } \
                                                                                                                                                                                                                            \
template< typename L, typename R >           CLASSNAME< L >& operator+= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                    { lhs.value += rhs      ; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator-= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                    { lhs.value -= rhs      ; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator*= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                    { lhs.value *= rhs      ; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator/= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                    { lhs.value /= rhs      ; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator%= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                    { lhs.value %= rhs      ; return lhs; } \
template< typename L, typename R >                      L  & operator+= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                    { lhs       += rhs.value; return lhs; } \
template< typename L, typename R >                      L  & operator-= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                    { lhs       -= rhs.value; return lhs; } \
template< typename L, typename R >                      L  & operator*= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                    { lhs       *= rhs.value; return lhs; } \
template< typename L, typename R >                      L  & operator/= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                    { lhs       /= rhs.value; return lhs; } \
template< typename L, typename R >                      L  & operator%= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                    { lhs       %= rhs.value; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator+= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                    { lhs.value += rhs.value; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator-= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                    { lhs.value -= rhs.value; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator*= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                    { lhs.value *= rhs.value; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator/= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                    { lhs.value /= rhs.value; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator%= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                    { lhs.value %= rhs.value; return lhs; } \
                                                                                                                                                                                                                            \
template< typename L             > constexpr           auto  operator<< ( const CLASSNAME< L >& lhs, int                   rhs ) -> CLASSNAME< decltype( lhs.value << rhs       ) > { return lhs.value << rhs;            } \
template< typename L             > constexpr           auto  operator>> ( const CLASSNAME< L >& lhs, int                   rhs ) -> CLASSNAME< decltype( lhs.value >> rhs       ) > { return lhs.value >> rhs;            } \
template< typename L             >           CLASSNAME< L >& operator<<=(       CLASSNAME< L >& lhs, int                   rhs )                                                    { lhs.value <<= rhs; return lhs;      } \
template< typename L             >           CLASSNAME< L >& operator>>=(       CLASSNAME< L >& lhs, int                   rhs )                                                    { lhs.value >>= rhs; return lhs;      } \
                                                                                                                                                                                                                            \
template< typename L             > constexpr           auto  operator~  ( const CLASSNAME< L >& lhs                            ) -> CLASSNAME< decltype( ~lhs                   ) > { return ~lhs                 ;       } \
template< typename L, typename R > constexpr           auto  operator&  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value & rhs        ) > { return lhs.value & rhs      ;       } \
template< typename L, typename R > constexpr           auto  operator|  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value | rhs        ) > { return lhs.value | rhs      ;       } \
template< typename L, typename R > constexpr           auto  operator^  ( const CLASSNAME< L >& lhs, const            R  & rhs ) -> CLASSNAME< decltype( lhs.value ^ rhs        ) > { return lhs.value ^ rhs      ;       } \
template< typename L, typename R > constexpr           auto  operator&  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       & rhs.value  ) > { return lhs       & rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator|  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       | rhs.value  ) > { return lhs       | rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator^  ( const            L  & lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs       ^ rhs.value  ) > { return lhs       ^ rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator&  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value & rhs.value  ) > { return lhs.value & rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator|  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value | rhs.value  ) > { return lhs.value | rhs.value;       } \
template< typename L, typename R > constexpr           auto  operator^  ( const CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs ) -> CLASSNAME< decltype( lhs.value ^ rhs.value  ) > { return lhs.value ^ rhs.value;       } \
                                                                                                                                                                                                                            \
template< typename L, typename R >           CLASSNAME< L >& operator&= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                    { lhs.value &= rhs      ; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator|= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                    { lhs.value |= rhs      ; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator^= (       CLASSNAME< L >& lhs, const            R  & rhs )                                                    { lhs.value ^= rhs      ; return lhs; } \
template< typename L, typename R >                      L  & operator&= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                    { lhs       &= rhs.value; return lhs; } \
template< typename L, typename R >                      L  & operator|= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                    { lhs       |= rhs.value; return lhs; } \
template< typename L, typename R >                      L  & operator^= (                  L  & lhs, const CLASSNAME< R >& rhs )                                                    { lhs       ^= rhs.value; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator&= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                    { lhs.value &= rhs.value; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator|= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                    { lhs.value |= rhs.value; return lhs; } \
template< typename L, typename R >           CLASSNAME< L >& operator^= (       CLASSNAME< L >& lhs, const CLASSNAME< R >& rhs )                                                    { lhs.value ^= rhs.value; return lhs; }


#endif
