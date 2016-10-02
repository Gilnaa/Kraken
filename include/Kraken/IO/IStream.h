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
 * @file IStream.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   30/09/2016
 */

#ifndef KRAKEN_ISTREAM_H
#define KRAKEN_ISTREAM_H

#include <stdlib.h>
#include "Kraken/membuf.h"

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
        inline ssize_t Read(membuf mem)
        {
            return Read(mem.buffer, mem.length);
        }

        /**
         * Write the given buffer to the stream.
         *
         * @param mem   The memory membuf to write.
         *
         * @note The function may return a valid-value that is smaller the size of the membuf.
         *
         * @return The amount of bytes written on success; `-errno` otherwise.
         */
        inline ssize_t Write(const_membuf mem)
        {
            return Write(mem.buffer, mem.length);
        }
    };
}

#endif //KRAKEN_ISTREAM_H
