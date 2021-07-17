#pragma once

#include <type_traits>

template<typename Enum>
struct EnableBitMaskOperators
{
    static const bool s_Enable = false;
};

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::s_Enable, Enum>::type
operator |(Enum lhs_, Enum rhs_)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (
        static_cast<underlying>(lhs_) |
        static_cast<underlying>(rhs_)
        );
}

#define ENABLE_BITMASK_OPERATORS(x)    \
template<>                             \
struct EnableBitMaskOperators<x>       \
{                                      \
    static const bool s_Enable = true; \
};
