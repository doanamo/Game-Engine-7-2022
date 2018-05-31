/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Common/Build.hpp"

namespace
{
    std::string workingDir;
    std::string sourceDir;
}

void Build::Initialize()
{
    workingDir = Utility::GetTextFileContent("WorkingDir.txt");
    sourceDir = Utility::GetTextFileContent("SourceDir.txt");
}

std::string Build::GetWorkingDir()
{
    return workingDir;
}

std::string Build::GetSourceDir()
{
    return sourceDir;
}
