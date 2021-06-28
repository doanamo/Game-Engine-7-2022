/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/EngineSystem.hpp>
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
    class FileSystem final : public Core::EngineSystem
    {
        REFLECTION_ENABLE(FileSystem, Core::EngineSystem)

    public:
        using FileDepotPtr = std::unique_ptr<FileDepot>;
        using OpenFlags = FileHandle::OpenFlags;

        enum class MountDepotErrors
        {
            EmptyMountPathArgument,
            InvalidMountPathArgument,
            InvalidFileDepotArgument,
        };

        using MountDepotResult = Common::Result<void, MountDepotErrors>;

    public:
        FileSystem();
        ~FileSystem() override;

        MountDepotResult MountDepot(fs::path mountPath, FileDepotPtr&& fileDepot);
        FileDepot::OpenFileResult OpenFile(fs::path filePath,
            OpenFlags::Type openFlags = OpenFlags::Read);

    private:
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;

        struct MountedDepotEntry
        {
            fs::path mountPath;
            FileDepotPtr fileDepot;
        };

        using MountedDepotList = std::vector<MountedDepotEntry>;

    private:
        MountedDepotList m_mountedDepots;
    };
}

REFLECTION_TYPE(System::FileSystem, Core::EngineSystem)
