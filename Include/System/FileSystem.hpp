/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>

/*
    File System
*/

namespace System
{
    class FileSystem : private NonCopyable
    {
    public:
        using MountedDirList = std::vector<std::string>;

    public:
        FileSystem() = default;
        ~FileSystem() = default;

        FileSystem(FileSystem&& other);
        FileSystem& operator=(FileSystem&& other);

        bool Initialize();
        bool MountDirectory(std::string dirPath);
        std::string ResolvePath(const std::string filePath);

    private:
        MountedDirList m_mountedDirs;
        bool m_initialized = false;
    };
}
