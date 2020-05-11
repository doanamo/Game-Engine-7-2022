/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Common/Debug.hpp"

#ifdef WIN32
    #define WIN32_LEAD_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#endif

void Debug::Initialize()
{
    // Enable debug memory allocator and memory leak detection.
    #if defined(WIN32) && !defined(NDEBUG)
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);
    #endif
}

bool Debug::IsDebuggerAttached()
{
#ifdef WIN32
    return IsDebuggerPresent();
#else
    // Not implemented for other platforms.
    return false;
#endif
}
