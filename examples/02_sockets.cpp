#include <Kraken/IO/Socket.h>
#include <iostream>
#include <Kraken/Collections.h>
#include <unistd.h>

using namespace std;
using namespace Kraken;

void types()
{
    static_assert(std::is_same<UnixSocket, Socket<ESocketDomain::Unix> >::value, "");
    static_assert(std::is_same<IPv4Socket, Socket<ESocketDomain::IPv4> >::value, "");
    static_assert(std::is_same<IPv6Socket, Socket<ESocketDomain::IPv6> >::value, "");

    static_assert(std::is_same<UnixAddress, Address<ESocketDomain::Unix> >::value, "");
    static_assert(std::is_same<IPv4Address, Address<ESocketDomain::IPv4> >::value, "");
    static_assert(std::is_same<IPv6Address, Address<ESocketDomain::IPv6> >::value, "");
}

int UDS()
{
    buffer<16> b;
    UnixSocket s, c;

    int err = s.Open(ESocketType::Datagram);
    if (err != 0)
    {
        cerr << "Failed to open socket. errno = " << err << endl;
        return EXIT_FAILURE;
    }

    err = c.Open(ESocketType::Datagram);
    if (err != 0)
    {
        cerr << "Failed to open socket. errno = " << err << endl;
        return EXIT_FAILURE;
    }

    // Bind a socket to an address.
    err = s.Bind(UnixAddress("socket_name.uds"));

    // sendto
    c.Send(b, UnixAddress("socket_name.uds"));
    c.Receive(b);

    // connect & send
    err = c.Connect(UnixAddress("socket_name.uds"));
    c.Send(b);

    unlink("socket_name.uds");
}

int main()
{
    return UDS();
}