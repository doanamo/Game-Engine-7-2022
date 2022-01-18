/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Precompiled Header
*/

#include <cassert>
#include <fstream>
#include <mutex>
#include <chrono>

#include "Common/Common.hpp"

#ifdef PLATFORM_WINDOWS
    #define WIN32_LEAD_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#endif

#ifdef PLATFORM_LINUX
    #include <sys/types.h>
    #include <unistd.h>
#endif
