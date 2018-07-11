/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <iostream>
#include "Logger/Logger.hpp"

/*
    Platform Defines
*/

// Windows specific defines.
#if defined(WIN32) && !defined(NDEBUG)
    #define _CRTDBG_MAP_ALLOC
    #define _CRTDBG_MAP_ALLOC_NEW
    #include <stdlib.h>
    #include <crtdbg.h>

    // Override new operator (breaks placement new).
    #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
    #define new DEBUG_NEW
#endif

/*
    Debug Utility
*/

namespace Debug
{
    // Initializes debug routines.
    void Initialize();
}

/*
    Debug Macros
*/

#define DEBUG_EXPAND_MACRO(x) x
#define DEBUG_STRINGIFY(expression) #expression
#define DEBUG_BREAKPOINT() __debugbreak();
#define DEBUG_PRINT(message) LOG_DEBUG() << message;

/*
    Assert Macros

    Ensures that a given expression is true.
    Use as a sanity check to guard against programming errors.
    
    Behavior in different build configurations:
    - Debug: Triggers a breakpoint
    - Release: Check is stripped
    
    Example usage:
        ASSERT(m_initialized);
        ASSERT(instance != nullptr, "Invalid instance.");
*/

#ifndef NDEBUG
    #define ASSERT_SIMPLE(expression) \
        if(!(expression)) \
        { \
            DEBUG_PRINT("Assertion failed: " DEBUG_STRINGIFY(expression)); \
            DEBUG_BREAKPOINT(); \
        }

    #define ASSERT_MESSAGE(expression, message) \
        if(!(expression)) \
        { \
            DEBUG_PRINT("Assertion failed: " DEBUG_STRINGIFY(expression) " - " message); \
            DEBUG_BREAKPOINT(); \
        }
#else
    #define ASSERT_SIMPLE(expression) ((void)0)
    #define ASSERT_MESSAGE(expression, message) ((void)0) 
#endif

#define ASSERT_DEDUCE(arg1, arg2, arg3, ...) arg3
#define ASSERT_CHOOSER(...) DEBUG_EXPAND_MACRO(ASSERT_DEDUCE(__VA_ARGS__, ASSERT_MESSAGE, ASSERT_SIMPLE))

#define ASSERT(...) DEBUG_EXPAND_MACRO(ASSERT_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

/*
    Verify Macro
    
    Ensures that a given expression is true.
    Use as a safety measure in case of runtime errors.

    Behavior in different build configurations:
    - Debug: Triggers a breakpoint
    - Release: Triggers a breakpoint
    
    Example usage:
        VERIFY(m_initialized);
        VERIFY(instance != nullptr, "Invalid instance.");
*/

#define VERIFY_SIMPLE(expression) \
    if(!(expression)) \
    { \
        DEBUG_PRINT("Verification failed: " DEBUG_STRINGIFY(expression)) \
        DEBUG_BREAKPOINT();                 \
    }

#define VERIFY_MESSAGE(expression, message) \
    if(!(expression)) \
    { \
        DEBUG_PRINT("Verification failed: " DEBUG_STRINGIFY(expression) " - " message) \
        DEBUG_BREAKPOINT(); \
    }

#define VERIFY_DEDUCE(arg1, arg2, arg3, ...) arg3
#define VERIFY_CHOOSER(...) DEBUG_EXPAND_MACRO(VERIFY_DEDUCE(__VA_ARGS__, VERIFY_MESSAGE, VERIFY_SIMPLE))

#define VERIFY(...) DEBUG_EXPAND_MACRO(VERIFY_CHOOSER(__VA_ARGS__)(__VA_ARGS__))
