/**
 * @file Socket.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   01/10/2016
 */

#ifndef KRAKEN_SOCKET_H
#define KRAKEN_SOCKET_H

#include <Kraken/IO/File.h>
#include <Kraken/IO/Address.h>

namespace Kraken
{
    /**
     * The set of possible socket communication types.
     */
    enum class ESocketType
    {
        Datagram = SOCK_DGRAM,
        SeqPacket = SOCK_SEQPACKET,
        Stream = SOCK_STREAM,
    };

    enum class ESendFlags
    {
        None = 0,
        Confirm = MSG_CONFIRM,
        DoNotRoute = MSG_DONTROUTE,
        DoNotWait = MSG_DONTWAIT,
        EndOfRecord = MSG_EOR,
        More = MSG_MORE,
        NoSignal = MSG_NOSIGNAL,
        OutOfBand = MSG_OOB,

        // Aliases
        NonBlock = DoNotWait,
    };

    enum class EReceiveFlags
    {
        None = 0,
        DoNotWait = MSG_DONTWAIT,
        MSG_ERRQUEUE,
        MSG_OOB,
        MSG_PEEK,
        MSG_TRUNC,
        MSG_WAITALL,

        // Aliases
        NonBlock = DoNotWait
    };

    /**
     * A templated socket wrapper.
     *
     * @tparam D    The domain of the socket.
     *
     * @see @ref Address<ESocketDomain::Unix>
     * @see @ref Address<ESocketDomain::IPv4>
     * @see @ref Address<ESocketDomain::IPv6>
     */
    template <ESocketDomain D>
    class Socket : public File
    {
    public:
        using addr_t = Address<D>;

        virtual ~Socket()
        {
            if (IsOpen())
            {
                Shutdown();
            }
        }

        void Shutdown()
        {
            shutdown(m_descriptor, SHUT_RDWR);
            Close();
        }

        /**
         * Creates a new socket object.
         *
         * @param type  The communication protocol type.
         * @return `0` on success; `-errno` otherwise.
         */
        int Open(ESocketType type)
        {
            int descriptor;

            if (IsOpen())
            {
                // TODO: KRAKEN_PRINT
                return -EBUSY;
            }

            descriptor = socket((int)D, (int)type, 0);
            if (descriptor < 0)
            {
                return -errno;
            }

            m_descriptor = descriptor;

            return 0;
        }

        /**
         * Binds the socket to the given address.
         *
         * @param localAddress  The local address to bind to.
         * @return `0` on success; `-errno` on error.
         */
        int Bind(const addr_t &localAddress)
        {
            int err = 0;

            if (!localAddress.IsValid())
            {
                KRAKEN_PRINT("Invalid address passed to bind.");
                return -EINVAL;
            }

            err = bind(m_descriptor, localAddress.GetBase(), localAddress.GetLength());
            if (err != 0)
            {
                KRAKEN_PRINT("Bind error. errno = %d", res);
                return -errno;
            }

            return 0;
        }

        /**
         * Turns this socket into a passive server, used to accept incoming connections.
         *
         * @param backlog   The size of the client waiting queue.
         * @return  `0` on success; `-errno` on error.
         */
        int Listen(int backlog)
        {
            int err = listen(m_descriptor, backlog);
            if (err < 0)
            {
                err = -errno;
            }

            return err;
        }

        /**
         * Connect to a remote address.
         *
         * @note Exact semantics depend on the @ref ESocketType of the socket.
         *
         * @param remoteAddress The address to connect to.
         * @return  `0` on success, `-errno` on error.
         */
        int Connect(const addr_t &remoteAddress)
        {
            int err = 0;

            err = connect(m_descriptor, remoteAddress.GetBase(), remoteAddress.GetLength());

            if (err != 0)
            {
                // TODO: KRAKEN_PRINT
                return -errno;
            }

            return 0;
        }

        /**
         * Accepts an incoming connection.
         *
         * @note The o_client object must *not* contain a valid descriptor. In such case, `-EBUSY` will be returned.
         *
         * @param o_client A Socket object to be initialized as the client.
         * @return `0` on success; `-errno` on error.
         */
        int Accept(Socket &o_client)
        {
            addr_t wastefulImplementationDetail;
            return Accept(o_client, wastefulImplementationDetail);
        }

        /**
         * Accepts an incoming connection.
         *
         * @note The o_client object must *not* contain a valid descriptor. In such case, `-EBUSY` will be returned.
         *
         * @param o_client          A Socket object to be initialized as the client.
         * @param o_clientAddress   An address object to be filled by the connection source's address.
         * @return `0` on success; `-errno` on error.
         */
        int Accept(Socket &o_client, addr_t &o_clientAddress)
        {
            int descriptor;
            socklen_t addressLength = addr_t::s_MaxSize;

            if (o_client.IsOpen())
            {
                // TODO: KRAKEN_PRINT
                return -EBUSY;
            }

            descriptor = accept(m_descriptor, o_clientAddress.GetBase(), &addressLength);
            if (descriptor < 0)
            {
                return -errno;
            }

            o_clientAddress.SetLength(addressLength);
            o_client.m_descriptor = descriptor;

            return 0;
        }

