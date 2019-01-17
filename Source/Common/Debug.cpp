/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Common/Debug.hpp"

void Debug::Initialize()
{
    // Enable debug memory allocator and memory leak detection.
    #if defined(WIN32) && !defined(NDEBUG)
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif
}
