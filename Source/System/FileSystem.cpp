/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "System/Precompiled.hpp"
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

    // Success!
    return Common::Success(std::move(instance));
}

FileSystem::MountDirectoryResult FileSystem::MountDirectory(std::filesystem::path directory)
{
    // Expand to absolute path.
    std::filesystem::path directoryAbsolute = std::filesystem::canonical(directory);

    // Validate argument.
    if(directory.empty())
    {
        LOG_WARNING("Attempted to mount an empty directory path!");
        return Common::Failure(MountDirectoryErrors::EmptyPathArgument);
    }

    if(!std::filesystem::is_directory(directoryAbsolute))
    {
        LOG_ERROR("Cannot mount \"{}\" path that is not a directory!", directoryAbsolute.generic_string());
        return Common::Failure(MountDirectoryErrors::NonDirectoryPathArgument);
    }

    // Add mount directory, but store it as a relative path (more optimal).
    LOG_INFO("Mounted \"{}\" directory.", directoryAbsolute.generic_string());
    m_mountedDirs.push_back(std::filesystem::relative(directoryAbsolute));

    // Success!
    return Common::Success();
}

FileSystem::ResolvePathResult FileSystem::ResolvePath(std::filesystem::path path, std::filesystem::path relative) const
{
    // Validate path argument.
    if(path.empty())
    {
        LOG_WARNING("Attempting to resolve empty file path!");
        return Common::Failure(ResolvePathErrors::EmptyPathArgument);
    }

    // Array of unresolved paths for debugging.
    #ifndef NDEBUG
        std::vector<std::filesystem::path> unresolvedPaths;
    #endif

    // Extract directory path from relative path which may contain a filename.
    std::filesystem::path relativeResolvedDir;

    if(!relative.empty())
    {
        relativeResolvedDir = relative;

        if(!std::filesystem::is_directory(relativeResolvedDir))
        {
            relativeResolvedDir.remove_filename();
        }

        // Relative path should be already resolved and valid.
        // Saves time of resolving and there should be no reason to not have it as such already.
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
        std::filesystem::path resolvePath = relativeResolvedDir / path;
        if(std::filesystem::exists(resolvePath))
        {
            return Common::Success(resolvePath);
        }

        #ifndef NDEBUG
            unresolvedPaths.push_back(resolvePath);
        #endif
    }

    // Check file path for each mounted directory (iterated in reverse).
    for(auto mountedDir = m_mountedDirs.crbegin(); mountedDir != m_mountedDirs.crend(); ++mountedDir)
    {
        std::filesystem::path resolvePath = *mountedDir / path;
        if(std::filesystem::exists(resolvePath))
        {
            return Common::Success(resolvePath);
        }

        #ifndef NDEBUG
            unresolvedPaths.push_back(resolvePath);
        #endif
    }

    // Failed to resolve path.
    LOG_WARNING("Failed to resolve \"{}\" path!", path.generic_string());

    #ifndef NDEBUG
        LOG_DEBUG("Following paths did not resolve to any existing file:");
        LOG_SCOPED_INDENT();

        for(auto& unresolvedPath : unresolvedPaths)
        {
            LOG_DEBUG("{}", unresolvedPath.generic_string());
        }
    #endif

    return Common::Failure(ResolvePathErrors::UnresolvablePath);
}
