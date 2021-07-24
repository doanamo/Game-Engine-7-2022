/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Logger/Logger.hpp>

/*
    Platform Defines
*/

#if defined(WIN32) && !defined(NDEBUG)
    #define _CRTDBG_MAP_ALLOC
    #define _CRTDBG_MAP_ALLOC_NEW
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
    Assert Evaluate Macros

    Ensures that given expression is true in debug configuration.
    In contrary to regular assert, evaluates expression in release configuration.

    Behavior in different build configurations:
    - Debug: Evaluates expression and triggers abort
    - Release: Evaluates expression but does not trigger abort

    Example usage:
        ASSERT_EVALUATE(Function());
        ASSERT_EVALUATE(Function() != nullptr, "Invalid call.");
*/

#ifndef NDEBUG
    #define ASSERT_EVALUATE_SIMPLE(expression) \
            if(expression) { } else \
            { \
                LOG_FATAL("Assertion failed: " DEBUG_STRINGIFY(expression)); \
                DEBUG_ABORT(); \
            }

    #define ASSERT_EVALUATE_MESSAGE(expression, message, ...) \
            if(expression) { } else \
            { \
                LOG_FATAL("Assertion failed: " DEBUG_STRINGIFY(expression) \
                    " - " message, ## __VA_ARGS__); \
                DEBUG_ABORT(); \
            }
#else
    #define ASSERT_EVALUATE_SIMPLE(expression) (expression)
    #define ASSERT_EVALUATE_MESSAGE(expression, message, ...) (expression)
#endif

#define ASSERT_EVALUATE_DEDUCE(arg1, arg2, arg3, arg4, arg5, arg6, arg7, ...) arg7
#define ASSERT_EVALUATE_CHOOSER(...) DEBUG_EXPAND_MACRO(ASSERT_EVALUATE_DEDUCE(__VA_ARGS__, \
    ASSERT_EVALUATE_MESSAGE, ASSERT_EVALUATE_MESSAGE, ASSERT_EVALUATE_MESSAGE, \
    ASSERT_EVALUATE_MESSAGE, ASSERT_EVALUATE_MESSAGE, ASSERT_EVALUATE_SIMPLE))
#define ASSERT_EVALUATE(...) DEBUG_EXPAND_MACRO(ASSERT_EVALUATE_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

/*
    Assert Always Macro

    Ensures that given expression is true in all configurations.
    Used to safeguard against possible errors we choose not to handle.

    Behavior in different build configurations:
    - Debug: Evaluates expression and triggers abort
    - Release: Evaluates expression and triggers abort

    Example usage:
        ASSERT_ALWAYS(m_initialized);
        ASSERT_ALWAYS(instance != nullptr, "Invalid instance.");
*/

#define ASSERT_ALWAYS_SIMPLE(expression) \
    if(expression) { } else \
    { \
        LOG_FATAL("Assertion failed: " DEBUG_STRINGIFY(expression)); \
        DEBUG_ABORT(); \
    }

#define ASSERT_ALWAYS_MESSAGE(expression, message, ...) \
    if(expression) { } else \
    { \
        LOG_FATAL("Assertion failed: " DEBUG_STRINGIFY(expression) \
            " - " message, ## __VA_ARGS__); \
        DEBUG_ABORT(); \
    }

#define ASSERT_ALWAYS_DEDUCE(arg1, arg2, arg3, arg4, arg5, arg6, arg7, ...) arg7
#define ASSERT_ALWAYS_CHOOSER(...) DEBUG_EXPAND_MACRO(ASSERT_ALWAYS_DEDUCE(__VA_ARGS__, \
    ASSERT_ALWAYS_MESSAGE, ASSERT_ALWAYS_MESSAGE, ASSERT_ALWAYS_MESSAGE, \
    ASSERT_ALWAYS_MESSAGE, ASSERT_ALWAYS_MESSAGE, ASSERT_ALWAYS_SIMPLE))
#define ASSERT_ALWAYS(...) DEBUG_EXPAND_MACRO(ASSERT_ALWAYS_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

/*
    Assert Warn Macro

    Ensures that given expression is true in all configurations.
    Used to only print warnings when we want to continue execution.

    Behavior in different build configurations:
    - Debug: Evaluates expression and logs warning
    - Release: Evaluates expression and logs warning

    Example usage:
        ASSERT_WARN(m_initialized);
        ASSERT_WARN(instance != nullptr, "Invalid instance.");
*/

#define ASSERT_WARN_SIMPLE(expression) \
    if(expression) { } else \
    { \
        LOG_WARNING("Assertion failed: " DEBUG_STRINGIFY(expression)); \
    }

#define ASSERT_WARN_MESSAGE(expression, message, ...) \
    if(expression) { } else \
    { \
        LOG_WARNING("Assertion failed: " DEBUG_STRINGIFY(expression) \
            " - " message, ## __VA_ARGS__); \
    }

#define ASSERT_WARN_OR_RETURN(expression, value, message, ...) \
    if(expression) { } else \
    { \
        LOG_WARNING("Assertion failed: " DEBUG_STRINGIFY(expression) \
            " - " message, ## __VA_ARGS__); \
        return value; \
    }

#define ASSERT_WARN_DEDUCE(arg1, arg2, arg3, arg4, arg5, arg6, arg7, ...) arg7
#define ASSERT_WARN_CHOOSER(...) DEBUG_EXPAND_MACRO(ASSERT_WARN_DEDUCE(__VA_ARGS__, \
    ASSERT_WARN_MESSAGE, ASSERT_WARN_MESSAGE, ASSERT_WARN_MESSAGE, \
    ASSERT_WARN_MESSAGE, ASSERT_WARN_MESSAGE, ASSERT_WARN_SIMPLE))
#define ASSERT_WARN(...) DEBUG_EXPAND_MACRO(ASSERT_WARN_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

/*
    Argument Handling
*/

#define ASSERT_ARGUMENT(expression) ASSERT_MESSAGE(expression, "Invalid argument!")
#define ASSERT_ALWAYS_ARGUMENT(expression) ASSERT_ALWAYS_MESSAGE(expression, "Invalid argument!")
#define ASSERT_LOG_ARGUMENT(expression) ASSERT_WARN_MESSAGE(expression, "Invalid argument!");

#define CHECK_ARGUMENT_OR_RETURN(expression, value, ...) \
    ASSERT_WARN_OR_RETURN(expression, value, "Invalid argument!")
