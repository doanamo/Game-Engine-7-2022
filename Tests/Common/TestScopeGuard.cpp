/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <doctest/doctest.h>
#include <Common/ScopeGuard.hpp>

TEST_CASE("Scope Guard")
{
    int* ptr = nullptr;

    SUBCASE("Make using lambda")
    {
        ptr = new int(4);
        auto cleanup = Common::MakeScopeGuard([&]()
        {
            delete ptr;
            ptr = nullptr;
        });
    }

    SUBCASE("Make regular using macro")
    {
        ptr = new int(4);
        SCOPE_GUARD(delete ptr; ptr = nullptr);
    }

    SUBCASE("Make regular using braced macros")
    {
        ptr = new int(4);

        SCOPE_GUARD_BEGIN();
        {
            delete ptr;
            ptr = nullptr;
        }
        SCOPE_GUARD_END();
    }

    SUBCASE("Make conditional using macro")
    {
        ptr = new int(4);
        SCOPE_GUARD_IF(false, ptr = new int(7));
        SCOPE_GUARD_IF(true, delete ptr; ptr = nullptr);
    }

    SUBCASE("Make conditional using braced macros")
    {
        ptr = new int(4);

        SCOPE_GUARD_BEGIN(false);
        {
            ptr = new int(7);
        }
        SCOPE_GUARD_END();

        SCOPE_GUARD_BEGIN(true);
        {
            delete ptr;
            ptr = nullptr;
        }
        SCOPE_GUARD_END();
    }

    CHECK(ptr == nullptr);
}
