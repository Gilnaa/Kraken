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
 * @file Timer.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   10/10/2016
 */

#ifndef KRAKEN_TIMER_H
#define KRAKEN_TIMER_H

#include <Kraken/IO/IEPollable.h>
#include <errno.h>
#include <stdint.h>
#include <sys/timerfd.h>

namespace Kraken
{
    /**
     * Defines the possible clock resolutions.
     */
    enum class EResolution
    {
        Seconds         = 1,
        MilliSeconds    = 1000,
        MicroSeconds    = 1000000,
        NanoSeconds     = 1000000000
    };

    /**
     * Defines the available clock types.
     */
    enum class EClockType
    {
        Realtime = CLOCK_REALTIME,
        Monotonic = CLOCK_MONOTONIC
    };

    /**
     * Defines the set of flags possible to use when opening a timer.
     */
    enum class ETimerFlags
    {
        None = 0,
        NonBlock= TFD_NONBLOCK,
        CloseOnExec = TFD_CLOEXEC,
    };

    class Timer : public IEPollable
    {
    public:
        Timer() :
                m_descriptor(-EBADFD)
        {}

        Timer(fd_t descriptor) :
                m_descriptor(descriptor >= 0 ? descriptor : -EBADFD)
        {}

        /**
         * Initializes this timer instance.
         *
         * @param flags Initiaization flags.
         * @param clock The type of the timer's clock.
         *
         * @return  `0` on succes; `-errno` otherwise.
         */
        int Init(ETimerFlags flags=ETimerFlags::None, EClockType clock=EClockType::Monotonic);

        /**
         * Starts the timer.
         *
         * @param firstExpiration   The amount of time between the arming of the timer and its first expiration.
         * @param interval          The amount of time between each expiration.
         * @param res               The resolution of the two previous parameters.
         *
         * @note Setting `firstExpiration` to `0` will disarm the timer.
         * @note Setting `interval` to `0` will cause the timer to expire only once. It can always be re-armed.
         * @note Accuracy of NanoSecond resolution depends on the implementation of the underlying OS.
         *
         * @return `0` on success; `-errno` on error.
         */
        int Arm(uint64_t firstExpiration, uint64_t interval, EResolution res=EResolution::MilliSeconds);

        /**
         * Stops the timer.
         *
         * @return `0` on success; `-errno` on error.
         */
        inline int Disarm()
        {
            return Arm(0, 0);
        }

        /**
         * Returns an approximation of the timer left until the next expiration.
         *
         * @param o_nextExpiration  After a successful call, will contain the amount of time left until the next expiration.
         * @param res               The requested resolution of the previous parameter.
         *
         * @return `0` on success; `-errno` on error.
         */
        int GetNextExpiration(uint64_t &o_nextExpiration, EResolution res=EResolution::MilliSeconds);

        /**
         * Returns the amount of expirations that have occurd since the last successful read operation,
         * or since the arming of the timer.
         *
         * @param o_expirations After a successful call, will contain the amount of expirations since the last check.
         *
         * @return `0` on success; `-errno` on error.
         */
        int ReadExpirations(uint64_t &o_expirations);

        /**
         * Clears the expiration counter.
         *
         * @return `0` on success; `-errno` on error.
         */
        int Clear();

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
        Timer (const Timer &) = delete;

        fd_t m_descriptor;
    };
}

#endif //KRAKEN_TIMER_H
