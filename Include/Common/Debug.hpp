/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <iostream>

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

#if defined(LOGGER_ENABLED)
    #define DEBUG_PRINT(message) \
        Logger::ScopedMessage(Logger::GetGlobalSink()).
        SetSource(__FILE__).SetLine(__LINE__) << message;
#else
    #define DEBUG_PRINT(message) \
        std::cout << message << " {" << __FILE__ << ":" << __LINE__ << "}";
#endif

/*
    Assert Macros

    Ensures that a given expression is true.
    Use as a sanity check to guard against programming errors.
    
    Behaviour in different build configurations:
    - Debug: Triggers a breakpoint
    - Release: Check is stripped
    
    Example usage:
        ASSERT(m_initialized);
        ASSERT(instance != nullptr, "Invalid instance.");
*/

#ifndef NDEBUG
    #define ASSERT_PRINT(expression, message) \
        DEBUG_PRINT("Assertion failed: " ## expression ## " - " ## message)

    #define ASSERT_SIMPLE(expression)           \
        if(!(expression))                       \
        {                                       \
            ASSERT_PRINT(#expression);          \
            __debugbreak();                     \
        }

    #define ASSERT_MESSAGE(expression, message) \
        if(!(expression))                       \
        {                                       \
            ASSERT_PRINT(#expression, message); \
            __debugbreak();                     \
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

    Behaviour in different build configurations:
    - Debug: Triggers a breakpoint
    - Release: Triggers a breakpoint
    
    Example usage:
        VERIFY(m_initialized);
        VERIFY(instance != nullptr, "Invalid instance.");
*/

#define VERIFY_PRINT(expression, message) \
    DEBUG_PRINT("Verify failed: " ## expression ## " - " ## message)

#define VERIFY_SIMPLE(expression)           \
    if(!(expression))                       \
    {                                       \
        VERIFY_PRINT(#expression);          \
        __debugbreak();                     \
    }

#define VERIFY_MESSAGE(expression, message) \
    if(!(expression))                       \
    {                                       \
        VERIFY_PRINT(#expression, message); \
        __debugbreak();                     \
    }

#define VERIFY_DEDUCE(arg1, arg2, arg3, ...) arg3
#define VERIFY_CHOOSER(...) DEBUG_EXPAND_MACRO(VERIFY_DEDUCE(__VA_ARGS__, VERIFY_MESSAGE, VERIFY_SIMPLE))

#define VERIFY(...) DEBUG_EXPAND_MACRO(VERIFY_CHOOSER(__VA_ARGS__)(__VA_ARGS__))
