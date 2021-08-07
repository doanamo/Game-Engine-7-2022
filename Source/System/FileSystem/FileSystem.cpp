/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/FileSystem/FileSystem.hpp"
#include "System/FileSystem/NativeFileDepot.hpp"
#include <Build/Build.hpp>
using namespace System;

namespace
{
    const char* AttachError = "Failed to create file system instance! {}";
}

FileSystem::FileSystem() = default;
FileSystem::~FileSystem() = default;

bool FileSystem::OnAttach(const Core::EngineSystemStorage& engineSystems)
{
    // Mount native working directory.
    LOG("Current working directory: {}", fs::current_path().generic_string());

    if(auto workingDirectoryDepot = NativeFileDepot::Create("./"))
    {
        if(!MountDepot("./", workingDirectoryDepot.Unwrap()))
        {
            LOG_ERROR(AttachError, "Could not mount default working directory.");
            return false;
        }
    }
    else
    {
        LOG_ERROR(AttachError, "Could not create default working directory.");
        return false;
    }

    // Mount native engine directory.
    if(!Build::GetEngineDir().empty())
    {
        if(auto engineDirectoryDepot = NativeFileDepot::Create(Build::GetEngineDir()))
        {
            if(!MountDepot("./", engineDirectoryDepot.Unwrap()))
            {
                LOG_ERROR(AttachError, "Could not mount default engine directory.");
                return false;
            }
        }
        else
        {
            LOG_ERROR(AttachError, "Could not create default engine directory depot.");
            return false;
        }
    }

    // Mount native game directory.
    if(!Build::GetGameDir().empty())
    {
        if(auto gameDirectoryDepot = NativeFileDepot::Create(Build::GetGameDir()))
        {
            if(!MountDepot("./", gameDirectoryDepot.Unwrap()))
            {
                LOG_ERROR(AttachError, "Could not mount default game directory.");
                return false;
            }
        }
        else
        {
            LOG_ERROR(AttachError, "Could not create default game directory depot.");
            return false;
        }
    }

    return true;
}

FileSystem::MountDepotResult FileSystem::MountDepot(fs::path mountPath, FileDepotPtr&& fileDepot)
{
    CHECK_ARGUMENT_OR_RETURN(!mountPath.empty(),
        Common::Failure(MountDepotErrors::EmptyMountPathArgument));
    CHECK_ARGUMENT_OR_RETURN(fileDepot != nullptr,
        Common::Failure(MountDepotErrors::InvalidFileDepotArgument));

    // Check whether mount path is a valid directory path.
    if(mountPath.has_filename())
    {
        LOG_ERROR("Cannot mount path \"{}\" that contains file name!",
            mountPath.generic_string());
        return Common::Failure(MountDepotErrors::InvalidMountPathArgument);
    }

    // Map file depot to mount path.
    m_mountedDepots.push_back({ mountPath.lexically_normal(), std::move(fileDepot) });
    return Common::Success();
}

FileDepot::OpenFileResult FileSystem::OpenFile(
    fs::path filePath, FileHandle::OpenFlags::Type openFlags)
{
    CHECK_ARGUMENT_OR_RETURN(!filePath.empty(),
        Common::Failure(FileDepot::OpenFileErrors::EmptyFilePathArgument));
    CHECK_ARGUMENT_OR_RETURN(openFlags != FileHandle::OpenFlags::None,
        Common::Failure(FileDepot::OpenFileErrors::InvalidOpenFlagsArgument));

    // Check whether file path contains file name.
    filePath = filePath.lexically_normal();
    if(!filePath.has_filename())
    {
        LOG_ERROR("Cannot open file from path \"{}\" that does not contain file name!",
            filePath.generic_string());
        return Common::Failure(FileDepot::OpenFileErrors::InvalidFilePathArgument);
    }

    // Walk through mounted file depots in reverse order (last mounted file depot has precedence).
    for(auto depot = m_mountedDepots.crbegin(); depot != m_mountedDepots.crend(); ++depot)
    {
        const fs::path& mountPath = depot->mountPath;
        auto mountPathIt = mountPath.begin();
        auto filePathIt = filePath.begin();

        // Check whether file path is within mounted depot path.
        while(mountPathIt != mountPath.end() && filePathIt != filePath.end())
        {
            if(*mountPathIt == ".")
            {
                ++mountPathIt;
                continue;
            }

            if(*filePathIt == ".")
            {
                ++filePathIt;
                continue;
            }

            if(*mountPathIt != *filePathIt)
                break;

            ++mountPathIt;
            ++filePathIt;
        }

        // If we reached the end of mount path then it is contained withing file path.
        if(mountPathIt == mountPath.end())
        {
            // Determine file path inside depot.
            fs::path depotFilePath = std::accumulate(
                filePathIt, filePath.end(), fs::path(), std::divides());

            // Attempt to open file inside depot.
            if(auto openFileResult = depot->fileDepot->OpenFile(depotFilePath, filePath, openFlags))
            {
                LOG_SUCCESS("Opened \"{}\" file.", filePath.generic_string());
                return openFileResult;
            }
            else
            {
                // If file was simply not found in depot, continue looking through remaining depots.
                FileDepot::OpenFileErrors openFileError = openFileResult.UnwrapFailure();
                if(openFileError != FileDepot::OpenFileErrors::FileNotFound)
                {
                    return Common::Failure(openFileError);
                }
            }
        }
    }

    // Could not find file in any mounted file depot.
    LOG_ERROR("Could not open \"{}\" file!", filePath.generic_string());
    return Common::Failure(FileDepot::OpenFileErrors::FileNotFound);
}
