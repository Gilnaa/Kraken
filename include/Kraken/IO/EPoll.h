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
 * @file EPoll.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   01/10/2016
 */

#ifndef KRAKEN_EPOLL_H
#define KRAKEN_EPOLL_H

#include <Kraken/IO/IEPollable.h>
#include <Kraken/Collections.h>
#include <sys/epoll.h>
#include <errno.h>
namespace Kraken
{
    /**
     * A class used to hold and poll several Kraken objects.
     *
     * @tparam T    The most common base class of the objects. (Optional)
     */
    template<typename T=IEPollable>
    class EPoll : public IEPollable
    {
    public:
        /**
         * Constructs a new EPoll object.
         */
        EPoll() : m_descriptor(-EBADFD)
        {
        }

        /**
         * Tries to open a new EPoll object.
         *
         * @return `0` on success; `-errno` on error.
         */
        int Open()
        {
            int descriptor = epoll_create1(0);
            if (descriptor < 0)
            {
                return -errno;
            }

            m_descriptor = descriptor;

            return 0;
        }

        /**
         * Adds the given interface to the epoll's watch list.
         *
         * @param epollable An epollable interface.
         * @return `0` on success; `-errno` on error.
         */
        int AddWatch(T &epollable)
        {
            int err;
            epoll_event evt;
            evt.data.ptr = &epollable;
            evt.events = EPOLLIN;

            err = epoll_ctl(m_descriptor, EPOLL_CTL_ADD, epollable.GetFileDescriptor(), &evt);
            if (err != 0)
            {
                return -errno;
            }

            return 0;
        }

        /**
         * Removes the given epollable from epoll's watch list.
         *
         * @param epollable The interface to remove from the list.
         * @return `0` on success; `-errno` on error.
         */
        int RemoveWatch(T &epollable)
        {
            int err = epoll_ctl(m_descriptor, EPOLL_CTL_DEL, epollable.GetFileDescriptor(), nullptr);
            if (err != 0)
            {
                return -errno;
            }

            return 0;
        }

        /**
         * Polls the watch list for any new events.
         *
         * @tparam N    The maximum amount of events to poll for.
         *
         * @param o_events  Will be filled with the ready events on return.
         * @param timeout   The wait timeout in milliseconds. `-1` to wait indefinitly, `0` for non-block operation.
         * @return The number of ready interfaces on success; `-errno` on error.
         */
        template <size_t N>
        int Wait(T *(&o_events)[N], int timeout = -1)
        {
            static_assert(N > 0, "N must be positive.");

            epoll_event events[N];
            int readyObjects;

            readyObjects = epoll_wait(m_descriptor, events, N, timeout);
            if (readyObjects < 0)
            {
                return -errno;
            }

            for (size_t index = 0; index < readyObjects; index++)
            {
                o_events[index] = (T *)events[index].data.ptr;
            }

            return readyObjects;
        }

        /**
         * Polls the watch list for any new events.
         *
         * @tparam N    The maximum amount of events to poll for.
         *
         * @param o_events  Will be filled with the ready events on return.
         * @param timeout   The wait timeout in milliseconds. `-1` to wait indefinitly, `0` for non-block operation.
         * @return The number of ready interfaces on success; `-errno` on error.
         */
        template <size_t N>
        int Wait(array<T *, N> o_events, int timeout = -1)
        {
            static_assert(N > 0, "N must be positive.");

            return Wait((typename array<T *, N>::native_array_ref)o_events, timeout);
        }

        /**
         * @return The object's underlying file descriptor.
         */
        fd_t GetFileDescriptor() const override
        {
            return m_descriptor;
        }

    protected:
        /**
         * Kernel handle for the epoll object.
         */
        fd_t m_descriptor;

    private:
        EPoll(const EPoll &) = delete;
    };
}

#endif //KRAKEN_EPOLL_H
