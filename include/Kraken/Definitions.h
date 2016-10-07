/**
 * Copyright (c) 2016 Gilad Naaman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file Definitions.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   30/09/2016
 */

#ifndef KRAKEN_DEFINITIONS_H
#define KRAKEN_DEFINITIONS_H

#include <Kraken/MetaSquid.h>

namespace Kraken
{
    /**
     * The type of a file descriptor.
     */
    using fd_t = int;

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
        static_assert(MetaSquid::is_enum_class<E>::value, "Value must be an enum class.");
        return (typename std::underlying_type<E>::type)_e;
    }
}

/**
 * Implement bitwise operators to enum classes.
 */
#define ENUM_FLAGS(_E) \
    static_assert(MetaSquid::is_enum_class<_E>::value, "Value must be an enum class."); \
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
