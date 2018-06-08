/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include <Engine.hpp>

int main()
{
    Debug::Initialize();

    std::cout << Build::GetBuildDir() << std::endl;
    std::cout << Build::GetIncludeDir() << std::endl;
    std::cout << Build::GetSourceDir() << std::endl;
    std::cout << Build::GetChangeNumber() << std::endl;
    std::cout << Build::GetChangeHash() << std::endl;
    std::cout << Build::GetChangeDate() << std::endl;
    std::cout << Build::GetBranchName() << std::endl;

    std::cin.get();
}
