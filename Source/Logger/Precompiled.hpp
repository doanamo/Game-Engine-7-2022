/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Precompiled Header
*/

#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <mutex>

#ifdef WIN32
    #define WIN32_LEAD_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#endif

#include <fmt/core.h>
#include <fmt/chrono.h>
