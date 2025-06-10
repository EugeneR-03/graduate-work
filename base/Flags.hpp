#pragma once

#include <type_traits>

namespace Flags {
    // для чисел
    template<typename T1, typename T2> requires std::is_integral_v<T1> && std::is_integral_v<T2>
    bool has_flag(T1 value, T2 flag) {
        T1 converted_flag = static_cast<T1>(flag);
        return (value & converted_flag) == converted_flag;
    }

    namespace EnumFlags {
        template <typename T>
        concept EnumFlag = std::is_enum_v<T>;

        template<EnumFlag T>
        constexpr T operator|(T lhs, T rhs) noexcept
        {
            using U = std::underlying_type_t<T>;
            return static_cast<T>(static_cast<U>(lhs) | static_cast<U>(rhs));
        }

        template<EnumFlag T>
        constexpr T operator&(T lhs, T rhs) noexcept
        {
            using U = std::underlying_type_t<T>;
            return static_cast<T>(static_cast<U>(lhs) & static_cast<U>(rhs));
        }

        template<EnumFlag T>
        constexpr T operator^(T lhs, T rhs) noexcept
        {
            using U = std::underlying_type_t<T>;
            return static_cast<T>(static_cast<U>(lhs) ^ static_cast<U>(rhs));
        }

        template<EnumFlag T>
        constexpr T operator~(T v) noexcept
        {
            using U = std::underlying_type_t<T>;
            return static_cast<T>(~static_cast<U>(v));
        }

        template<EnumFlag T>
        constexpr T& operator|=(T& lhs, T rhs) noexcept
        {
            return lhs = lhs | rhs;
        }

        template<EnumFlag T>
        constexpr T& operator&=(T& lhs, T rhs) noexcept
        {
            return lhs = lhs & rhs;
        }

        template<EnumFlag T>
        constexpr T& operator^=(T& lhs, T rhs) noexcept
        {
            return lhs = lhs ^ rhs;
        }

        template<EnumFlag T>
        constexpr bool operator!(T v) noexcept
        {
            using U = std::underlying_type_t<T>;
            return !static_cast<U>(v);
        }

        template<EnumFlag T>
        constexpr bool operator==(T lhs, T rhs) noexcept
        {
            using U = std::underlying_type_t<T>;
            return static_cast<U>(lhs) == static_cast<U>(rhs);
        }

        template<EnumFlag T>
        constexpr bool operator!=(T lhs, T rhs) noexcept
        {
            using U = std::underlying_type_t<T>;
            return static_cast<U>(lhs) != static_cast<U>(rhs);
        }

        template<EnumFlag T>
        constexpr bool has_flag(T value, T flag) noexcept
        {
            using U = std::underlying_type_t<T>;
            return (static_cast<U>(value) & static_cast<U>(flag)) == static_cast<U>(flag);
        }
    }
}