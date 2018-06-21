/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Common/Build.hpp"
#include "Common/Utility.hpp"
#include "Config/BuildInfo.hpp"

namespace
{
    std::string MountDir;
}

void Build::Initialize()
{
    MountDir = Utility::GetTextFileContent("MountDirectory.txt");
}

std::string Build::GetMountDir()
{
    return MountDir;
}

std::string Build::GetBuildDir()
{
    return BuildInfo::BuildDir;
}

std::string Build::GetIncludeDir()
{
    return BuildInfo::IncludeDir;
}

std::string Build::GetSourceDir()
{
    return BuildInfo::SourceDir;
}

std::string Build::GetChangeNumber()
{
    return BuildInfo::ChangeNumber;
}

std::string Build::GetChangeHash()
{
    return BuildInfo::CommitHash;
}

std::string Build::GetChangeDate()
{
    return BuildInfo::CommitDate;
}

std::string Build::GetBranchName()
{
    return BuildInfo::BranchName;
}
