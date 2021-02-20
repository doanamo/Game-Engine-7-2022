/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "System/FileSystem/FileDepot.hpp"

/*
    File System

    Manages file depots that are mounted to specified directions, used to
    locate and then open files. File depots acts as virtual file systems
    when mounted, allowing directories to be mapped under specified paths.
    This is not limited to mounting directories, as zipped archives and
    memory regions can be used as mounted virtual file systems as well.

    Implementation of separate native/memory/archive depots was roughly
    inspired by: https://github.com/yevgeniy-logachev/vfspp
*/

namespace System
{
    class FileSystem final : private Common::NonCopyable
    {
    public:
        using FileDepotPtr = std::unique_ptr<FileDepot>;
        using OpenFlags = FileHandle::OpenFlags;

        enum class CreateErrors
        {
            FailedDefaultDepotCreation,
            FailedDefaultDepotMounting,
        };

        using CreateResult = Common::Result<
            std::unique_ptr<FileSystem>, CreateErrors>;

        enum class MountDepotErrors
        {
            EmptyMountPathArgument,
            InvalidMountPathArgument,
            InvalidFileDepotArgument,
        };

        using MountDepotResult = Common::Result<
            void, MountDepotErrors>;

        ~FileSystem();

        static CreateResult Create();

        MountDepotResult MountDepot(fs::path mountPath, FileDepotPtr&& fileDepot);
        FileDepot::OpenFileResult OpenFile(fs::path filePath,
            OpenFlags::Type openFlags = OpenFlags::Read);

    private:
        FileSystem();

        struct MountedDepotEntry
        {
            fs::path mountPath;
            FileDepotPtr fileDepot;
        };

        using MountedDepotList = std::vector<MountedDepotEntry>;

        MountedDepotList m_mountedDepots;
    };
}
