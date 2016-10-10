/**
 * @file test_sample.cpp
 *
 * @author  Gilad "Salmon" Naaman
 * @since   30/09/2016
 */
#include <Kraken/Collections.h>
#include <gtest/gtest.h>
#include <Kraken/IO/File.h>

using namespace Kraken;

TEST(CollectionTests, Arrays_Init)
{
    array<int, 32> arr = {0};

    ASSERT_EQ(arr.length(), 32);
    ASSERT_EQ(arr.byte_size(), 32 * sizeof(int));

    for (size_t index = 0; index < arr.length(); index++)
        ASSERT_EQ(arr[index], 0);

    array<int, 5> arr2({0, 1, 2, 3, 4});
    for (size_t index = 0; index < arr2.length(); index++)
        ASSERT_EQ(arr2[index], index);

    int actualArray[12];
    array<int, 12> arr3 = actualArray;
    for (size_t index = 0; index < arr3.length(); index++)
        ASSERT_EQ(arr3[index], actualArray[index]);
}

TEST(CollectionTests, Stack)
{
    Stack<int, 5> s;
    int popped = 0;

    ASSERT_TRUE(s.IsEmpty());
    ASSERT_FALSE(s.IsFull());
    ASSERT_EQ(s.Capcity(), 5);
    ASSERT_EQ(s.Count(), 0);

    ASSERT_TRUE(s.Push(0));
    ASSERT_FALSE(s.IsEmpty());

    ASSERT_TRUE(s.Push(1));
    ASSERT_TRUE(s.Push(2));
    ASSERT_TRUE(s.Push(3));
    ASSERT_TRUE(s.Push(4));

    ASSERT_FALSE(s.Push(120)); // No capcity for 6 items.
    ASSERT_TRUE(s.IsFull());

    ASSERT_TRUE(s.Pop(popped));
    ASSERT_EQ(popped, 4);

    ASSERT_TRUE(s.Pop(popped));
    ASSERT_EQ(popped, 3);

    ASSERT_TRUE(s.Pop(popped));
    ASSERT_EQ(popped, 2);

    ASSERT_TRUE(s.Pop(popped));
    ASSERT_EQ(popped, 1);

    ASSERT_TRUE(s.Pop(popped));
    ASSERT_EQ(popped, 0);

    ASSERT_FALSE(s.Pop(popped)); // No more items.
}

TEST(CollectionTests, Queue)
{
    Queue<int, 5> q;
    int popped;

    ASSERT_TRUE(q.IsEmpty());
    ASSERT_FALSE(q.IsFull());
    ASSERT_EQ(q.Capcity(), 5);
    ASSERT_EQ(q.Count(), 0);

    ASSERT_TRUE(q.Push(0));
    ASSERT_FALSE(q.IsEmpty());
    ASSERT_EQ(q.Count(), 1);

    ASSERT_TRUE(q.Push(1));
    ASSERT_TRUE(q.Push(2));
    ASSERT_TRUE(q.Push(3));
    ASSERT_TRUE(q.Push(4));
    ASSERT_FALSE(q.Push(120));

    ASSERT_TRUE(q.Pop(popped));
    ASSERT_EQ(popped, 0);

    ASSERT_TRUE(q.Pop(popped));
    ASSERT_EQ(popped, 1);

    ASSERT_TRUE(q.Pop(popped));
    ASSERT_EQ(popped, 2);

    ASSERT_TRUE(q.Pop(popped));
    ASSERT_EQ(popped, 3);

    ASSERT_TRUE(q.Pop(popped));
    ASSERT_EQ(popped, 4);

    ASSERT_FALSE(q.Pop(popped));
}

TEST(CollectionTests, QueueWrap)
{
    Queue<int, 5> q;
    int popped;

    q.Push(1);
    q.Push(2);
    q.Push(3);
    q.Pop(popped);
    q.Pop(popped);
    ASSERT_EQ(q.Count(), 1);

    ASSERT_TRUE(q.Push(4));
    ASSERT_TRUE(q.Push(5));
    ASSERT_TRUE(q.Push(6));
    ASSERT_TRUE(q.Push(7));
    ASSERT_EQ(q.Count(), 5);

    ASSERT_FALSE(q.Push(17));
}


TEST(CollectionTests, ArrayQueue)
{
    using X = int[13];
    Queue<X, 5> q0;
    X a, b, c, d, e;
    ASSERT_TRUE(q0.Push(a));
    ASSERT_TRUE(q0.Push(b));
    ASSERT_TRUE(q0.Push(c));
    ASSERT_TRUE(q0.Push(d));
    ASSERT_TRUE(q0.Push(e));
}