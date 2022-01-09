/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Platform
*/

#if defined(WIN32)
    #define PLATFORM_WINDOWS "Windows"
    #define PLATFORM_CURRENT PLATFORM_WINDOWS
#elif defined(__linux__)
    #define PLATFORM_LINUX "Linux"
    #define PLATFORM_CURRENT PLATFORM_LINUX
#elif defined(__APPLE__)
    #define PLATFORM_MACOS "MacOS"
    #define PLATFORM_CURRENT PLATFORM_MACOS
#elif defined(__EMSCRIPTEN__)
    #define PLATFORM_EMSCRIPTEN "Emscripten"
    #define PLATFORM_CURRENT PLATFORM_EMSCRIPTEN
#else
    #error Unknown platform!
#endif


/*
    Configuration
*/

#if defined(CMAKE_CONFIG_DEBUG)
    #define CONFIG_DEBUG "Debug"
    #define CONFIG_CURRENT CONFIG_DEBUG
#elif defined(CMAKE_CONFIG_DEVELOP)
    #define CONFIG_DEVELOP "Develop"
    #define CONFIG_CURRENT CONFIG_DEVELOP
#elif defined(CMAKE_CONFIG_RELEASE)
    #define CONFIG_RELEASE "Release"
    #define CONFIG_CURRENT CONFIG_RELEASE
#else
    #error Unknown configuration!
#endif
