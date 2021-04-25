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
    SUBCASE("Empty")
    {
        Common::Name nameEmpty;
        CHECK_EQ(nameEmpty, Common::StringHash<Common::Name::HashType>(""));
    }

    SUBCASE("Basic")
    {
        Common::Name nameOne = NAME_CONSTEXPR("One");
        Common::Name nameTwo = NAME_CONSTEXPR("Two");

        CHECK_NE(nameOne, nameTwo);
        CHECK_EQ(nameOne, Common::Name("One"));
        CHECK_EQ(nameTwo, Common::Name("Two"));

        SUBCASE("Copy")
        {
            Common::Name nameOneCopy;
            nameOneCopy = nameOne;
            CHECK_EQ(nameOneCopy, nameOne);

            Common::Name nameTwoCopy(nameTwo);
            CHECK_EQ(nameTwoCopy, nameTwo);
        }

        SUBCASE("From identifier")
        {
            Common::Name NameIdentifier(Common::StringHash<Common::Name::HashType>("One"));

            CHECK_EQ(nameOne, NameIdentifier);
            CHECK_NE(nameTwo, NameIdentifier);
        }
    }

    SUBCASE("Hash")
    {
        Common::Name nameTest = NAME_CONSTEXPR("TestNameType");
        CHECK_EQ(nameTest.GetHash(), Reflection::StaticType<TestNameType>().Identifier);
    }

    SUBCASE("String")
    {
        std::string string = Common::Name("TestNameType").GetString();

#ifdef NAME_REGISTRY_ENABLED
        std::string compare = "TestNameType";
#else
        std::stringstream stream;
        stream << "{";
        stream << Common::StringHash<Common::Name::HashType>("TestNameType");
        stream << "}";
        std::string compare = stream.str();
#endif

        CHECK_EQ(string, compare);
    }
}
