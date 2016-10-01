/**
 * @file Definitions.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   30/09/2016
 */

#ifndef KRAKEN_DEFINITIONS_H
#define KRAKEN_DEFINITIONS_H

#include <type_traits>

namespace Kraken
{
    /**
     * The type of a file descriptor.
     */
    using fd_t = int;

    template<typename T>
    using is_enum_class = std::integral_constant<bool, std::is_enum<T>::value && !std::is_convertible<T, int>::value>;

    /**
     * Convert an enum class value into its underlying type.
     *
     * @tparam      An enum-class type.
     *
     * @param _e    The value to convert.
     * @return  The numerical equivalent of the passed value.
     */
    template <typename E>
    constexpr typename std::underlying_type<E>::type primitivize(E _e)
    {
        static_assert(is_enum_class<E>::value, "Value must be an enum class.");
        return (typename std::underlying_type<E>::type)_e;
    }
}

/**
 * Implement bitwise operators to enum classes.
 */
#define ENUM_FLAGS(_E) \
    static_assert(is_enum_class<_E>::value, "Value must be an enum class."); \
    constexpr _E operator | (_E a, _E b) { return (_E)(primitivize(a) | primitivize(b)); } \
    constexpr _E operator & (_E a, _E b) { return (_E)(primitivize(a) & primitivize(b)); } \
    constexpr _E operator ~ (_E a) { return (_E)~primitivize(a); }

/* Features */
#if defined(KRAKEN_OPT_PRINT_ON_ERROR)
# include<stdio.h>
# define KRAKEN_PRINT(msg, ...) fprintf(stderr, "[%s:%s:%u]" msg, __FILE__, __FUNCTION__, __LINE__,##__VA_ARGS__)
#else
# define KRAKEN_PRINT(msg, ...)
#endif /* KRAKEN_OPT_PRINT_ON_ERROR */

#endif //KRAKEN_DEFINITIONS_H
