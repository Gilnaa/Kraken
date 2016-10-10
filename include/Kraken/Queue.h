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
 * @file Queue.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   10/10/2016
 */

#ifndef KRAKEN_QUEUE_H

#include <stdlib.h>

namespace Kraken
{
    /**
     * A FIFO data structure.
     *
     * @tparam T Queue item type
     * @tparam N Queue maximum capacity.
     */
    template <typename T, size_t N>
    class Queue
    {
    public:
        Queue() : m_counter(0),
                  m_nextFullSlot(0),
                  m_nextEmptySlot()
        {}

        /**
         * @return The amount of items in the queue.
         */
        inline size_t Count() const
        {
            return m_counter;
        }

        /**
         * @return The maximum amount of items in the queue.
         */
        inline size_t Capcity() const
        {
            return N;
        }

        /**
         * @return `true` if there are no items in the queue.
         */
        inline bool IsEmpty() const
        {
            return m_counter == 0;
        }

        /**
         * @return `true` if the queue is full.
         */
        inline bool IsFull() const
        {
            return (m_counter == N);
        }

        /**
         * Push an item to the queue.
         *
         * @param item  The item to enqueue.
         *
         * @return `true` if the item was enqueued; `false` if the queue is full.
         */
        bool Push(const T &item)
        {
            if (IsFull())
            {
                return false;
            }

            MetaSquid::copy(item, *reinterpret_cast<T *>(&m_data[m_nextEmptySlot]));

            m_nextEmptySlot = (++m_nextEmptySlot % N);
            m_counter++;

            return true;
        }

        /**
         * Pops an item from the queue into the given space.
         *
         * @param o_item    Output. After a successful call will contain the dequeued item.
         *
         * @return `true` if an item was dequeued; `false` if the queue was empty.
         */
        bool Pop(T &o_item)
        {
            if (IsEmpty())
            {
                return false;
            }

            MetaSquid::copy(*reinterpret_cast<T *>(&m_data[m_nextFullSlot]), o_item);
            m_nextFullSlot = (++m_nextFullSlot % N);
            m_counter--;

            return true;
        }

        /**
         * Peeks at the top of the queue.
         *
         * @param o_item     Output. After a successful call will contain the head of the queue.
         *
         * @return `true` if an item was copied from the queue; `false` if the queue was empty.
         */
        bool Peek(T &o_item)
        {
            if (IsEmpty())
            {
                return false;
            }

            MetaSquid::copy(*reinterpret_cast<T *>(&m_data[m_nextFullSlot]), o_item);

            return true;
        }

    private:
        size_t m_counter;
        size_t m_nextFullSlot;
        size_t m_nextEmptySlot;
        typename std::aligned_storage<sizeof(T)>::type m_data[N];
    };
}

#define KRAKEN_QUEUE_H

#endif //KRAKEN_QUEUE_H
