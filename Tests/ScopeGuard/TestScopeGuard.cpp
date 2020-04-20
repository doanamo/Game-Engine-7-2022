/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include <Common/ScopeGuard.hpp>
#include <TestHelpers.hpp>

bool TestMakeScopeGuard()
{
    int* i = nullptr;

    {
        i = new int(4);
        auto cleanup = Common::MakeScopeGuard([&]()
        {
            delete i;
            i = nullptr;
        });
    }

    TEST_EQ(i, nullptr);

    return true;
}

bool TestScopeGuardMacro()
{
    int* i = nullptr;

    {
        i = new int(4);
        SCOPE_GUARD(delete i; i = nullptr);
    }

    TEST_EQ(i, nullptr);

    return true;
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

    TEST_EQ(i, nullptr);
    TEST_EQ(j, nullptr);

    return true;
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

    TEST_EQ(i, nullptr);

    return true;
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

    TEST_EQ(i, nullptr);
    TEST_EQ(j, nullptr);

    return true;
}

int main()
{
    TEST_RUN(TestMakeScopeGuard);
    TEST_RUN(TestScopeGuardMacro);
    TEST_RUN(TestConditionalScopeGuardMacro);
    TEST_RUN(TestBracedScopeGuardMacro);
    TEST_RUN(TestConditionalBracedScopeGuardMacro);
}
