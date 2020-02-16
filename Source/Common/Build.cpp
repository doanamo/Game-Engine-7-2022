/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Common/Build.hpp"
#include "Common/Utility.hpp"
#include "Config/BuildInfo.hpp"

namespace
{
    std::string GameDir;
    std::string EngineDir;
}

void Build::Initialize()
{
    // Retrieve directory path.
    GameDir = Utility::GetTextFileContent("GameDir.txt");
    EngineDir = Utility::GetTextFileContent("EngineDir.txt");

    // Set engine directory to be the same as game directory if not specified.
    if(EngineDir.empty())
    {
        EngineDir = GameDir;
    }

    // Print retrieved build info.
    PrintInfo();
}

void Build::PrintInfo()
{
    LOG_INFO("Printing build info...");
    LOG_SCOPED_INDENT();

    LOG_INFO("Engine directory: \"{}\"",EngineDir.empty() ? "Default" : EngineDir);
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
