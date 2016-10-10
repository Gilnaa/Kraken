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
 * @file Timer.cpp
 *
 * @author  Gilad "Salmon" Naaman
 * @since   10/10/2016
 */

#include <Kraken/IO/Timer.h>
#include <unistd.h>

using namespace Kraken;

int Timer::Init(ETimerFlags flags, EClockType clock)
{
    m_descriptor = timerfd_create((int)clock, (int)flags);

    if (m_descriptor < 0)
    {
        return -errno;
    }
    return 0;
}

int Timer::Arm(uint64_t firstExpiration, uint64_t interval, EResolution res)
{
    const uint64_t nanoConversion = (uint64_t)EResolution::NanoSeconds / (uint64_t)res;
    int timeError = 0;

    struct itimerspec timeout;
    timeout.it_value.tv_sec = (time_t)(firstExpiration / (uint64_t)res);
    timeout.it_value.tv_nsec = (long)(firstExpiration % (uint64_t)res) / nanoConversion;

    timeout.it_interval.tv_sec = (time_t)(interval / (uint64_t)res);
    timeout.it_interval.tv_nsec = (long)(interval % (uint64_t)res) / nanoConversion;

    timeError = timerfd_settime(m_descriptor, 0, &timeout, nullptr);

    if (timeError == -1)
    {
        return -errno;
    }
    return 0;
}

int Timer::GetNextExpiration(uint64_t &o_nextExpiration, EResolution res)
{
    int timeError = 0;
    struct itimerspec timeout;

    timeError = timerfd_gettime(m_descriptor, &timeout);

    if (timeError == 0)
    {
        o_nextExpiration = timeout.it_value.tv_sec * (uint64_t) res;
        o_nextExpiration += (timeout.it_value.tv_nsec * (uint64_t)res) /
                            (uint64_t)EResolution::NanoSeconds;

        return 0;
    }

    return -errno;
}

int Timer::ReadExpirations(uint64_t &o_expirations)
{
    ssize_t timeError = read(m_descriptor, &o_expirations, sizeof(o_expirations));

    if (timeError < 0)
    {
        return (int)timeError;
    }
    else if (timeError != sizeof(uint64_t))
    {
        return -EIO;
    }

    return 0;
}

int Timer::Clear()
{
    uint64_t temp;
    return ReadExpirations(temp);
}

void Timer::Close()
{
    if (close(m_descriptor) == 0)
    {
        m_descriptor = -EBADFD;
    }
}
