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
    // Read directory overrides from files in current working directory.
    GameDir = Common::GetTextFileContent("GameDir.txt");
    EngineDir = Common::GetTextFileContent("EngineDir.txt");

    if(EngineDir.empty())
    {
        EngineDir = GameDir;
    }
}

void Build::PrintInfo()
{
    LOG_INFO("Printing build info...");
    LOG_SCOPED_INDENT();

    LOG_INFO("Engine directory: \"{}\"", EngineDir.empty() ? "Default" : EngineDir);
    LOG_INFO("Engine repository: {}-{}-{} ({})",
        Build::GetEngineChangeNumber(),
        Build::GetEngineChangeHash(),
        Build::GetEngineBranchName(),
        Build::GetEngineChangeDate());

    LOG_INFO("Game directory: \"{}\"", GameDir.empty() ? "Default" : GameDir);
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
