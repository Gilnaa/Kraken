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
 * @file event_tests.cpp
 *
 * @author  Gilad "Salmon" Naaman
 * @since   04/10/2016
 */

#include <gtest/gtest.h>
#include <Kraken/IO/Event.h>

using namespace Kraken;

TEST(EventTests, Initialization)
{
    Event e;

    ASSERT_FALSE(e.IsOpen());
    ASSERT_EQ(e.Open(1), 0);
    ASSERT_TRUE(e.IsOpen());

    ASSERT_EQ(e.Open(1), -EBUSY);
    e.Close();
    ASSERT_FALSE(e.IsOpen());
    ASSERT_EQ(e.Open(1), 0);
    e.Close();

    ASSERT_EQ(e.Open(1, EEventFlags::NonBlock), 0);
    e.Close();
    ASSERT_EQ(e.Open(1, EEventFlags::NonBlock | EEventFlags::Semaphore), 0);

}

TEST(EventTests, BasicLoop)
{
    Event e;
    ASSERT_EQ(e.Open(1, EEventFlags::NonBlock), 0);

    ASSERT_EQ(e.Wait(), 0); // Initial value is 1
    ASSERT_EQ(e.Wait(), -EAGAIN);
    ASSERT_EQ(e.Post(), 0);
    ASSERT_EQ(e.Wait(), 0);
}

TEST(EventTests, BasicLoop2)
{
    Event e;
    ASSERT_EQ(e.Open(1, EEventFlags::NonBlock), 0);

    ASSERT_EQ(e.Wait(), 0); // Initial value is 1
    ASSERT_EQ(e.Wait(), -EAGAIN);
    ASSERT_EQ(e.Post(30), 0);

    uint64_t x;
    ASSERT_EQ(e.Wait(x), 0);
    ASSERT_EQ(x, 30);
}

TEST(EventTests, Semaphore)
{
    Event e;
    ASSERT_EQ(e.Open(1, EEventFlags::NonBlock | EEventFlags::Semaphore), 0);

    ASSERT_EQ(e.Wait(), 0); // Initial value is 1
    ASSERT_EQ(e.Wait(), -EAGAIN);
    ASSERT_EQ(e.Post(30), 0);

    uint64_t x;
    for (size_t idx = 0; idx < 30; idx++)
    {
        ASSERT_EQ(e.Wait(x), 0);
        ASSERT_EQ(x, 1);
    }

    ASSERT_EQ(e.Wait(), -EAGAIN);
}