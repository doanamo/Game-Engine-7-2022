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

    // Override new operator to store additional information about allocations.
    #define CHECKED_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
    #define CHECKED_NEW new
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
    #include <signal.h>
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
    Used as sanity check to guard against programming errors.

    Behavior in different build configurations:
    - Debug: Evaluates expression and triggers abort
    - Release: Stripped out completely (does not evaluate expression)

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
            LOG_FATAL("Assertion failed: " DEBUG_STRINGIFY(expression) \
                " - " message, ## __VA_ARGS__); \
            DEBUG_ABORT(); \
        }
#else
    #define ASSERT_SIMPLE(expression) ((void)0)
    #define ASSERT_MESSAGE(expression, message, ...) ((void)0) 
#endif

#define ASSERT_DEDUCE(arg1, arg2, arg3, arg4, arg5, arg6, arg7, ...) arg7
#define ASSERT_CHOOSER(...) DEBUG_EXPAND_MACRO(ASSERT_DEDUCE(__VA_ARGS__, ASSERT_MESSAGE, \
    ASSERT_MESSAGE, ASSERT_MESSAGE, ASSERT_MESSAGE, ASSERT_MESSAGE, ASSERT_SIMPLE))
#define ASSERT(...) DEBUG_EXPAND_MACRO(ASSERT_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

/*
    Verify Macro

    Ensures that given expression is true in all configurations.
    Used to safeguard against errors we choose not to handle.

    Behavior in different build configurations:
    - Debug: Evaluates expression and triggers abort
    - Release: Evaluates expression and triggers abort

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
        LOG_FATAL("Verification failed: " DEBUG_STRINGIFY(expression) \
            " - " message, ## __VA_ARGS__); \
        DEBUG_ABORT(); \
    }

#define VERIFY_DEDUCE(arg1, arg2, arg3, arg4, arg5, arg6, arg7, ...) arg7
#define VERIFY_CHOOSER(...) DEBUG_EXPAND_MACRO(VERIFY_DEDUCE(__VA_ARGS__, VERIFY_MESSAGE, \
    VERIFY_MESSAGE, VERIFY_MESSAGE, VERIFY_MESSAGE, VERIFY_MESSAGE, VERIFY_SIMPLE))
#define VERIFY(...) DEBUG_EXPAND_MACRO(VERIFY_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

/*
    Check Macro

    Checks if given expression is true in debug configurations.
    Used to print warnings and when we want to continue execution
    despite some possible error.

    Behavior in different build configurations:
    - Debug: Evaluates expression and logs warning
    - Release: Evaluates expression only

    Example usage:
        CHECK(m_initialized);
        CHECK(instance != nullptr, "Invalid instance.");
*/

#ifndef NDEBUG
    #define CHECK_SIMPLE(expression) \
        if(expression) { } else \
        { \
            LOG_WARNING("Check failed: " DEBUG_STRINGIFY(expression)); \
        }

    #define CHECK_MESSAGE(expression, message, ...) \
        if(expression) { } else \
        { \
            LOG_WARNING("Check failed: " DEBUG_STRINGIFY(expression) \
                " - " message, ## __VA_ARGS__); \
        }

    #define CHECK_OR_RETURN(expression, value, message, ...) \
        if(expression) { } else \
        { \
            LOG_WARNING("Check failed: " DEBUG_STRINGIFY(expression) \
                " - " message, ## __VA_ARGS__); \
            return value; \
        }
#else
    #define CHECK_SIMPLE(expression) if(expression) { } else { ((void)0); }
    #define CHECK_MESSAGE(expression, message, ...) if(expression) { } else { ((void)0); } 
    #define CHECK_OR_RETURN(expression, value, message, ...) \
        if(expression) { } else { return value; }
#endif

#define CHECK_DEDUCE(arg1, arg2, arg3, arg4, arg5, arg6, arg7, ...) arg7
#define CHECK_CHOOSER(...) DEBUG_EXPAND_MACRO(CHECK_DEDUCE(__VA_ARGS__, CHECK_MESSAGE, \
    CHECK_MESSAGE, CHECK_MESSAGE, CHECK_MESSAGE, CHECK_MESSAGE, CHECK_SIMPLE))
#define CHECK(...) DEBUG_EXPAND_MACRO(CHECK_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

/*
    Argument Handling
*/

#define ASSERT_ARGUMENT(expression) ASSERT_MESSAGE(expression, "Invalid argument!")
#define VERIFY_ARGUMENT(expression) VERIFY_MESSAGE(expression, "Invalid argument!")
#define CHECK_ARGUMENT(expression) CHECK_MESSAGE(expression, "Invalid argument!");

#define CHECK_ARGUMENT_OR_RETURN(expression, value) \
    CHECK_OR_RETURN(expression, value, "Invalid argument!");
