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
 * @file IEPollable.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   01/10/2016
 */

#ifndef KRAKEN_IEPOLLABLE_H
#define KRAKEN_IEPOLLABLE_H

#include <Kraken/Definitions.h>

namespace Kraken
{
    /**
     * Defines the minimal requirements for an interface to be (e)pollable.
     */
    class IEPollable
    {
    public:
        /**
         * @return The object's underlying file descriptor.
         */
        virtual fd_t GetFileDescriptor() const = 0;
    };
}

#endif //KRAKEN_IEPOLLABLE_H
