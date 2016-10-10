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
 * @file MetaSquid.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   06/10/2016
 */

#ifndef KRAKEN_METASQUID_H
#define KRAKEN_METASQUID_H

#include <type_traits>
#include <stdlib.h>
#include <string.h>

namespace Kraken
{

/**
 * Contains various template-metaprogramming utilities.
 */
namespace MetaSquid
{
    /**
     * Checks if the given type is complete (i.e. not a forward declaration).
     *
     * @tparam T The type in question.
     */
    struct is_complete_helper
    {
        using no = char;
        using yes = char (&)[2];

        template <typename U, size_t = sizeof(U)>
        static yes check(U *);

        template <typename U>
        static no check(...);
    };

    template<class T>
    using is_complete = std::integral_constant<bool, sizeof(is_complete_helper::check<T>(nullptr)) == sizeof(is_complete_helper::yes)>;

    template <typename T>
    struct enable_if_complete
    {
        static_assert(MetaSquid::is_complete<T>::value, "T is no a complete type");
        typedef T type;
    };

    /**
     * @return The length of the given array (not its size).
     */
    template <typename T, size_t N>
    constexpr size_t lengthof(const T (&)[N])
    {
        return N;
    };

    template <typename T, size_t N>
    constexpr T &last(T (&array)[N])
    {
        return array[N - 1];
    };

    template <typename T, size_t N>
    constexpr const T &last(const T (&array)[N])
    {
        return array[N - 1];
    };

    /**
     * Determines whether the given type is an enum class.
     */
    template<typename T>
    using is_enum_class = std::integral_constant<bool, std::is_enum<T>::value && !std::is_convertible<T, int>::value>;

    template <class T>
    typename std::enable_if<std::is_copy_constructible<T>::value && !std::is_copy_assignable<T>::value>::type
    copy(const T &src, T &dst)
    {
        new(&dst) T(src);
    }

    template <class T>
    typename std::enable_if<std::is_copy_assignable<T>::value>::type
    copy(const T &src, T &dst)
    {
        dst = src;
    }

    template <class T>
    typename std::enable_if<std::is_pod<T>::value && !std::is_copy_assignable<T>::value && !std::is_copy_constructible<T>::value>::type
    copy(const T &src, T &dst)
    {
        memcpy(&dst, &src, sizeof(T));
    }
}
}

#endif //KRAKEN_METASQUID_H
