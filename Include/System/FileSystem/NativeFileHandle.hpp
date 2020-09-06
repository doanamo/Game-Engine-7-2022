/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "System/FileSystem/FileHandle.hpp"
#include "System/FileSystem/FileDepot.hpp"

/*
    Native File Handle
*/

namespace System
{
    class NativeFileHandle final : public FileHandle
    {
    public:
        using OpenFileErrors = FileDepot::OpenFileErrors;

        static FileDepot::OpenFileResult Create(const fs::path& filePath,
            const fs::path& requestedPath, OpenFlags::Type openFlags);

        ~NativeFileHandle();

        uint64_t Tell() override;
        uint64_t Seek(uint64_t offset, SeekMode mode) override;
        uint64_t Read(uint8_t* data, uint64_t bytes) override;
        uint64_t Write(const uint8_t* data, uint64_t bytes) override;

        bool IsGood() const override;
        uint64_t GetSize() const override;

    private:
        NativeFileHandle(const std::string& path, OpenFlags::Type flags);

        std::fstream m_stream;
        uint64_t m_size;
    };
}
