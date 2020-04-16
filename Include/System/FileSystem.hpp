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
    class FileSystem final : private NonCopyable, public Resettable<FileSystem>
    {
    public:
        using MountedDirList = std::vector<std::string>;

        enum class MountDirectoryErrors
        {
            EmptyPathArgument,
        };

        using MountDirectoryResult = Result<void, MountDirectoryErrors>;

        enum class ResolvePathErrors
        {
            EmptyPathArgument,
            UnresolvablePath,
        };

        using ResolvePathResult = Result<std::string, ResolvePathErrors>;

    public:
        FileSystem();
        ~FileSystem();

        GenericResult Initialize();
        MountDirectoryResult MountDirectory(std::string directory);
        ResolvePathResult ResolvePath(const std::string path) const;

    private:
        MountedDirList m_mountedDirs;
        bool m_initialized = false;
    };
}
