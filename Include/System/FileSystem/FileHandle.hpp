/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    File Handle

    Base for implementations of files opened by file system through file depots
    that are ready for reading and writing if returned.
*/

namespace System
{
    class FileHandle : private Common::NonCopyable
    {
    public:
        enum class SeekMode
        {
            Begin,
            Current,
            End,
        };

        struct OpenFlags
        {
            enum
            {
                None = 0,

                Read = 1 << 0,
                Write = 1 << 1,
                Append = 1 << 2,
                Truncate = 1 << 3,

                ReadWrite = Read | Write,
            };

            using Type = uint8_t;
        };

        virtual ~FileHandle();

        virtual uint64_t Tell() = 0;
        virtual uint64_t Seek(uint64_t offset, SeekMode mode = SeekMode::Begin) = 0;
        virtual uint64_t Read(uint8_t* data, uint64_t bytes) = 0;
        virtual uint64_t Write(const uint8_t* data, uint64_t bytes) = 0;

        virtual bool IsGood() const = 0;
        virtual uint64_t GetSize() const = 0;

        const std::string& GetPath() const;
        OpenFlags::Type GetFlags() const;
        bool IsReadOnly() const;

        std::vector<uint8_t> ReadAsBinaryArray();
        std::string ReadAsTextString();

        template<typename Type>
        bool Read(Type& value)
        {
            return Read(&value, sizeof(Type)) == sizeof(Type);
        }

        template<typename Type>
        bool Write(const Type& value)
        {
            return Write(&value, sizeof(Type)) == sizeof(Type);
        }

    protected:
        FileHandle(const std::string& path, OpenFlags::Type flags);

    private:
        std::string m_path;
        OpenFlags::Type m_flags;
    };
}
