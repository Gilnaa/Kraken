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
 * @file Stack.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   10/10/2016
 */

#ifndef KRAKEN_STACK_H
#define KRAKEN_STACK_H

#include <stdlib.h>

namespace Kraken
{
    /**
     * A FILO data structure.
     *
     * @tparam T Stack item type
     * @tparam N Stack maximum capacity.
     */
    template <typename T, size_t N>
    class Stack
    {
    public:
        Stack() : m_cursor(0),
                  m_data()
        {}

        /**
         * @return The amount of items in the stack.
         */
        inline size_t Count() const
        {
            return m_cursor;
        }

        /**
         * @return The maximum amount of items on stack.
         */
        inline size_t Capcity() const
        {
            return N;
        }

        /**
         * @return `true` if there are no items on the stack.
         */
        inline bool IsEmpty() const
        {
            return m_cursor == 0;
        }

        /**
         * @return `true` if the stack is full.
         */
        inline bool IsFull() const
        {
            return (m_cursor == N);
        }

        /**
         * Push an item on the stack.
         *
         * @param item  The item to push on the stack.
         *
         * @return `true` if the item was pushed on the stack; `false` if the stack is full.
         */
        bool Push(const T &item)
        {
            if (IsFull())
            {
                return false;
            }

            MetaSquid::copy(item, *reinterpret_cast<T *>(&m_data[m_cursor++]));

            return true;
        }

        /**
         * Pops an item from the stack into the given space.
         *
         * @param o_item    Output. After a successful call will contain the popped item.
         *
         * @return `true` if an item was popped from the stack; `false` if the stack was empty.
         */
        bool Pop(T &o_item)
        {
            if (IsEmpty())
            {
                return false;
            }

            MetaSquid::copy(*reinterpret_cast<T *>(&m_data[--m_cursor]), o_item);

            return true;
        }

        /**
         * Peeks at the top of the stack.
         *
         * @param o_item     Output. After a successful call will contain the top of the stack.
         *
         * @return `true` if an item was copied from the stack; `false` if the stack was empty.
         */
        bool Peek(T &o_item) const
        {
            if (IsEmpty())
            {
                return false;
            }

            MetaSquid::copy(*reinterpret_cast<T *>(&m_data[m_cursor - 1]), o_item);

            return true;
        }

    private:
        size_t m_cursor;
        typename std::aligned_storage<sizeof(T)>::type m_data[N];
    };
}

#endif //KRAKEN_STACK_H
