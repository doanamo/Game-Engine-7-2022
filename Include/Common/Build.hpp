/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>

/*
    Build

    Stores information generated and retrieved from the build system.
*/

namespace Build
{
    // Initializes build related info.
    void Initialize();

    // Prints retrieved build info to log.
    void PrintInfo();

    // Gets the working directory.
    std::string GetWorkingDir();

    // Build info retrieved from engine's repository.
    std::string GetEngineChangeNumber();
    std::string GetEngineChangeHash();
    std::string GetEngineChangeDate();
    std::string GetEngineBranchName();

    // Build info retrieved from project's repository.
    std::string GetProjectChangeNumber();
    std::string GetProjectChangeHash();
    std::string GetProjectChangeDate();
    std::string GetProjectBranchName();
}
