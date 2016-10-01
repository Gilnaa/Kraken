/**
 * @file Collections.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   30/09/2016
 */

#ifndef KRAKEN_COLLECTIONS_H
#define KRAKEN_COLLECTIONS_H

#include <Kraken/Slice.h>
#include <Kraken/Definitions.h>

namespace Kraken
{
    /**
     * A generic array implementation.
     * Does not decay into a pointer.
     *
     * @tparam T    The type of the array's elements.
     * @tparam N    The number of array elements.
     */
    template<typename T, size_t N>
    struct array
    {
    private:
        /**
         * Internal data representation
         */
        T data[N];

    public:
        /**
         * Default constructor.
         */
        array() = default;

        /**
         * Initialize all of the arrays elements with a default value.
         * @param defaultValue
         */
        array(const T &defaultValue)
        {
            for (size_t index = 0; index < N; index++)
            {
                data[index] = defaultValue;
            }
        }

        /**
         * Copy the array from a language-array.
         * @param source The original data.
         */
        array(const T (&source)[N])
        {
            for (size_t index = 0; index < N; index++)
            {
                data[index] = source[index];
            }
        }

        /**
         * @return The amount of elements contained in the array.
         */
        constexpr size_t length() const { return N; }

        /**
         * @return The size of the memory area occupied by this array.
         */
        constexpr size_t byte_size() const { return sizeof(*this); }

        /**
         * Unsafe accessor to the underlying data's elements.
         *
         * @param index     The index of the wanter element.
         * @return A reference to the element at the specified index.
         */
        inline T &operator [](size_t index)
        {
            return data[index];
        }

        /**
         * Unsafe accessor to the underlying data's elements.
         *
         * @param index     The index of the wanter element.
         * @return A const-reference to the element at the specified index.
         */
        inline const T &operator [](size_t index) const
        {
            return data[index];
        }

        /**
         * Constructs a sub-range of the array.
         *
         * @param offset    The offset of the first element in the slice.
         * @param size      The amount of elements to include in the slice.
         *
         * @note This functions returns an invalid slice on error (null, 0).
         *
         * @return          A `slice` pointing to the raw memory for the specified elements.
         */
        slice range(unsigned long offset, unsigned long size)
        {
            if ((offset + size) > N)
            {
                KRAKEN_PRINT("Invalid range requested.");
                return slice(nullptr, 0);
            }

            return slice(&data[offset], sizeof(T) * (N - size));
        }

        /**
         * Constructs an immutable sub-range of the array.
         *
         * @param offset    The offset of the first element in the slice.
         * @param size      The amount of elements to include in the slice.
         *
         * @note This functions returns an invalid slice on error (null, 0).
         *
         * @return          A `const_slice` pointing to the raw memory for the specified elements.
         */
        const_slice range(unsigned long offset, unsigned long size) const
        {
            if ((offset + size) > N)
            {
                KRAKEN_PRINT("Invalid range requested.");
                return slice(nullptr, 0);
            }

            return const_slice(&data[offset], sizeof(T) * (N - size));
        }

        /**
         * Casts this array instance into a `slice`.
         */
        operator slice()
        {
            return slice(data);
        }

        /**
         * Casts this array instance into a `slice`.
         */
        operator const_slice() const
        {
            return const_slice(data);
        }
    };

    /**
     * A byte buffer.
     *
     * @tparam N The size of the buffer, in bytes.
     */
    template <size_t N>
    using buffer = array<unsigned char, N>;
}
#endif //KRAKEN_COLLECTIONS_H