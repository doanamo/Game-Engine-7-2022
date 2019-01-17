/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include <Common/ScopeGuard.hpp>

bool TestMakeScopeGuard()
{
    int* i = nullptr;

    {
        i = new int(4);
        auto cleanup = MakeScopeGuard([&]()
        {
            delete i;
            i = nullptr;
        });
    }

    return i == nullptr;
}

bool TestScopeGuardMacro()
{
    int* i = nullptr;

    {
        i = new int(4);
        SCOPE_GUARD(delete i; i = nullptr);
    }

    return i == nullptr;
}

bool TestConditionalScopeGuardMacro()
{
    int* i = nullptr;

    {
        i = new int(4);
        bool condition = true;
        SCOPE_GUARD_IF(condition, delete i; i = nullptr);
    }

    int* j = nullptr;

    {
        bool condition = false;
        SCOPE_GUARD_IF(condition, j = new int(4));
    }

    return i == nullptr && j == nullptr;
}

bool TestBracedScopeGuardMacro()
{
    int* i = nullptr;

    {
        i = new int(4);
        SCOPE_GUARD_BEGIN();
        {
            delete i;
            i = nullptr;
        }
        SCOPE_GUARD_END();
    }

    return i == nullptr;
}

bool TestConditionalBracedScopeGuardMacro()
{
    int* i = nullptr;

    {
        i = new int(4);
        bool condition = true;
        SCOPE_GUARD_BEGIN(condition);
        {
            delete i;
            i = nullptr;
        }
        SCOPE_GUARD_END();
    }

    int* j = nullptr;

    {
        bool condition = false;
        SCOPE_GUARD_BEGIN(condition);
        {
            j = new int(4);
        }
        SCOPE_GUARD_END();
    }

    return i == nullptr && j == nullptr;
}

int main()
{
    if(!TestMakeScopeGuard())
        return 1;

    if(!TestScopeGuardMacro())
        return 1;

    if(!TestConditionalScopeGuardMacro())
        return 1;

    if(!TestBracedScopeGuardMacro())
        return 1;

    if(!TestConditionalBracedScopeGuardMacro())
        return 1;

    return 0;
}
