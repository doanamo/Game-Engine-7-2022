/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include <Engine.hpp>

int main()
{
    Debug::Initialize();
    Logger::Initialize();

    {
        LOG() << "Build info:" << LOG_INDENT();
        LOG() << "Build directory: " << Build::GetBuildDir();
        LOG() << "Include directory: " << Build::GetIncludeDir();
        LOG() << "Source directory: " << Build::GetSourceDir();
        LOG() << "Change number: " << Build::GetChangeNumber();
        LOG() << "Change hash: " << Build::GetChangeHash();
        LOG() << "Change date: " << Build::GetChangeDate();
        LOG() << "Branch name: " << Build::GetBranchName();
    }

    std::cin.get();
}
