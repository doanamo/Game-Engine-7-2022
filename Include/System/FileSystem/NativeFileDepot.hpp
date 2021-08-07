/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "System/FileSystem/FileDepot.hpp"

/*
    Native File Depot

    Collection of loose files present in native directory that can be mounted under different path
    using file system.
*/

namespace System
{
    class NativeFileDepot final : public FileDepot
    {
    public:
        enum class CreateErrors
        {
            EmptyDepotDirectoryArgument,
            NonExistingDepotDirectory,
        };

        using CreateResult = Common::Result<std::unique_ptr<NativeFileDepot>, CreateErrors>;
        static CreateResult Create(fs::path fileDirectory);

    public:
        ~NativeFileDepot();

        OpenFileResult OpenFile(const fs::path& depotPath, const fs::path& requestedPath,
            FileHandle::OpenFlags::Type openFlags) override;

    private:
        NativeFileDepot();

    private:
        fs::path m_fileDirectory;
    };
}
