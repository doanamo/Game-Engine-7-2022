/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>

/*
    Build

    Stores information generated and retrieved from the build system.
    Need to be initialized to be populated with build information.
*/

namespace Build
{
    // Initializes info gathered during build process.
    void Initialize();

    // Prints build info to log.
    void PrintInfo();

    // Returns directories where engine and game data is located.
    std::string GetEngineDir();
    std::string GetGameDir();

    // Returns information from engine and game repository.
    std::string GetEngineChangeNumber();
    std::string GetEngineChangeHash();
    std::string GetEngineChangeDate();
    std::string GetEngineBranchName();

    std::string GetGameChangeNumber();
    std::string GetGameChangeHash();
    std::string GetGameChangeDate();
    std::string GetGameBranchName();
}
