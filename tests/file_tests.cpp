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

TEST(FileTests, UMask)
{
    mode_t originalValue = umask(0);
    EXPECT_EQ(umask(originalValue), 0); // Why the hell not.

    {
        ScopedUMask mask(EFileModes::OthersAll);
        ASSERT_EQ(mask.previousMask, originalValue);
    }

    mode_t expectedOriginal = umask(0);
    ASSERT_EQ(expectedOriginal, originalValue);
    umask(originalValue);
}

TEST(FileTests, Vectors)
{
    File read, write;
    buffer<16> a(0);
    buffer<16> b(2);
    buffer<32> c;
    const_membuf vec[] = {a, b};

    ASSERT_EQ(File::Pipe(read, write), 0);

    ASSERT_EQ(write.Write(vec), 32);
    ASSERT_EQ(read.Read(c), 32);

    ASSERT_EQ(memcmp(a, c, sizeof(a)), 0);
    ASSERT_EQ(memcmp(b, &c[16], sizeof(a)), 0);
}

TEST(FileTests, OffsetIO)
{
    File temp(fileno(tmpfile()));
    uint8_t bigSample[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    uint8_t buf[sizeof(bigSample) / 2];
    const_membuf expected(&bigSample[5], sizeof(bigSample) / 2);

    uint8_t initialValue[128] = { 0 };
    memset(initialValue, 0, sizeof(initialValue));

    temp.Write(initialValue);
    temp.WriteAt(bigSample, 64);
    temp.ReadAt(buf, 64 + sizeof(bigSample) / 2);

    ASSERT_EQ(memcmp(buf, expected.buffer, expected.length), 0);
}