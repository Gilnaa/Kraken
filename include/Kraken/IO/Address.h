/**
 * @file Address.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   01/10/2016
 */

#ifndef KRAKEN_ADDRESS_H
#define KRAKEN_ADDRESS_H

#include <Kraken/Slice.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

namespace Kraken
{
    enum class ESocketDomain
    {
        Unix = AF_UNIX,
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
    };

    /**
     * Reference implementation for an address struct.
     */
    template<ESocketDomain D>
    struct Address
    {
        /**
         * Denotes the maximum available address buffer size.
         */
        static constexpr socklen_t s_MaxSize = sizeof(Address<D>);

        /**
         * @return The size of the address buffer.
         */
        socklen_t GetLength() const;

        /**
         * Sets the actual length of the address.
         */
        void SetLength(socklen_t);

        /**
         * @return The base address of the sock-addr buffer.
         */
        sockaddr *GetBase();

        /**
         * @return The base address of the sock-addr buffer.
         */
        const sockaddr *GetBase() const;

        /**
         * @return `true` if the address is valid; `false` otherwise.
         */
        bool IsValid() const;
    };

    template<>
    struct Address<ESocketDomain::Unix>
    {
        /**
         * Denotes the maximum size of a UnixAddress.
         */
        static constexpr socklen_t s_MaxSize = sizeof(sockaddr_un);

        /**
         * The maximum length of the path portion of the address.
         */
        static constexpr socklen_t s_MaxPathLength = sizeof(sockaddr_un::sun_path);

        /**
         * The minimum address length [when len(path) == 0].
         */
        static constexpr socklen_t s_BaseStructSize = sizeof(sockaddr_un) - s_MaxPathLength;

        /**
         * Length of the relevant part of the address.
         */
        socklen_t length;

        /**
         * The actual address struct as the OS expects it.
         */
        struct sockaddr_un data;

        Address() : length(0)
        {
            memset(&data, 0, sizeof(data));
            data.sun_family = ~(sa_family_t)AF_UNIX; // Mark as invalid
        }

        Address(const char *path) : Address()
        {
            Init(path);
        }

        Address(const char *path, size_t length) : Address()
        {
            Init(path, length);
        }

        bool Init(const char *path)
        {
            if (path == nullptr)
            {
                // TODO: KRAKEN_PRINT
                return false;
            }
            else if (path[0] == '\0')
            {
                // TODO: KRAKEN_PRINT
                return false;
            }

            return Init(path, strnlen(path, s_MaxPathLength));
        }

        bool Init(const char *path, size_t pathLength)
        {
            data.sun_family = ~(sa_family_t)AF_UNIX; // Invalidate address.

            if (path == nullptr)
            {
                // TODO: KRAKEN_PRINT
                return false;
            }
            else if (pathLength >= s_MaxPathLength)
            {
                // TODO: KRAKEN_PRINT
                return false;
            }

            length = (socklen_t)(pathLength + s_BaseStructSize);
            memset(data.sun_path, 0, s_MaxPathLength);
            memcpy(data.sun_path, path, pathLength);
            data.sun_family = AF_UNIX;

            return true;
        }

        inline socklen_t GetLength() const
        {
            return sizeof(struct sockaddr_un);
        }

        inline void SetLength(socklen_t newLength)
        {
            // UnixAddress need to support variable-size (in a range, of course) in order to support
            // abstract addresses.

            if (newLength > sizeof(data))
            {
                length = sizeof(data);
            }
            else if (newLength < s_BaseStructSize)
            {
                length = s_BaseStructSize;
            }
            else
            {
                length = newLength;
            }
        }

        inline sockaddr *GetBase()
        {
            return (sockaddr *)&data;
        }

        inline const sockaddr *GetBase() const
        {
            return (sockaddr *)&data;
        }

        inline bool IsValid() const
        {
            return (data.sun_family == AF_UNIX);
        }

        inline const char *GetPath()
        {
            return data.sun_path;
        }
    };

    template<>
    struct Address<ESocketDomain::IPv4>
    {
        static constexpr socklen_t s_MaxSize = sizeof(sockaddr_in);
        struct sockaddr_in data;

        Address()
        {
            memset(&data, 0, sizeof(data));
            data.sin_family = ~(sa_family_t)AF_INET;
        }


        Address(const char *address, uint16_t port)
        {
            Init(address, port);
        }

        bool Init(const char *address, uint16_t port)
        {
            int err;

            data.sin_family = ~(sa_family_t)AF_INET; // Invalidate address.

            if (address == nullptr)
            {
                KRAKEN_PRINT("Null parameter: address");
                return false;
            }

            memset(&data, 0, sizeof(data));

            err = inet_pton(AF_INET, address, &data.sin_addr);
            if (err != 1)
            {
                KRAKEN_PRINT("Failed to parse address. errno = %ld", -errno);
                return false;
            }

            data.sin_port = htons(port);
            data.sin_family = AF_INET;
            return true;
        }

        inline socklen_t GetLength() const
        {
            return sizeof(data);
        }

        inline void SetLength(socklen_t newLength)
        {
            // Knowingly left empty.
        }

        inline sockaddr *GetBase()
        {
            return (sockaddr *)&data;
        }

        inline const sockaddr *GetBase() const
        {
            return (sockaddr *)&data;
        }

        inline bool IsValid() const
        {
            return data.sin_family == AF_INET;
        }
    };

    template<>
    struct Address<ESocketDomain::IPv6>
    {
        static constexpr socklen_t s_MaxSize = sizeof(sockaddr_in6);
        struct sockaddr_in6 data;

        Address()
        {
            memset(&data, 0, sizeof(data));
            data.sin6_family = ~(sa_family_t)AF_INET6;
        }


        Address(const char *address, uint16_t port)
        {
            Init(address, port);
        }

        bool Init(const char *address, uint16_t port)
        {
            int err;

            data.sin6_family = ~(sa_family_t)AF_INET6; // Invalidate address.

            if (address == nullptr)
            {
                KRAKEN_PRINT("Null parameter: address");
                return false;
            }

            memset(&data, 0, sizeof(data));

            err = inet_pton(AF_INET6, address, &data.sin6_addr);
            if (err != 1)
            {
                KRAKEN_PRINT("Failed to parse address. errno = %ld", -errno);
                return false;
            }

            data.sin6_port = htons(port);
            data.sin6_family = AF_INET6;
            return true;
        }

        inline socklen_t GetLength() const
        {
            return sizeof(data);
        }

        inline void SetLength(socklen_t newLength)
        {
            // Knowingly left empty.
        }

        inline sockaddr *GetBase()
        {
            return (sockaddr *)&data;
        }

        inline const sockaddr *GetBase() const
        {
            return (sockaddr *)&data;
        }

        inline bool IsValid() const
        {
            return data.sin6_family == AF_INET6;
        }
    };

    using UnixAddress = struct Address<ESocketDomain::Unix>;
    using IPv4Address = struct Address<ESocketDomain::IPv4>;
    using IPv6Address = struct Address<ESocketDomain::IPv6>;
}

#endif //KRAKEN_ADDRESS_H
