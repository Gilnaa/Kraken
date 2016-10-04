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
 * @file File.h
 *
 * @author  Gilad "Salmon" Naaman
 * @since   30/09/2016
 */

#ifndef KRAKEN_FILE_H
#define KRAKEN_FILE_H

#include <errno.h>
#include <fcntl.h>
#include <Kraken/Definitions.h>
#include <Kraken/IO/IEPollable.h>
#include <Kraken/IO/IStream.h>

namespace Kraken
{
    /**
     * The set of file open flags.
     */
    enum class EFileFlags
    {
        None = 0,

        Read = O_RDONLY,
        Write = O_WRONLY,
        ReadWrite = O_RDWR,
        Append = O_APPEND,
        Async = O_ASYNC,
        CloseOnExec = O_CLOEXEC,
        Create = O_CREAT,
        Direct = O_DIRECT,
        Directory = O_DIRECTORY,
        DataSync = O_DSYNC,
        ExpectCreation = O_EXCL,
        LargeFile = O_LARGEFILE,
        NoAccessTimes = O_NOATIME,
        NoControlingTerminal = O_NOCTTY,
        NoFollowLinks = O_NOFOLLOW,
        NonBlock = O_NONBLOCK,
        Path = O_PATH,
        Sync = O_SYNC,
        TempFile = O_TMPFILE,
        Truncate = O_TRUNC,
    };

    /**
     * The set of available file creation modes.
     */
    enum class EFileModes
    {
        /**
         * User (file owner) has read, write, and execute permission
         */
        UserAll = S_IRWXU,
        /**
         * User has read permission
         */
        UserRead = S_IRUSR,
        /**
         * User has write permission
         */
        UserWrite = S_IWUSR,
        /**
         * User has execute permission
         */
        UserExec = S_IXUSR,


        /**
         * Group has read, write, and execute permission
         */
        GroupAll = S_IRWXG,
        /**
         * Group has read permission
         */
        GroupRead = S_IRGRP,
        /**
         * Group has write permission
         */
        GroupWrite = S_IWGRP,
        /**
         * Group has execute permission
         */
        GroupExec = S_IXGRP,


        /**
         * Others have read, write, and execute permission
         */
        OthersAll = S_IRWXO,
        /**
         * Others have read permission
         */
        OthersRead = S_IROTH,
        /**
         * Others have write permission
         */
        OthersWrite = S_IWOTH,
        /**
         * Others have execute permission
         */
        OthersExec = S_IXOTH,


        /**
         * Executing this file will set the effective UID of the process to that of the file's owner.
         * @note Linux-specific.
         */
        SetUserID = S_ISUID, //0004000 set-user-ID bit

        /**
         * Executing this file will set the effective GID of the process to that of the file's group.
         * @note Linux-specific.
         */
        SetGroupdID = S_ISGID, //0002000 set-group-ID bit (see stat(2))

        /**
         * When the sticky bit is set on a directory, files in that directory may only be unlinked or renamed by root or the directory owner or the file owner
         * @note Linux-specific.
         */
        StickyBit = S_ISVTX, //0001000 sticky bit (see stat(2))

        /**
         * The default file permissions.
         */
        Default = UserRead | UserWrite | GroupRead | OthersRead
    };

    ENUM_FLAGS(EFileFlags);
    ENUM_FLAGS(EFileModes);

