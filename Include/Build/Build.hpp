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
    void Initialize();
    void PrintInfo();

    std::string GetEngineDir();
    std::string GetGameDir();

    std::string GetEngineChangeNumber();
    std::string GetEngineChangeHash();
    std::string GetEngineChangeDate();
    std::string GetEngineBranchName();

    std::string GetGameChangeNumber();
    std::string GetGameChangeHash();
    std::string GetGameChangeDate();
    std::string GetGameBranchName();
}
