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
 * @file Event.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   04/10/2016
 */

#ifndef KRAKEN_EVENT_H
#define KRAKEN_EVENT_H

#include <Kraken/IO/IEPollable.h>
#include <errno.h>
#include <stdint.h>
#include <sys/eventfd.h>

namespace Kraken
{
    /**
     * Possible Event initialization flags.
     */
    enum class EEventFlags
    {
        None = 0,
        CloseOnExec = EFD_CLOEXEC,
        NonBlock = EFD_NONBLOCK,
        Semaphore = EFD_SEMAPHORE,
    };

    ENUM_FLAGS(EEventFlags);

    /**
     * A file-system based flag/semaphore object.
     *
     * @author  Gilad "Salmon" Naaman
     * @since   04/10/2016
     */
    class Event : public IEPollable
    {
    public:
        Event() :
                m_descriptor(-EBADFD)
        {}

        /**
         * Constructs an Event object from an existing, open file descriptor.
         *
         * @note There is no validation as for the type of the descriptor.
         *
         * @note This does not disable RAII, so if the File the file object goes out of scope,
         *          the kernel object will be closed.
         *
         * @param descriptor The file descriptor to use.
         */
        Event(fd_t descriptor) :
                m_descriptor(descriptor >= 0 ? descriptor : -EBADFD)
        {}

        virtual ~Event()
        {
            if (IsOpen())
            {
                Close();
            }
        }

        /**
         * Opens a new Event object.
         *
         * @note Will fail with `-EBUSY` if the Event is already open.
         *
         * @param initialValue  The initial value of the event counter.
         * @param flags         Flags modifying the created event.
         * @return `0` on success; `-errno` on error.
         */
        int Open(unsigned int initialValue, EEventFlags flags = EEventFlags::None);

        /**
         * Incremeant the event counter.
         *
         * @param value The value to incremeant by. Defaults to 1.
         * @return `0` on success; `-errno` on error.
         */
        int Post(uint64_t value = 1);

        /**
         * Waits for event activity.
         * If the internal counter is non-zero, this function returns immediately;
         * otherwise and if the object was set to nonblocking mode, it returns with `-EAGAIN`.
         * If the object is in blocking mode (the default), and the counter is 0, this function blocks.
         *
         * @return `0` on success; `-errno` on error.
         */
        inline int Wait()
        {
            uint64_t wastefulImplementationDetail;
            return Wait(wastefulImplementationDetail);
        }

        /**
         * Returns the value of the event counter.
         * The exact semantics of this method depend on the open flags.
         *
         * If the counter is positive, and the event was opened as:
         *  - Semaphore: The counter is decremented by 1, and 1 is returned on `o_counterValue`.
         *  - Regular (default): The counter is set to 0 and its pervious value is set in `o_counterValue`.
         * else, if the counter is zero and the event was opened as:
         *  - NonBlocking: This function will return with `-EAGAIN`
         *  - Blocking (default): This function will block until some other thread of execution posts the event.
         *
         * @param o_counterValue    The old value of the event counter.
         * @return `0` on success; `-errno` on error.
         */
        int Wait(uint64_t &o_counterValue);

        /**
         * Indicates whether the file contains a handle that appears to be valid.
         *
         * @note This check is superficial. There is a way to ask the OS if the given handle is valid,
         *          but there's no way to promise it actually points to the same object.
         *
         * @return `true` if the file contains a valid descriptor.
         */
        inline bool IsOpen() const
        {
            return (m_descriptor >= 0);
        }

        /**
         * Returns the underlying file-descriptor handle.
         */
        fd_t GetFileDescriptor() const override final
        {
            return m_descriptor;
        }

        /**
         * Closes this Event object.
         */
        void Close();

    private:
        Event(const Event &) = delete;

        fd_t m_descriptor;
    };
}

#endif //KRAKEN_EVENT_H
