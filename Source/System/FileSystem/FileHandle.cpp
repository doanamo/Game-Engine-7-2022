/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "System/Precompiled.hpp"
#include "System/FileSystem/FileHandle.hpp"
using namespace System;

FileHandle::FileHandle(const std::string& path, OpenFlags::Type flags) :
    m_path(path), m_flags(flags)
{
}

FileHandle::~FileHandle() = default;

const std::string& FileHandle::GetPath() const
{
    return m_path;
}

FileHandle::OpenFlags::Type FileHandle::GetFlags() const
{
    return m_flags;
}

bool FileHandle::IsReadOnly() const
{
    return (m_flags & OpenFlags::Read) && !(m_flags & OpenFlags::Write);
}

std::vector<uint8_t> FileHandle::ReadAsBinaryArray()
{
    std::vector<uint8_t> binary;
    binary.resize(GetSize());

    Seek(0, SeekMode::Begin);
    Read((uint8_t*)&binary[0], GetSize());

    return binary;
}

std::string FileHandle::ReadAsTextString()
{
    std::string text;
    text.resize(GetSize());

    Seek(0, SeekMode::Begin);
    Read((uint8_t*)&text[0], GetSize());

    return text;
}
