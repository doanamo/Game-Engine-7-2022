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
    class FileSystem final : private Common::NonCopyable
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

        using CreateResult = Common::Result<std::unique_ptr<FileSystem>, void>;
        static CreateResult Create();

        using MountDirectoryResult = Common::Result<void, MountDirectoryErrors>;
        using ResolvePathResult = Common::Result<std::string, ResolvePathErrors>;
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
