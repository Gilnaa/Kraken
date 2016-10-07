/**
 * @file squid_tests.cpp.cpp
 *
 * @author  Gilad "Salmon" Naaman
 * @since   06/10/2016
 */

#include <Kraken/MetaSquid.h>
#include <gtest/gtest.h>

using namespace Kraken::MetaSquid;
namespace squid = Kraken::MetaSquid;

TEST(MetaSquidTests, Arrays)
{
    int a[13];
    uint8_t b[72];

    a[12] = 71;
    b[71] = 12;

    ASSERT_EQ(lengthof(a), 13);
    ASSERT_EQ(lengthof(b), 72);

    ASSERT_EQ(last(a), 71);
    ASSERT_EQ(last(b), 12);
}

struct TestStruct1;
struct TestStruct2 {};
template <typename T>
struct TempTestStruct;
template <>
struct TempTestStruct<int> {};

TEST(MetaSquidTests, IsComplete)
{
    ASSERT_FALSE(squid::is_complete<TestStruct1>::value);
    ASSERT_TRUE(squid::is_complete<TestStruct2>::value);
    ASSERT_FALSE(squid::is_complete<TempTestStruct<void>>::value);
    ASSERT_TRUE(squid::is_complete<TempTestStruct<int>>::value);
}