/**
 * @file IStream.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   30/09/2016
 */

#ifndef KRAKEN_ISTREAM_H
#define KRAKEN_ISTREAM_H

#include <stdlib.h>
#include "Kraken/Slice.h"

namespace Kraken
{
    /**
     * A minimal interface for byte-streams.
     *
     * @author  Gilad "Salmon" Naaman
     * @since   30/09/2016
     */
    class IStream
    {
    public:
        /**
         * Read up to `length` bytes from the stream into `o_buffer`.
         *
         * @param o_buffer  The buffer to fill with data.
         * @param length    The size of the buffer, in bytes.
         *
         * @note The function may return a valid-value that is smaller than `length`.
         *
         * @return The amount of bytes read on success; `-errno` otherwise.
         */
        virtual ssize_t Read(void *o_buffer, size_t length) = 0;

        /**
         * Write the given buffer to the stream.
         *
         * @param buffer    The buffer containing the data to write.
         * @param length    The size of the buffer, in bytes.
         *
         * @note The function may return a valid-value that is smaller than `length`.
         *
         * @return The amount of bytes written on success; `-errno` otherwise.
         */
        virtual ssize_t Write(const void *buffer, size_t length) = 0;

        /**
         * Read up to `length` bytes from the stream into `o_buffer`.
         *
         * @param mem   The buffer to fill with data.
         *
         * @note The function may return a valid-value that is smaller than `length`.
         *
         * @return The amount of bytes read on success; `-errno` otherwise.
         */
        inline ssize_t Read(slice mem)
        {
            return Read(mem.buffer, mem.length);
        }

        /**
         * Write the given buffer to the stream.
         *
         * @param mem   The memory slice to write.
         *
         * @note The function may return a valid-value that is smaller the size of the slice.
         *
         * @return The amount of bytes written on success; `-errno` otherwise.
         */
        inline ssize_t Write(const_slice mem)
        {
            return Write(mem.buffer, mem.length);
        }
    };
}

#endif //KRAKEN_ISTREAM_H
