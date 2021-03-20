/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <Common/ReflectionGenerated.hpp>

TEST_CASE("Register reflection")
{
    Reflection::Generated::RegisterModuleCommon();
}
