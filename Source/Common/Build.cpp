/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Common/Build.hpp"
#include "Common/Utility.hpp"

namespace
{
    std::string workingDir;
    std::string sourceDir;
    std::string includeDir;
}

void Build::Initialize()
{
    workingDir = Utility::GetTextFileContent("WorkingDir.txt");
    includeDir = Utility::GetTextFileContent("IncludeDir.txt");
    sourceDir = Utility::GetTextFileContent("SourceDir.txt");
}

std::string Build::GetWorkingDir()
{
    return workingDir;
}

std::string Build::GetIncludeDir()
{
    return includeDir;
}

std::string Build::GetSourceDir()
{
    return sourceDir;
}
