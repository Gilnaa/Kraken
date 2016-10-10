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
 * @file membuf.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   30/09/2016
 */

#ifndef KRAKEN_COLLECTIONS_H_H
#define KRAKEN_COLLECTIONS_H_H

#include <Kraken/MetaSquid.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/uio.h>

namespace Kraken
{
    /**
     * A forward declaration for a template-based membuf conversion opearation.
     *
     * The following functions are expected to be implemented:
     *  - static void *addr(T &);
     *  - static const void *addr(const T &);
     *  - static size_t size(const T &);
     *
     * @tparam T    The type to convert from.
     */
    template<typename T>
    struct membuf_adapter;

    /**
     * A wrapper around a sized, untyped, mutable memory buffer.
     */
    struct membuf
    {
        /**
         * The pointer to the raw memory.
         */
        void *const buffer;

        /**
         * The size of the memory section.
         */
        const size_t length;

        /**
         * Construct a new membuf from the given values.
         *
         * @param buffer The base-address of the buffer.
         * @param length The length of the buffer.
         */
        membuf (void *buffer, size_t length) : buffer(buffer), length(length) {}

        /**
         * Redeclared copy constructor.
         *
         * @param s     The original membuf.
         */
        membuf(const membuf &s) : buffer(s.buffer), length(s.length) {}

        /**
         * Construct a new membuf from an object with an adapter.
         *
         * @param o     An adaptable object.
         * @tparam T    The type of the object. Must have membuf_adapter<T> implemented for.
         */
        template <typename T>
        membuf(T &o) : buffer(MetaSquid::enable_if_complete<membuf_adapter<T>>::type::addr(o)),
                       length(MetaSquid::enable_if_complete<membuf_adapter<T>>::type::size(o)) { }

        /**
         * Construct a new membuf from an array.
         * @param array The array to turn into a buffer.
         *
         * @tparam T    The type of the object. Must be a POD type.
         */
        template <typename T, size_t N>
        membuf(T (&array)[N]) : buffer(&array), length(sizeof(array))
        {
            static_assert(std::is_pod<T>::value, "T must be POD type.");
        }

        /**
         * Deleted to avoid construction from immutable objects.
         */
        template <typename T> membuf(const T&) = delete;

        /**
         * Deleted to avoid construction from immutable arrays.
         */
        template <typename T, size_t N> membuf(const T (&array)[N]) = delete;

        /**
         * Indicates whether the underlying pointer referes to a valid memory address.
         *
         * @return true if the buffer is non-null and has a positive length; false otherwise.
         */
        inline bool is_valid() const
        {
            return (buffer != nullptr) && (length > 0);
        }
    };

    /**
     * A wrapper around a sized, untyped, immutable memory buffer.
     */
    struct const_membuf
    {
        /**
         * An immutable pointer to the raw memory section.
         */
        const void *const buffer;

        /**
         * The length of the memory section, in bytes.
         */
        const size_t length;

        /**
         * Construct a new const_membuf from an existing mutable membuf.
         *
         * @param s The original buffer wrapper.
         */
        const_membuf(membuf s) : buffer(s.buffer), length(s.length) {}

        /**
         * Construct a new const_membuf from an existing immutable membuf.
         *
         * @param s The original buffer wrapper.
         */
        const_membuf(const const_membuf &s) : buffer(s.buffer), length(s.length) {}

        /**
         * Construct a new const_membuf from the given values.
         *
         * @param buffer The base-address of the buffer.
         * @param length The length of the buffer.
         */
        const_membuf(const void *buffer, size_t length) : buffer(buffer), length(length) {}

        /**
         * Construct a new const_membuf from an object with an adapter.
         *
         * @param o     An adaptable object.
         * @tparam T    The type of the object. Must have membuf_adapter<T> implemented for.
         */
        template <typename T>
        const_membuf(const T &o) : buffer(MetaSquid::enable_if_complete<membuf_adapter<T>>::type::addr(o)),
                                   length(MetaSquid::enable_if_complete<membuf_adapter<T>>::type::size(o)) { }

        /**
         * Construct a new const_membuf from an array.
         * @param array The array to turn into a buffer.
         *
         * @tparam T    The type of the object. Must be a POD type.
         */
        template <typename T, size_t N>
        const_membuf(const T (&array)[N]) : buffer(&array), length(sizeof(array))
        {
            static_assert(std::is_pod<T>::value, "T must be POD type.");
        }

        /**
         * Indicates whether the underlying pointer referes to a valid memory address.
         *
         * @return true if the buffer is non-null and has a positive length; false otherwise.
         */
        inline bool is_valid() const
        {
            return (buffer != nullptr) && (length > 0);
        }
    };

    /**
     * template-gettho used to contain the conversion between a membuf and an iovec.
     *
     * Usually they're defined the same way, but AFAIK it is not promised to be so.
     * In case they are interchangable, there is no actual conversion (membuf[] = iovec[]),
     * else, a new iovec[] is created and the pointers are copied.
     */
    namespace details
    {
        using membuf_is_iovec = std::integral_constant<bool, (sizeof(membuf) == sizeof(iovec)) &&
                                                             (offsetof(membuf, buffer) == offsetof(iovec, iov_base)) &&
                                                             (offsetof(membuf, length) == offsetof(iovec, iov_len))>;

        template <bool S>
        struct membuf_iovec_provider;

        template <>
        struct membuf_iovec_provider<true>
        {
            template <size_t N>
            using iovec_type = struct iovec *;

            template <size_t N>
            static inline void copy(membuf (&src)[N], iovec_type<N> &dest)
            {
                dest = (iovec_type<N>)&src;
            }

            template <size_t N>
            static inline void copy(const_membuf (&src)[N], iovec_type<N> &dest)
            {
                dest = (iovec_type<N>)&src;
            }
        };

        template <>
        struct membuf_iovec_provider<false>
        {
            template <size_t N>
            using iovec_type = struct iovec [N];

            template <size_t N>
            static void copy(membuf (&src)[N], iovec_type<N> &dest)
            {
                for (size_t i = 0; i < N; i++)
                {
                    dest[i].iov_base = src[i].buffer;
                    dest[i].iov_len = src[i].length;
                }
            }

            template <size_t N>
            static void copy(const_membuf (&src)[N], iovec_type<N> &dest)
            {
                for (size_t i = 0; i < N; i++)
                {
                    dest[i].iov_base = const_cast<void *>(src[i].buffer);
                    dest[i].iov_len = src[i].length;
                }
            }
        };

        using membuf_iovec_converter = membuf_iovec_provider<membuf_is_iovec::value>;
    }
}

#endif //KRAKEN_COLLECTIONS_H_H
