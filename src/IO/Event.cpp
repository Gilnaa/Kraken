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
 * @file Event.cpp
 *
 * @author  Gilad "Salmon" Naaman
 * @since   04/10/2016
 */

#include <Kraken/IO/Event.h>
#include <unistd.h>

using namespace Kraken;

int Event::Open(unsigned int initialValue, Kraken::EEventFlags flags)
{
    int descriptor;

    if (IsOpen())
    {
        return -EBUSY;
    }

    descriptor = eventfd(initialValue, (int)flags);
    if (descriptor < 0)
    {
        return -errno;
    }

    m_descriptor = descriptor;

    return 0;
}

int Event::Post(uint64_t value)
{
    ssize_t res = write(m_descriptor, &value, sizeof(value));
    if (res < 0)
    {
        return -errno;
    }

    return 0;
}

int Event::Wait(uint64_t &o_counterValue)
{
    ssize_t res = read(m_descriptor, &o_counterValue, sizeof(o_counterValue));
    if (res < 0)
    {
        return -errno;
    }

    return 0;
}

void Kraken::Event::Close()
{
    if (close(m_descriptor) == 0)
    {
        m_descriptor = -EBADFD;
    }
}
