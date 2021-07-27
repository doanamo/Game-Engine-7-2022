/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#define DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
#include <doctest/doctest.h>
#include <Common/ScopeGuard.hpp>

DOCTEST_TEST_CASE("Scope Guard")
{
    int* ptr = nullptr;

    DOCTEST_SUBCASE("Make using lambda")
    {
        ptr = new int(4);
        auto cleanup = Common::MakeScopeGuard([&]()
        {
            delete ptr;
            ptr = nullptr;
        });
    }

    DOCTEST_SUBCASE("Make regular using macro")
    {
        ptr = new int(4);
        SCOPE_GUARD([&ptr]
        {
            delete ptr;
            ptr = nullptr;
        });
    }

    DOCTEST_SUBCASE("Make regular using braced macros")
    {
        ptr = new int(4);
        SCOPE_GUARD([&ptr]
        {
            delete ptr;
            ptr = nullptr;
        });
    }

    DOCTEST_CHECK(ptr == nullptr);
}
