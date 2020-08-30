/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Build/Build.hpp"
#include "BuildInfo.hpp"
#include <Common/Utility.hpp>

namespace
{
    std::string GameDir;
    std::string EngineDir;
}

void Build::Initialize()
{
    // Retrieve engine and game directory overrides from working directory.
    // When application is launched these files will not be present as both files
    // would otherwise point at same current working directory, but when launching
    // from generated project in development both will be created by CMake in temporary
    // output directory to account for engine and game directories being separate,
    // same as for their repositories. This allows engine data to be loaded easily
    // when library is added to another project as dependency.

    EngineDir = Common::GetTextFileContent("EngineDir.txt");
    GameDir = Common::GetTextFileContent("GameDir.txt");

    if(EngineDir.empty())
    {
        EngineDir = GameDir;
    }

    Build::PrintInfo();
}

void Build::PrintInfo()
{
    LOG_INFO("Printing build information...");
    LOG_SCOPED_INDENT();

    LOG_INFO("Engine directory: \"{}\"", EngineDir.empty() ? "./" : EngineDir);
    LOG_INFO("Engine repository: {}-{}-{} ({})",
        Build::GetEngineChangeNumber(),
        Build::GetEngineChangeHash(),
        Build::GetEngineBranchName(),
        Build::GetEngineChangeDate());

    LOG_INFO("Game directory: \"{}\"", GameDir.empty() ? "./" : GameDir);
    LOG_INFO("Game repository: {}-{}-{} ({})",
        Build::GetGameChangeNumber(),
        Build::GetGameChangeHash(),
        Build::GetGameBranchName(),
        Build::GetGameChangeDate());
}

std::string Build::GetGameDir()
{
    return GameDir;
}

std::string Build::GetEngineDir()
{
    return EngineDir;
}

std::string Build::GetEngineChangeNumber()
{
    return BuildInfo::Engine::ChangeNumber;
}

std::string Build::GetEngineChangeHash()
{
    return BuildInfo::Engine::CommitHash;
}

std::string Build::GetEngineChangeDate()
{
    return BuildInfo::Engine::CommitDate;
}

std::string Build::GetEngineBranchName()
{
    return BuildInfo::Engine::BranchName;
}

std::string Build::GetGameChangeNumber()
{
    return BuildInfo::Game::ChangeNumber;
}

std::string Build::GetGameChangeHash()
{
    return BuildInfo::Game::CommitHash;
}

std::string Build::GetGameChangeDate()
{
    return BuildInfo::Game::CommitDate;
}

std::string Build::GetGameBranchName()
{
    return BuildInfo::Game::BranchName;
}
