/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include <Engine.hpp>

int main()
{
    Build::Initialize();
    Debug::Initialize();

    std::cout << Build::GetWorkingDir() << std::endl;
    std::cout << Build::GetIncludeDir() << std::endl;
    std::cout << Build::GetSourceDir() << std::endl;

    std::cin.get();
}
