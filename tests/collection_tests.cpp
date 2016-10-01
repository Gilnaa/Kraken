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