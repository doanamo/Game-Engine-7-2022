/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/FileSystem.hpp"
using namespace System;

FileSystem::FileSystem() = default;
FileSystem::~FileSystem() = default;

FileSystem::CreateResult FileSystem::Create()
{
    LOG("Creating file system...");
    LOG_SCOPED_INDENT();

    // Print contents of the working directory.
    {
        LOG_DEBUG("Printing working directory contents...");
        LOG_SCOPED_INDENT();

        for(const auto& entry : std::filesystem::directory_iterator("."))
        {
            LOG_DEBUG("{}", entry.path().string());
        }

        #ifndef NDEBUG
            if(std::filesystem::exists("Data"))
            {
                for(const auto& entry : std::filesystem::recursive_directory_iterator("Data"))
                {
                    LOG_DEBUG("{}", entry.path().string());
                }
            }
        #endif
    }

    // Create instance.
    auto instance = std::unique_ptr<FileSystem>(new FileSystem());

    // Mount working directory.
    instance->MountDirectory("./");

    // Success!
    return Common::Success(std::move(instance));
}

FileSystem::MountDirectoryResult FileSystem::MountDirectory(std::string directory)
{
    // Validate argument.
    if(directory.empty())
    {
        LOG_WARNING("Attempted to mount an empty directory path!");
        return Common::Failure(MountDirectoryErrors::EmptyPathArgument);
    }

    // Normalize path separators.
    std::replace(directory.begin(), directory.end(), '\\', '/');

    // Add trailing separator if it is missing.
    if(directory.back() != '/')
    {
        directory += '/';
    }

    // Add mount directory.
    m_mountedDirs.push_back(directory);
    LOG_INFO("Mounted \"{}\" directory.", directory);

    return Common::Success();
}

FileSystem::ResolvePathResult FileSystem::ResolvePath(const std::string path, const std::string relative) const
{
    // Validate path argument.
    if(path.empty())
    {
        LOG_WARNING("Attempting to resolve empty file path!");
        return Common::Failure(ResolvePathErrors::EmptyPathArgument);
    }

    // Array of unresolved paths for debugging.
    #ifndef NDEBUG
        std::vector<std::string> unresolvedPaths;
    #endif

    // Extract relative directory from relative path.
    std::string relativeResolvedDir;

    if(!relative.empty())
    {
        relativeResolvedDir = Common::GetFileDirectory(relative);

        // Relative path should be already resolved and valid.
        // Saves time of resolving and there should be no reason to not have it resolved already.
        #ifndef NDEBUG
            if(!std::filesystem::exists(relative))
            {
                LOG_WARNING("Trying to resolve \"{}\" path with relative \"{}\" path that does not exist!");
            }
        #endif
    }

    // Try to resolve path using specified relative directory.
    if(!relativeResolvedDir.empty())
    {
        std::string resolvePath = relativeResolvedDir + path;
        if(std::filesystem::exists(resolvePath))
            return Common::Success(resolvePath);

        #ifndef NDEBUG
            unresolvedPaths.push_back(resolvePath);
        #endif
    }

    // Check file path for each mounted directory (iterated in reverse).
    for(auto mountedDir = m_mountedDirs.crbegin(); mountedDir != m_mountedDirs.crend(); ++mountedDir)
    {
        std::string resolvePath = *mountedDir + path;
        if(std::filesystem::exists(resolvePath))
            return Common::Success(resolvePath);

        #ifndef NDEBUG
            unresolvedPaths.push_back(resolvePath);
        #endif
    }

    // Failed to resolve path.
    LOG_WARNING("Failed to resolve \"{}\" path!", path);

    #ifndef NDEBUG
    {
        LOG_DEBUG("Following paths did not resolve to any existing file:");
        LOG_SCOPED_INDENT();

        for(const std::string& unresolvedPath : unresolvedPaths)
        {
            LOG_DEBUG("{}", unresolvedPath);
        }
    }
    #endif

    return Common::Failure(ResolvePathErrors::UnresolvablePath);
}
