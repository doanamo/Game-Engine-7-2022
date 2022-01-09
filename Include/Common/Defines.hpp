/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

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
    #error Unknown configuration type!
#endif
