/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    File System
*/

namespace System
{
    // File system class.
    class FileSystem
    {
    public:
        // Type declarations.
        using MountDirList = std::vector<std::string>;

    public:
        FileSystem();
        ~FileSystem();

        FileSystem(const FileSystem& other) = delete;
        FileSystem& operator=(const FileSystem& other) = delete;

        FileSystem(FileSystem&& other);
        FileSystem& operator=(FileSystem&& other);

        // Initializes the file system.
        bool Initialize();

        // Mounts a directory that will be used to resolve file paths.
        bool MountDirectory(std::string dirPath);

        // Resolves a path of a file in mounted directories to full file path.
        std::string ResolvePath(const std::string filePath);

    private:
        // Mount directories.
        MountDirList m_mountDirs;

        // Initialization state.
        bool m_initialized;
    };
}
