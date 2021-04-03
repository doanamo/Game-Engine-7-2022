/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <doctest/doctest.h>
#include <Common/Name.hpp>
#include <Reflection/Reflection.hpp>

class TestNameType
{
};

REFLECTION_TYPE(TestNameType);

TEST_CASE("Name")
{
    Common::Name nameOne = "One";
    Common::Name nameTwo = "Two";

    CHECK_NE(nameOne, nameTwo);
    CHECK_EQ(nameOne, Common::Name("One"));
    CHECK_EQ(nameTwo, Common::Name("Two"));

    CHECK_EQ(Common::Name("TestNameType").GetHash(),
        Reflection::StaticType<TestNameType>().Identifier);
}
