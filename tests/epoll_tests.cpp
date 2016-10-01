/**
 * @file epoll_tests.cpp
 *
 * @author  Gilad "Salmon" Naaman
 * @since   01/10/2016
 */

#include <gtest/gtest.h>
#include <Kraken/IO/EPoll.h>
#include <Kraken/IO/File.h>
#include <Kraken/IO/Socket.h>

using namespace Kraken;

static constexpr auto s_TestingUnixAddress = "address.uds";

TEST(EPollTests, CompilationTest)
{
    EPoll<> e1;
    EPoll<File> e2;
    EPoll<UnixSocket> e3;
}

TEST(EPollTests, Basic)
{
    uint8_t testBuffer[16];
    EPoll<> epoll;
    UnixSocket s[2];
    IEPollable *rdyIfcs[1] = { nullptr };

    ASSERT_EQ(s[0].Open(ESocketType::Datagram), 0);
    ASSERT_EQ(s[1].Open(ESocketType::Datagram), 0);

    ASSERT_EQ(s[0].Bind(UnixAddress(s_TestingUnixAddress)), 0);
    ASSERT_EQ(s[1].Connect(UnixAddress(s_TestingUnixAddress)), 0);

    ASSERT_EQ(epoll.Open(), 0);
    ASSERT_EQ(epoll.AddWatch(s[0]), 0);

    // With no timeout: return immediately with 0 ready interfaces.
    ASSERT_EQ(epoll.Wait(rdyIfcs, 0), 0);
    ASSERT_EQ(s[1].Send(testBuffer), 16);
    ASSERT_EQ(epoll.Wait(rdyIfcs, 0), 1);
    ASSERT_EQ(epoll.Wait(rdyIfcs, 0), 1);
    ASSERT_EQ(s[0].Receive(testBuffer), 16);
    ASSERT_EQ(epoll.Wait(rdyIfcs, 0), 0);

    unlink(s_TestingUnixAddress);
}