/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/FileSystem/NativeFileHandle.hpp"
using namespace System;

NativeFileHandle::NativeFileHandle(const fs::path& path, OpenFlags::Type flags)
    : FileHandle(path, flags)
{
}

NativeFileHandle::~NativeFileHandle() = default;

FileDepot::OpenFileResult NativeFileHandle::Create(const fs::path& filePath,
    const fs::path& requestedPath, OpenFlags::Type openFlags)
{
    // Create class instance.
    auto instance = std::unique_ptr<NativeFileHandle>(
        new NativeFileHandle(requestedPath, openFlags));

    // Determine file stream mode.
    std::ios_base::openmode openMode = std::fstream::binary;

    if(openFlags & OpenFlags::Read)
    {
        openMode |= std::fstream::in;
    }

    if(openFlags & OpenFlags::Write)
    {
        openMode |= std::fstream::out;
    }

    if(openFlags & OpenFlags::Append)
    {
        openMode |= std::fstream::app;
    }

    if(openFlags & OpenFlags::Truncate)
    {
        openMode |= std::fstream::trunc;
    }

    // Open file stream.
    instance->m_stream.open(filePath, openMode);
    if(!instance->m_stream.is_open() || !instance->m_stream.good())
    {
        OpenFileErrors error = OpenFileErrors::UnknownFileOpenError;

        switch(errno)
        {
        case ENOENT:
            error = OpenFileErrors::FileNotFound;
            break;

        case EACCES:
            error = OpenFileErrors::AccessDenied;
            break;

        case EMFILE:
            error = OpenFileErrors::TooManyHandles;
            break;

        case EFBIG:
            error = OpenFileErrors::FileTooLarge;
            break;
        }

        return Common::Failure(error);
    }

    // Determine file stream size.
    instance->m_stream.ignore(std::numeric_limits<std::streamsize>::max());
    instance->m_size = instance->m_stream.gcount();
    instance->m_stream.clear();
    instance->m_stream.seekg(0, std::ios_base::beg);

    return Common::Success(std::move(instance));
}

uint64_t NativeFileHandle::Tell()
{
    ASSERT(m_stream.tellp() == m_stream.tellg());
    return Common::NumericalCast<uint64_t>(m_stream.tellg());
}

uint64_t NativeFileHandle::Seek(uint64_t offset, SeekMode mode)
{
    std::ios_base::seekdir seekDirection = std::ios_base::beg;

    switch(mode)
    {
    case FileHandle::SeekMode::Current:
        seekDirection = std::ios_base::cur;
        break;

    case FileHandle::SeekMode::End:
        seekDirection = std::ios_base::end;
        break;
    }

    m_stream.seekg(offset, seekDirection);

    ASSERT(m_stream.tellp() == m_stream.tellg());
    return Common::NumericalCast<uint64_t>(m_stream.tellg());
}

uint64_t NativeFileHandle::Read(uint8_t* data, uint64_t bytes)
{
    m_stream.read(reinterpret_cast<char*>(data), bytes);
    return Common::NumericalCast<uint64_t>(m_stream.gcount());
}

uint64_t NativeFileHandle::Write(const uint8_t* data, uint64_t bytes)
{
    m_stream.write(reinterpret_cast<const char*>(data), bytes);
    if(!m_stream.good())
        return 0;

    m_size = std::max(m_size, Common::NumericalCast<uint64_t>(m_stream.tellg()));
    return bytes;
}

bool NativeFileHandle::IsGood() const
{
    return m_stream.good();
}

uint64_t NativeFileHandle::GetSize() const
{
    return m_size;
}
