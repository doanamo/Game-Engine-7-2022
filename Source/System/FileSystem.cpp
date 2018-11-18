/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/FileSystem.hpp"
using namespace System;

FileSystem::FileSystem() :
    m_initialized(false)
{
}

FileSystem::~FileSystem()
{
}

FileSystem::FileSystem(FileSystem&& other) :
    FileSystem()
{
    *this = std::move(other);
}

FileSystem& FileSystem::operator=(FileSystem&& other)
{
    std::swap(m_mountDirs, other.m_mountDirs);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool FileSystem::Initialize()
{
    LOG() << "Initializing file system..." << LOG_INDENT();

    // Make sure that the class instance has not been already initialized.
    VERIFY(!m_initialized, "File system has already been initialized!");

    // Setup a cleanup guard.
    SCOPE_GUARD_IF(!m_initialized, *this = FileSystem());

    // Success!
    return m_initialized = true;
}

bool FileSystem::MountDirectory(std::string dirPath)
{
    ASSERT(m_initialized, "File system has not been initialzied!");

    // Validate argument.
    if(dirPath.empty())
    {
        LOG_WARNING() << "Attempted to mount an empty directory path!";
        return false;
    }

    // Normalize path separators.
    std::replace(dirPath.begin(), dirPath.end(), '\\', '/');

    // Add a trailing separator if it is missing.
    if(dirPath.back() != '/')
    {
        dirPath += '/';
    }

    // Add a mount directory.
    m_mountDirs.push_back(dirPath);

    LOG_INFO() << "Mounted \"" << dirPath << "\" directory.";

    // Success!
    return true;
}

std::string FileSystem::ResolvePath(const std::string filePath)
{
    ASSERT(m_initialized, "File system has not been initialzied!");

    // Validate argument.
    if(filePath.empty())
    {
        LOG_WARNING() << "Attempting to resolve empty file path!";
        return filePath;
    }

    // Check file path for each mounted directory (iterated in reverse).
    for(auto it = m_mountDirs.crbegin(); it != m_mountDirs.crend(); ++it)
    {
        // Create a resolved path.
        std::string resolvedPath = *it + filePath;

        // Check if file or directory exists (good() works with directories as well).
        // This may be bit slow, but does its job for now.
        std::ifstream file(resolvedPath);

        if(file.good())
        {
            // Return the resolved path.
            return resolvedPath;
        }
    }

    // Return the provided argument if path has not been resolved.
    return filePath;
}
