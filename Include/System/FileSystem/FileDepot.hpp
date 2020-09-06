/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "System/FileSystem/FileHandle.hpp"

/*
    File Depot

    Interface for file depot implementations that are mounted in file system.
*/

namespace System
{
    class FileDepot
    {
    public:
        enum class OpenFileErrors
        {
            EmptyFilePathArgument,
            InvalidFilePathArgument,
            InvalidOpenFlagsArgument,
            UnknownFileOpeningError,
            FileNotFound,
            AccessDenied,
            TooManyHandles,
            FileTooLarge,
        };

        using OpenFileResult = Common::Result<std::unique_ptr<FileHandle>, OpenFileErrors>;
        using DirectoryContentList = std::vector<fs::path>;

        virtual ~FileDepot() = default;
        virtual OpenFileResult OpenFile(const fs::path& depotPath,
            const fs::path& requestedPath, FileHandle::OpenFlags::Type openFlags) = 0;
    };
}
