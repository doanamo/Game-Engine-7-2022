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
    class FileSystem final : private NonCopyable
    {
    public:
        enum class MountDirectoryErrors
        {
            EmptyPathArgument,
        };

        enum class ResolvePathErrors
        {
            EmptyPathArgument,
            UnresolvablePath,
        };

        using CreateResult = Result<std::unique_ptr<FileSystem>, void>;
        static CreateResult Create();

        using MountDirectoryResult = Result<void, MountDirectoryErrors>;
        using ResolvePathResult = Result<std::string, ResolvePathErrors>;
        using MountedDirList = std::vector<std::string>;

    public:
        ~FileSystem();

        MountDirectoryResult MountDirectory(std::string directory);
        ResolvePathResult ResolvePath(const std::string path) const;

    private:
        FileSystem();

    private:
        MountedDirList m_mountedDirs;
    };
}
