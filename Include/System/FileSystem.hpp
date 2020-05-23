/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>
#include <filesystem>

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
            NonDirectoryPathArgument,
        };

        enum class ResolvePathErrors
        {
            EmptyPathArgument,
            UnresolvablePath,
        };

        using CreateResult = Common::Result<std::unique_ptr<FileSystem>, void>;
        static CreateResult Create();

        using MountDirectoryResult = Common::Result<void, MountDirectoryErrors>;
        using ResolvePathResult = Common::Result<std::filesystem::path, ResolvePathErrors>;
        using MountedDirList = std::vector<std::filesystem::path>;

    public:
        ~FileSystem();

        // Mounts directories used for resolving paths.
        MountDirectoryResult MountDirectory(std::filesystem::path directory);

        // Resolves path by searching for it in mounted directories.
        // Relative path can be specified where search will begin first, but it must be already resolved.
        ResolvePathResult ResolvePath(std::filesystem::path path, std::filesystem::path relative = "") const;

    private:
        FileSystem();

    private:
        MountedDirList m_mountedDirs;
    };
}
