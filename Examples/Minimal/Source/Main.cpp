/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include <Engine.hpp>
#include <System/Platform.hpp>
#include <System/Window.hpp>

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

    System::Platform platform;
    if(!platform.Initialize())
        return 1;

    System::WindowInfo windowInfo;
    windowInfo.width = 1024;
    windowInfo.height = 576;
    windowInfo.vsync = false;
    windowInfo.visible = true;

    System::Window window;
    if(!window.Open(windowInfo))
        return 1;

    while(window.IsOpen())
    {
        window.ProcessEvents();

        window.Present();
    }
}
