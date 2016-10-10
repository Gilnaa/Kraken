/**
 * @file socket_tests.cpp
 *
 * @author  Gilad "Salmon" Naaman
 * @since   01/10/2016
 */

#include <Kraken/IO/Socket.h>
#include <gtest/gtest.h>

using namespace Kraken;

static constexpr auto TestSocketAddress = "kraken_sock_test1.uds\0";
static constexpr auto TestSocketAddress_Extra = "kraken_sock_test2.uds\0";

class SocketTest : public ::testing::Test
{
    void TearDown() override
    {
        unlink(TestSocketAddress);
        unlink(TestSocketAddress_Extra);
    }
};

TEST_F(SocketTest, TemplateCompilation)
{
    UnixAddress unixAddress;
    IPv4Address ipv4Address;
    IPv6Address ipv6Address;

    UnixSocket unixSocket;
    IPv4Socket ipv4Socket;
    IPv6Socket ipv6Socket;
}

TEST_F(SocketTest, Init)
{
    UnixSocket s;

    ASSERT_FALSE(s.IsOpen());
    ASSERT_EQ(s.Open(ESocketType::Datagram), 0);
    ASSERT_EQ(s.Open(ESocketType::Datagram), -EBUSY);

    s.Close();
    ASSERT_EQ(s.Open(ESocketType::SeqPacket), 0);
}

TEST_F(SocketTest, BindConnect)
{
    UnixSocket sender, receiver;

    EXPECT_EQ(sender.Open(ESocketType::Datagram), 0);
    EXPECT_EQ(receiver.Open(ESocketType::Datagram), 0);

    ASSERT_LT(sender.Connect(UnixAddress(TestSocketAddress)), 0); // Bind point does not yet exist
    ASSERT_EQ(receiver.Bind(UnixAddress(TestSocketAddress)), 0);
    ASSERT_EQ(sender.Connect(UnixAddress(TestSocketAddress)), 0);
}

TEST_F(SocketTest, SendReceive)
{
    const uint8_t inputBuffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    uint8_t outputBuffer[8] = {0};
    UnixSocket sender, receiver;

    EXPECT_EQ(sender.Open(ESocketType::Datagram), 0);
    EXPECT_EQ(receiver.Open(ESocketType::Datagram), 0);

    EXPECT_EQ(receiver.Bind(UnixAddress(TestSocketAddress)), 0);
    EXPECT_EQ(sender.Connect(UnixAddress(TestSocketAddress)), 0);

    ASSERT_EQ(sender.Send(inputBuffer), sizeof(inputBuffer));
    ASSERT_EQ(receiver.Receive(outputBuffer), sizeof(inputBuffer));

    ASSERT_EQ(memcmp(inputBuffer, outputBuffer, sizeof(inputBuffer)), 0);
}

TEST_F(SocketTest, SendToReceiveFrom)
{
    const uint8_t inputBuffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    uint8_t outputBuffer[8] = {0};
    UnixSocket sender, receiver;
    UnixAddress senderAddress;

    EXPECT_EQ(sender.Open(ESocketType::Datagram), 0);
    EXPECT_EQ(receiver.Open(ESocketType::Datagram), 0);

    EXPECT_EQ(receiver.Bind(UnixAddress(TestSocketAddress)), 0);
    EXPECT_EQ(sender.Bind(UnixAddress(TestSocketAddress_Extra)), 0);

    ASSERT_EQ(sender.Send(inputBuffer, UnixAddress(TestSocketAddress)),
              sizeof(inputBuffer));

    ASSERT_EQ(receiver.Receive(outputBuffer, senderAddress),
              sizeof(inputBuffer));

    EXPECT_EQ(memcmp(inputBuffer, outputBuffer, sizeof(inputBuffer)), 0);
    ASSERT_EQ(strcmp(senderAddress.data.sun_path, TestSocketAddress_Extra), 0);
}

TEST_F(SocketTest, SeqPacket)
{
    UnixSocket server, client, remoteClient;

    EXPECT_EQ(server.Open(ESocketType::SeqPacket), 0);
    EXPECT_EQ(remoteClient.Open(ESocketType::SeqPacket), 0);

    EXPECT_EQ(server.Bind(UnixAddress(TestSocketAddress)), 0);
    ASSERT_EQ(server.Listen(1), 0);

    EXPECT_EQ(remoteClient.Connect(UnixAddress(TestSocketAddress)), 0);
    ASSERT_EQ(server.Accept(client), 0);
}

TEST_F(SocketTest, IPv6)
{
    IPv6Socket s, c;
    uint8_t buffer0[1024] = { 0 };
    uint8_t buffer1[1024];


    ASSERT_EQ(s.Open(ESocketType::Datagram), 0);
    ASSERT_EQ(c.Open(ESocketType::Datagram), 0);

    ASSERT_EQ(s.Bind(IPv6Address("::1", 0x6668)), 0);

    ASSERT_EQ(c.Send(buffer0, IPv6Address("::1", 0x6668)), sizeof(buffer0));
    ASSERT_EQ(s.Receive(buffer1), sizeof(buffer1));

    ASSERT_EQ(memcmp(buffer0, buffer1, sizeof(buffer0)), 0);
}

TEST_F(SocketTest, AcceptFrom)
{
    IPv4Socket server, client, remoteClient;
    const IPv4Address expectedAddress("127.0.0.1", 0x6667);
    IPv4Address remoteAddress;

    ASSERT_EQ(server.Open(ESocketType::Stream), 0);
    ASSERT_EQ(remoteClient.Open(ESocketType::Stream), 0);

    ASSERT_EQ(server.Bind(IPv4Address ("127.0.0.1", 0x6666)), 0);
    ASSERT_EQ(server.Listen(1), 0);
    ASSERT_EQ(remoteClient.Bind(IPv4Address ("127.0.0.1", 0x6667)), 0);

    ASSERT_EQ(remoteClient.Connect(IPv4Address ("127.0.0.1", 0x6666)), 0);
    ASSERT_EQ(server.Accept(client, remoteAddress), 0);

    ASSERT_EQ(memcmp(&expectedAddress, &remoteAddress, sizeof(remoteAddress)), 0);

    remoteClient.Shutdown();
    client.Shutdown();
    server.Shutdown();
}

TEST_F(SocketTest, Pair)
{
    UnixSocket a, b;
    uint8_t buffer0[1024] = { 0 };
    uint8_t buffer1[1024];

    ASSERT_EQ(0, UnixSocket::Pair(ESocketType::Datagram, a, b));
    ASSERT_EQ(a.Send(buffer0), sizeof(buffer0));
    ASSERT_EQ(b.Receive(buffer1), sizeof(buffer1));

    ASSERT_EQ(-EBUSY, UnixSocket::Pair(ESocketType::Datagram, a, b));
}