    /**
     * A basic POSIX file wrapper.
     *
     * @author  Gilad "Salmon" Naaman
     * @since   30/09/2016
     */
    class File : public IStream,
                 public IEPollable
    {
    public:
        using IStream::Read;
        using IStream::Write;

        /**
         * Construct a default, non-open instance.
         */
        File() : m_descriptor(-EBADFD) { }

        /**
         * Construct an instance around an open-file descriptor.
         *
         * @note This does not disable RAII, so if the File the file object goes out of scope,
         *          the kernel object will be closed.
         *
         * @param descriptor The file descriptor to use.
         */
        File(int descriptor) : m_descriptor(descriptor < 0 ? -EBADFD : descriptor) {}

        virtual ~File()
        {
            if (IsOpen())
            {
                Close();
            }
        }

        /**
         * Open a file for IO operations.
         *
         * @note This function fails with `-EBUSY` when this object contains a valid descriptor.
         *
         * @param path  The name of the file. Must be absolute, or relative to the current directory.
         * @param flags Flags controlling the opening-mode of the file.
         * @param mode  Optional file permission mask. Used when creating a file.
         *
         * @return `0` on success; `-errno` on error.
         */
        int Open(const char *path, EFileFlags flags, EFileModes mode = EFileModes::Default);

        /**
         * Read up to `length` bytes from the file into `o_buffer`.
         *
         * @param o_buffer  The buffer to fill with data.
         * @param length    The size of the buffer, in bytes.
         *
         * @note The function may return a valid-value that is smaller than `length`.
         *
         * @return The amount of bytes read on success; `-errno` otherwise.
         */
        virtual ssize_t Read(void *o_buffer, size_t length) override;

        /**
         * Read from an offset in the file.
         *
         * @param o_buffer  The buffer to read into.
         * @param length    The length of the buffer.
         * @param offset    The starting-offset of the read.
         *
         * @note The function may return a valid-value that is smaller than `length`.
         *
         * @return  The number of bytes read on success; `-errno` on error.
         */
        ssize_t Read(void *o_buffer, size_t length, size_t offset);

        /**
         * Read from an offset in the file into the given membuf.
         *
         * @param o_mem     The membuf to fill with data.
         * @param offset    The starting offset of the read.
         *
         * @note The function may return a valid-value that is smaller than the length of the membuf.
         *
         * @return  The number of bytes read on success; `-errno` on error.
         */
        inline ssize_t Read(membuf o_mem, size_t offset)
        {
            return Read(o_mem.buffer, o_mem.length, offset);
        }

        /**
         * Write the given buffer to the file.
         *
         * @param buffer    The buffer containing the data to write.
         * @param length    The size of the buffer, in bytes.
         *
         * @note The function may return a valid-value that is smaller than `length`.
         *
         * @return The amount of bytes written on success; `-errno` otherwise.
         */
        virtual ssize_t Write(const void *buffer, size_t length) override;

        /**
         * Write the given buffer to the file at an offset.
         *
         * @param buffer    The buffer containing the data to write.
         * @param length    The size of the buffer, in bytes.
         * @param offset    The starting offset of the write.
         *
         * @note The function may return a valid-value that is smaller than `length`.
         *
         * @return The amount of bytes written on success; `-errno` otherwise.
         */
        ssize_t Write(const void *buffer, size_t length, size_t offset);

        /**
         * Write the given buffer to the file at an offset.
         *
         * @param mem       The membuf to write.
         * @param offset    The starting offset of the write.
         *
         * @note The function may return a valid-value that is smaller than the length of the membuf.
         *
         * @return The amount of bytes written on success; `-errno` otherwise.
         */
        inline ssize_t Write(const_membuf mem, size_t offset)
        {
            return Write(mem.buffer, mem.length, offset);
        }

        /**
         * Device independent IO control.
         *
         * @param command       A device indicated command op-code.
         * @param parameter     A command-specific parameter.
         *
         * @return Non-negative result on success; `-errno` on error.
         */
        int IOControl(unsigned long command, void *parameter);

        /**
         * Closes the file.
         */
        void Close();

        /**
         * Indicates whether the file contains a handle that appears to be valid.
         *
         * @note This check is superficial. There is a way to ask the OS if the given handle is valid,
         *          but there's no way to promise it actually points to the same object.
         *
         * @return `true` if the file contains a valid descriptor.
         */
        inline bool IsOpen()
        {
            return (m_descriptor > 0);
        }

        /**
         * Returns the underlying file-descriptor handle.
         */
        fd_t GetFileDescriptor() const override
        {
            return m_descriptor;
        }

        /**
         * Creates a new uni-directional pipe and places its ends in the given `File`s.
         *
         * @param o_readEnd     A placeholder for the read-end of the pipe.
         * @param o_writeEnd    A placeholder for the write-end of the pipe.
         * @param flags         An optional set of flags.
         *
         * @note `flags` may contain these following flags: EFileFlags::{CloseOnExec,Direct,NonBlock}
         *
         * @return `0` on success; `-errno` on error.
         */
        static int Pipe(File &o_readEnd, File &o_writeEnd, EFileFlags flags = EFileFlags::None);

    protected:
        /**
         * Holds the OS handle to the open file.
         */
        fd_t m_descriptor;

    private:
        /**
         * Deleted to disallow duplicating the file.
         */
        File(const File &) = delete;
    };
}

#endif //KRAKEN_FILE_H
