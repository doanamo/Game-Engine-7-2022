/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Logger/Logger.hpp>

/*
    Platform Defines
*/

#if defined(WIN32) && !defined(NDEBUG)
    #define _CRTDBG_MAP_ALLOC
    #define _CRTDBG_MAP_ALLOC_NEW
    #include <stdlib.h>
    #include <crtdbg.h>

    // Override new operator to store additional information about an allocation.
    #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
    #define DEBUG_NEW new
#endif

/*
    Debug Utility
*/

namespace Debug
{
    void Initialize();
    bool IsDebuggerAttached();
}

/*
    Debug Macros
*/

#define DEBUG_EXPAND_MACRO(x) x
#define DEBUG_STRINGIFY(expression) #expression

#ifdef _MSC_VER
    #define DEBUG_BREAK() __debugbreak()
#else
    #include <sys/signal.h>
    #define DEBUG_BREAK() raise(SIGTRAP)
#endif

#define DEBUG_ABORT() \
    DEBUG_BREAK(); \
    abort()

#ifdef WIN32
    #define DEBUG_BREAK_IF_ATTACHED() \
        if(Debug::IsDebuggerAttached()) \
        { \
            DEBUG_BREAK(); \
        }
#else
    #define DEBUG_BREAK_IF_ATTACHED() ((void)0)
#endif

/*
    Assert Macros

    Ensures that given expression is true in debug configuration.
    Used as a sanity check to guard against programming errors.

    Behavior in different build configurations:
    - Debug: Triggers abort
    - Release: Check is stripped

    Example usage:
        ASSERT(m_initialized);
        ASSERT(instance != nullptr, "Invalid instance.");
*/

#ifndef NDEBUG
    #define ASSERT_SIMPLE(expression) \
        if(expression) { } else \
        { \
            LOG_FATAL("Assertion failed: " DEBUG_STRINGIFY(expression)); \
            DEBUG_ABORT(); \
        }

    #define ASSERT_MESSAGE(expression, message, ...) \
        if(expression) { } else \
        { \
            LOG_FATAL("Assertion failed: " DEBUG_STRINGIFY(expression) " - " message, ## __VA_ARGS__); \
            DEBUG_ABORT(); \
        }
#else
    #define ASSERT_SIMPLE(expression) ((void)0)
    #define ASSERT_MESSAGE(expression, message) ((void)0) 
#endif

#define ASSERT_DEDUCE(arg1, arg2, arg3, arg4, arg5, arg6, arg7, ...) arg7
#define ASSERT_CHOOSER(...) DEBUG_EXPAND_MACRO(ASSERT_DEDUCE(__VA_ARGS__, ASSERT_MESSAGE, ASSERT_MESSAGE, ASSERT_MESSAGE, ASSERT_MESSAGE, ASSERT_MESSAGE, ASSERT_SIMPLE))
#define ASSERT(...) DEBUG_EXPAND_MACRO(ASSERT_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

/*
    Verify Macro

    Ensures that given expression is true in all configurations.
    Used to safeguard against errors we choose not to handle.

    Behavior in different build configurations:
    - Debug: Triggers abort
    - Release: Triggers abort

    Example usage:
        VERIFY(m_initialized);
        VERIFY(instance != nullptr, "Invalid instance.");
*/

#define VERIFY_SIMPLE(expression) \
    if(expression) { } else \
    { \
        LOG_FATAL("Verification failed: " DEBUG_STRINGIFY(expression)); \
        DEBUG_ABORT(); \
    }

#define VERIFY_MESSAGE(expression, message, ...) \
    if(expression) { } else \
    { \
        LOG_FATAL("Verification failed: " DEBUG_STRINGIFY(expression) " - " message, ## __VA_ARGS__); \
        DEBUG_ABORT(); \
    }

#define VERIFY_DEDUCE(arg1, arg2, arg3, arg4, arg5, arg6, arg7, ...) arg7
#define VERIFY_CHOOSER(...) DEBUG_EXPAND_MACRO(VERIFY_DEDUCE(__VA_ARGS__, VERIFY_MESSAGE, VERIFY_MESSAGE, VERIFY_MESSAGE, VERIFY_MESSAGE, VERIFY_MESSAGE, VERIFY_SIMPLE))
#define VERIFY(...) DEBUG_EXPAND_MACRO(VERIFY_CHOOSER(__VA_ARGS__)(__VA_ARGS__))