        /**
         * Sends a buffer through the socket.
         *
         * @note The socket must be connected for this function to work.
         *
         * @param buffer    The buffer to send.
         * @param length    The length of the buffer.
         * @param flags     Send flags
         * @return On success, this function returns the amount of bytes sent; on error `-errno`.
         */
        ssize_t Send(const void *buffer, size_t length, ESendFlags flags = ESendFlags::None)
        {
            ssize_t bytesSent;

            if (buffer == nullptr)
            {
                // TODO: KRAKEN_PRINT
                return -EINVAL;
            }

            bytesSent = send(m_descriptor, buffer, length, (int)flags);
            if (bytesSent < 0)
            {
                bytesSent = -errno;
            }

            return bytesSent;
        }

        /**
         * Sends a buffer through the socket to a specified address.
         *
         * @param buffer        The buffer to send.
         * @param length        The length of the buffer.
         * @param destination   The destination to send to.
         * @param flags         Send flags
         * @return On success, this function returns the amount of bytes sent; on error `-errno`.
         */
        ssize_t Send(const void *buffer, size_t length, const addr_t &destination, ESendFlags flags = ESendFlags::None)
        {
            ssize_t bytesSent;

            if (buffer == nullptr)
            {
                // TODO: KRAKEN_PRINT
                return -EINVAL;
            }

            bytesSent = sendto(m_descriptor, buffer, length, (int)flags, destination.GetBase(), destination.GetLength());
            if (bytesSent < 0)
            {
                bytesSent = -errno;
            }

            return bytesSent;
        }

        /**
         * Receives data from the socket into the given buffer.
         *
         * @param o_buffer  The buffer to fill with the received data.
         * @param length    The length of the buffer.
         * @param flags     Receive flag.
         * @return On success, this function returns the amount of bytes received; on error `-errno`.
         */
        ssize_t Receive(void *o_buffer, size_t length, EReceiveFlags flags = EReceiveFlags::None)
        {
            ssize_t bytesReceived;

            if (o_buffer == nullptr)
            {
                // TODO: KRAKEN_PRINT
                return -EINVAL;
            }

            bytesReceived = recv(m_descriptor, o_buffer, length, (int)flags);
            if (bytesReceived < 0)
            {
                // TODO: KRAKEN_PRINT
                bytesReceived = -errno;
            }

            return bytesReceived;
        }

        /**
         * Receives data from the socket into the given buffer, along with the sender's address (when possible).
         *
         * @param o_buffer          The buffer to fill with the received data.
         * @param length            The length of the buffer.
         * @param o_senderAddress   A buffer to be filled with the sender's address.
         * @param flags             Receive flag.
         * @return On success, this function returns the amount of bytes received; on error `-errno`.
         */
        ssize_t Receive(void *o_buffer, size_t length, addr_t &o_senderAddress, EReceiveFlags flags = EReceiveFlags::None)
        {
            socklen_t addressLength = addr_t::s_MaxSize;
            ssize_t bytesReceived;

            if (o_buffer == nullptr)
            {
                // TODO: KRAKEN_PRINT
                return -EINVAL;
            }

            bytesReceived = recvfrom(m_descriptor, o_buffer, length, (int)flags, o_senderAddress.GetBase(), &addressLength);
            if (bytesReceived < 0)
            {
                // TODO: KRAKEN_PRINT
                return -errno;
            }

            o_senderAddress.SetLength(addressLength);
            return bytesReceived;
        }

        /**
         * Sends a memory slice through the socket.
         *
         * @note The socket must be connect in order for this function to work.
         *
         * @param mem       The buffer to send.
         * @param flags     Send flags.
         * @return On success, this function returns the amount of bytes sent; on error `-errno`.
         */
        inline ssize_t Send(const_slice mem, ESendFlags flags = ESendFlags::None)
        {
            return Send(mem.buffer, mem.length, flags);
        }

        /**
         * Sends a memory slice through the socket to the specified destination.
         *
         * @param mem           The buffer to send.
         * @param flags         Send flags.
         * @param destination   The address of the remote endpoint.
         * @return On success, this function returns the amount of bytes sent; on error `-errno`.
         */
        inline ssize_t Send(const_slice mem, const addr_t &destination, ESendFlags flags = ESendFlags::None)
        {
            return Send(mem.buffer, mem.length, destination, flags);
        }

        /**
         * Receives data from the socket into the given buffer.
         *
         * @param o_mem     The buffer to fill with the received data.
         * @param flags     Receive flags.
         * @return On success, this function returns the amount of bytes received; on error `-errno`.
         */
        inline ssize_t Receive(slice o_mem, EReceiveFlags flags = EReceiveFlags::None)
        {
            return Receive(o_mem.buffer, o_mem.length, flags);
        }

        /**
         * Receives data from the socket into the given buffer, along with the sender's address (when possible).
         *
         * @param o_mem             The buffer to fill with the received data.
         * @param o_senderAddress   A buffer to be filled with the sender's address.
         * @param flags             Receive flags.
         * @return On success, this function returns the amount of bytes received; on error `-errno`.
         */
        inline ssize_t Receive(slice o_mem, addr_t &o_senderAddress, EReceiveFlags flags = EReceiveFlags::None)
        {
            return Receive(o_mem.buffer, o_mem.length, o_senderAddress, flags);
        }
    };

    using UnixSocket = Socket<ESocketDomain::Unix>;
    using IPv4Socket = Socket<ESocketDomain::IPv4>;
    using IPv6Socket = Socket<ESocketDomain::IPv6>;
}

#endif //KRAKEN_SOCKET_H
