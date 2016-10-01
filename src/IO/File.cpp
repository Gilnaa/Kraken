/**
 * @file File.cpp
 *
 * @author  Gilad "Salmon" Naaman
 * @since   30/09/2016
 */

#include "Kraken/IO/File.h"
#include <unistd.h>
#include <sys/ioctl.h>

using namespace Kraken;

int File::Open(const char *path, EFileFlags flags, EFileModes mode)
{
    int descriptor;

    if (path == nullptr)
    {
        KRAKEN_PRINT("Null parameter: path");
        return -EINVAL;
    }
    else if (IsOpen())
    {
        KRAKEN_PRINT("Object already contains a valid descriptor.");
        return -EBUSY;
    }

    descriptor = open(path, (int)flags, (int)mode);
    if (descriptor < 0)
    {
        return -errno;
    }

    m_descriptor = descriptor;
    return 0;
}

ssize_t File::Read(void *o_buffer, size_t length)
{
    ssize_t res = 0;

    if (o_buffer == nullptr)
    {
        KRAKEN_PRINT("Null parameter (`o_buffer`).");
        return -EINVAL;
    }

    res = read(m_descriptor, o_buffer, length);
    if (res < 0)
    {
        res = -errno;
        KRAKEN_PRINT("Failed to read from file. `o_buffer` = %p, `length` = %lu, errno = %ld", o_buffer, length, res);
    }

    return res;
}

ssize_t File::Write(const void *buffer, size_t length)
{
    ssize_t res = 0;

    if (buffer == nullptr)
    {
        KRAKEN_PRINT("Null parameter (`buffer`).");
        return -EINVAL;
    }

    res = write(m_descriptor, buffer, length);
    if (res < 0)
    {
        res = -errno;
        KRAKEN_PRINT("Failed to write to file. `buffer` = %p, `length` = %lu, errno = %ld", buffer, length, res);
    }

    return res;
}

ssize_t File::Read(void *o_buffer, size_t length, size_t offset)
{
    ssize_t res = 0;

    if (o_buffer == nullptr)
    {
        KRAKEN_PRINT("Null parameter (`o_buffer`).");
        return -EINVAL;
    }

    res = pread(m_descriptor, o_buffer, length, offset);
    if (res < 0)
    {
        res = -errno;
        KRAKEN_PRINT("Failed to read from file. `o_buffer` = %p, `length` = %lu, errno = %ld", o_buffer, length, res);
    }

    return res;
}

ssize_t File::Write(const void *buffer, size_t length, size_t offset)
{
    ssize_t res = 0;

    if (buffer == nullptr)
    {
        KRAKEN_PRINT("Null parameter (`buffer`).");
        return -EINVAL;
    }

    res = pwrite(m_descriptor, buffer, length, offset);
    if (res < 0)
    {
        res = -errno;
        KRAKEN_PRINT("Failed to write to file. `buffer` = %p, `length` = %lu, errno = %ld", buffer, length, res);
    }

    return res;
}

int File::IOControl(unsigned long command, void *parameter)
{
    int res = 0;

    res = ioctl(m_descriptor, command, parameter);
    if (res < 0)
    {
        KRAKEN_PRINT("Command failed. errno = %d", res);
        res = -errno;
    }

    return res;
}

void File::Close()
{
    // There's nothing we can do really for close failure.
    close(m_descriptor);
    m_descriptor = -EBADFD;
}
