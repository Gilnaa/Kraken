/**
 * @file file_tests.cpp
 *
 * @author  Gilad "Salmon" Naaman
 * @since   01/10/2016
 */

#include <Kraken/IO/File.h>
#include <gtest/gtest.h>

using namespace Kraken;

TEST(FileTests, Open)
{
    File testFile;

    ASSERT_FALSE(testFile.IsOpen());
}