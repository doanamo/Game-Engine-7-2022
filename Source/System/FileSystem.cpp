/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "System/FileSystem.hpp"
using namespace System;

FileSystem::FileSystem() = default;
FileSystem::~FileSystem() = default;

GenericResult FileSystem::Initialize()
{
    // Print initialization info.
    LOG("Initializing file system...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Success!
    m_initialized = true;
    return Success();
}

FileSystem::MountDirectoryResult FileSystem::MountDirectory(std::string directory)
{
    ASSERT(m_initialized, "File system has not been initialzied!");

    // Validate argument.
    if(directory.empty())
    {
        LOG_WARNING("Attempted to mount an empty directory path!");
        return Failure(MountDirectoryErrors::EmptyPathArgument);
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

    // Success!
    return Success();
}

FileSystem::ResolvePathResult FileSystem::ResolvePath(const std::string path) const
{
    ASSERT(m_initialized, "File system has not been initialzied!");

    // Validate argument.
    if(path.empty())
    {
        LOG_WARNING("Attempting to resolve empty file path!");
        return Failure(ResolvePathErrors::EmptyPathArgument);
    }

    // Check file path for each mounted directory (iterated in reverse).
    for(auto it = m_mountedDirs.crbegin(); it != m_mountedDirs.crend(); ++it)
    {
        // Create resolved path.
        std::string resolvedPath = *it + path;

        // Check if file or directory exists (good() works with directories as well).
        // This may be too slow for the purpose, but does its job for now.
        std::ifstream file(resolvedPath);

        if(file.good())
        {
            // Return resolved path.
            return Success(resolvedPath);
        }
    }

    // Failed to resolve path.
    return Failure(ResolvePathErrors::UnresolvablePath);
}
