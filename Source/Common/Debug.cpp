/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Debug.hpp"

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
