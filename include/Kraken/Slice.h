/**
 * @file Slice.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   30/09/2016
 */

#ifndef KRAKEN_COLLECTIONS_H_H
#define KRAKEN_COLLECTIONS_H_H

#include <stdlib.h>     // For size_t
#include <type_traits>  // For false_type, true_type, is_pod

namespace Kraken
{

    /**
     * A forward declaration for a template-based slice conversion opearation.
     *
     * @tparam T    The type to convert from.
     *
     * @note This struct is denoted final to avoid inherit the `unspecialised_slice_adapter` to derived structs.
     */
    template<typename T>
    struct slice_adapter final
    {
        /**
         * Used to tell if a type has a slice_adapter implemented.
         */
        typedef std::false_type unspecialised_slice_adapter;

        /**
         * When implemented on specialised templates, returns the base address of the given object.
         * @return A pointer to the raw memory of the object.
         */
        static void *addr(T &);

        /**
         * When implemented on specialised templates, returns the base address of the given object.
         * @return A const pointer to the raw memory of the object.
         */
        static const void *addr(const T &);

        /**
         * When implemented on specialised templates, returns the size of the object's raw memory.
         * @return The size, in bytes, of the object's memory.
         */
        static size_t size(const T &);
    };

    /**
     * details-gettho used to contain templates.
     */
    namespace details
    {
        template <bool COND, typename TRUE_TYPE, typename FALSE_TYPE = void>
        struct meta_if {
            typedef TRUE_TYPE type;
        };

        template <typename TRUE_TYPE, typename FALSE_TYPE>
        struct meta_if<false, TRUE_TYPE, FALSE_TYPE> {
            typedef FALSE_TYPE type;
        };

        template <typename T, typename _ = void>
        struct has_slice_adapter : std::false_type {};

        template <typename T>
        struct has_slice_adapter<T, typename meta_if<false, typename slice_adapter<T>::unspecialised_slice_adapter>::type> : std::true_type {};
    }

    /**
     * A wrapper around a sized, untyped, mutable memory buffer.
     */
    struct slice
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
         * Construct a new slice from the given values.
         *
         * @param buffer The base-address of the buffer.
         * @param length The length of the buffer.
         */
        slice (void *buffer, size_t length) : buffer(buffer), length(length) {}

        /**
         * Redeclared copy constructor.
         *
         * @param s     The original slice.
         */
        slice(const slice &s) : buffer(s.buffer), length(s.length) {}

        /**
         * Construct a new slice from an object with an adapter.
         *
         * @param o     An adaptable object.
         * @tparam T    The type of the object. Must have slice_adapter<T> implemented for.
         */
        template <typename T>
        slice(T &o) : buffer(slice_adapter<T>::addr(o)),
                      length(slice_adapter<T>::size(o))
        {
            static_assert(!details::has_slice_adapter<T>::value, "Type does not have a slice_adapter");
        }

        /**
         * Construct a new slice from an array.
         * @param array The array to turn into a buffer.
         *
         * @tparam T    The type of the object. Must be a POD type.
         */
        template <typename T, size_t N>
        slice(T (&array)[N]) : buffer(&array), length(sizeof(array))
        {
            static_assert(std::is_pod<T>::value, "T must be POD type.");
        }

        /**
         * Deleted to avoid construction from immutable objects.
         */
        template <typename T> slice(const T&) = delete;

        /**
         * Deleted to avoid construction from immutable arrays.
         */
        template <typename T, size_t N> slice(const T (&array)[N]) = delete;

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
    struct const_slice
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
         * Construct a new const_slice from an existing mutable slice.
         *
         * @param s The original buffer wrapper.
         */
        const_slice(slice s) : buffer(s.buffer), length(s.length) {}

        /**
         * Construct a new const_slice from an existing immutable slice.
         *
         * @param s The original buffer wrapper.
         */
        const_slice(const const_slice &s) : buffer(s.buffer), length(s.length) {}

        /**
         * Construct a new const_slice from the given values.
         *
         * @param buffer The base-address of the buffer.
         * @param length The length of the buffer.
         */
        const_slice(const void *buffer, size_t length) : buffer(buffer), length(length) {}

        /**
         * Construct a new const_slice from an object with an adapter.
         *
         * @param o     An adaptable object.
         * @tparam T    The type of the object. Must have slice_adapter<T> implemented for.
         */
        template <typename T>
        const_slice(const T &o) : buffer(slice_adapter<T>::addr(o)),
                                  length(slice_adapter<T>::size(o))
        {
            static_assert(!details::has_slice_adapter<T>::value, "Type does not have a slice_adapter");
        }

        /**
         * Construct a new const_slice from an array.
         * @param array The array to turn into a buffer.
         *
         * @tparam T    The type of the object. Must be a POD type.
         */
        template <typename T, size_t N>
        const_slice(const T (&array)[N]) : buffer(&array), length(sizeof(array))
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
}

#endif //KRAKEN_COLLECTIONS_H_H
