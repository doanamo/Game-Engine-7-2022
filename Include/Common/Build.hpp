/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
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

    // Gets the directory where engine data is located.
    std::string GetEngineDir();

    // Build info retrieved from engine's repository.
    std::string GetEngineChangeNumber();
    std::string GetEngineChangeHash();
    std::string GetEngineChangeDate();
    std::string GetEngineBranchName();

    // Gets the directory where game data is located.
    std::string GetGameDir();

    // Build info retrieved from game's repository.
    std::string GetGameChangeNumber();
    std::string GetGameChangeHash();
    std::string GetGameChangeDate();
    std::string GetGameBranchName();
}
