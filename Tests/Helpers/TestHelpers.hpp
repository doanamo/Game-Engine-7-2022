/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include <Common/Debug.hpp>
#include <Logger/Logger.hpp>

/*
    Helper Macros
*/

#define TEST_VOID(value) (value)

#define TEST_RUN(function, ...) \
    if((function(__VA_ARGS__)) == false) \
        return 1;

#define TEST_TRUE(value) \
    if((value) != true) \
    { \
        DEBUG_BREAK_IF_ATTACHED(); \
        return false; \
    }

#define TEST_FALSE(value) \
    if((value) != false) \
    { \
        DEBUG_BREAK_IF_ATTACHED(); \
        return false; \
    }

#define TEST_EQ(left, right) \
    if(!((left) == (right))) \
    { \
        DEBUG_BREAK_IF_ATTACHED(); \
        return false; \
    }

#define TEST_NEQ(left, right) \
    if(!((left) != (right))) \
    { \
        DEBUG_BREAK_IF_ATTACHED(); \
        return false; \
    }
