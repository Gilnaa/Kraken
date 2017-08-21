/**
 * Copyright (c) 2016 Gilad Naaman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file Address.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   01/10/2016
 */

#ifndef KRAKEN_ADDRESS_H
#define KRAKEN_ADDRESS_H

#include <Kraken/membuf.h>
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

        /**
         * Tries to initialise this UnixAddress with a path.
         *
         * @note This call may fail; you can check with @ref IsValid
         * @note Do not use this constructor for abstract addresses.
         *
         * @param path The file-system path of the socket.
         */
        Address(const char *path) : Address()
        {
            Init(path);
        }

        /**
         * Tries to initialise this UnixAddress with a sized path.
         *
         * @note This call may fail; you can check with @ref IsValid
         *
         * @param path      The file-system path of the socket.
         * @param length    The length of the path.
         */
        Address(const char *path, size_t length) : Address()
        {
            Init(path, length);
        }

        /**
         * Initialises this UnixAddress instance.
         *
         * @note This overload does not support abstract addresses.
         *
         * @param path The path to use.
         * @return `true` on success; `false` on invalid input.
         */
        bool Init(const char *path)
        {
            if (path == nullptr)
            {
                return false;
            }
            else if (path[0] == '\0')
            {
                return false;
            }

            return Init(path, strnlen(path, s_MaxPathLength));
        }

        /**
         * Initialises this UnixAddress instance with a path
         *
         * @param path          The path to use.
         * @param pathLength    The length of the path.
         * @return `true` on success; `false` on invalid input.
         */
        bool Init(const char *path, size_t pathLength)
        {
            data.sun_family = ~(sa_family_t)AF_UNIX; // Invalidate address.

            if (path == nullptr)
            {
                return false;
            }
            else if (pathLength >= s_MaxPathLength)
            {
                return false;
            }

            length = (socklen_t)(pathLength + s_BaseStructSize);
            memset(data.sun_path, 0, s_MaxPathLength);
            memcpy(data.sun_path, path, pathLength);
            data.sun_family = AF_UNIX;

            return true;
        }

        /**
         * @return The current length of the address.
         */
        inline socklen_t GetLength() const
        {
            return length;
        }

        /**
         * Sets the length of the address.
         * Used mainly for abstract addresses which are not null-terminated.
         *
         * @param newLength The new length of the address.
         */
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

        /**
         * @return A pointer to the start of the address buffer.
         */
        inline sockaddr *GetBase()
        {
            return (sockaddr *)&data;
        }

        /**
         * @return A const pointer to the start of the address buffer.
         */
        inline const sockaddr *GetBase() const
        {
            return (sockaddr *)&data;
        }

        /**
         * @return `true` if the address is valid and initialised; `false` otherwise.
         */
        inline bool IsValid() const
        {
            return (data.sun_family == AF_UNIX);
        }

        /**
         * @return `true` if the path is valid and abstract; `false` otherwise.
         */
        inline bool IsAbstract()
        {
            return IsValid() && (data.sun_path[0] == '\0');
        }

        /**
         * @note This function returns unmeaningful result when using an abstract address.
         * @return The path of the socket on the file-system.
         */
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
