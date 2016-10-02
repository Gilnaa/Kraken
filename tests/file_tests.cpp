/**
 * @file file_tests.cpp
 *
 * @author  Gilad "Salmon" Naaman
 * @since   01/10/2016
 */

#include <gtest/gtest.h>
#include <Kraken/IO/File.h>
#include <Kraken/Collections.h>

using namespace Kraken;

TEST(FileTests, Open)
{
    File testFile;

    ASSERT_FALSE(testFile.IsOpen());
}

TEST(FileTests, Pipe)
{
    buffer<16> in(0);
    buffer<16> out;
    File a;
    File b;

    ASSERT_EQ(File::Pipe(a, b), 0);
    ASSERT_EQ(b.Write(in), sizeof(in));
    ASSERT_EQ(a.Read(out), sizeof(in));

    ASSERT_EQ(File::Pipe(a, b), -EBUSY);
}
