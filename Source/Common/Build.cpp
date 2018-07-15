/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Common/Build.hpp"
#include "Common/Utility.hpp"
#include "Config/BuildInfo.hpp"

namespace
{
    std::string WorkingDir;
}

void Build::Initialize()
{
    // Retrieve the working directory path.
    WorkingDir = Utility::GetTextFileContent("WorkingDir.txt");

    // Print retrieved build info.
    PrintInfo();
}

void Build::PrintInfo()
{
    LOG_INFO() << "Printing retrieved build info..." << LOG_INDENT();
    LOG_INFO() << "Working directory: " << (Build::GetWorkingDir().empty() ? "default" : Build::GetWorkingDir());

    {
        LOG_INFO() << "Engine build info:" << LOG_INDENT();
        LOG_INFO() << "Change number - " << Build::GetEngineChangeNumber();
        LOG_INFO() << "Commit hash   - " << Build::GetEngineChangeHash();
        LOG_INFO() << "Commit date   - " << Build::GetEngineChangeDate();
        LOG_INFO() << "Branch name   - " << Build::GetEngineBranchName();
    }

    {
        LOG_INFO() << "Project build info:" << LOG_INDENT();
        LOG_INFO() << "Change number - " << Build::GetProjectChangeNumber();
        LOG_INFO() << "Commit hash   - " << Build::GetProjectChangeHash();
        LOG_INFO() << "Commit date   - " << Build::GetProjectChangeDate();
        LOG_INFO() << "Branch name   - " << Build::GetProjectBranchName();
    }
    
}

std::string Build::GetWorkingDir()
{
    return WorkingDir;
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

std::string Build::GetProjectChangeNumber()
{
    return BuildInfo::Project::ChangeNumber;
}

std::string Build::GetProjectChangeHash()
{
    return BuildInfo::Project::CommitHash;
}

std::string Build::GetProjectChangeDate()
{
    return BuildInfo::Project::CommitDate;
}

std::string Build::GetProjectBranchName()
{
    return BuildInfo::Project::BranchName;
}
