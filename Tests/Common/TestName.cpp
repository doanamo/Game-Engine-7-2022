/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#define DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
#include <doctest/doctest.h>

#include <sstream>
#include <Common/Name.hpp>
#include <Reflection/Reflection.hpp>

class TestNameType
{
};

REFLECTION_TYPE(TestNameType);

DOCTEST_TEST_CASE("Name")
{
    DOCTEST_SUBCASE("Empty")
    {
        Common::Name nameEmpty;
        DOCTEST_CHECK_EQ(nameEmpty, Common::StringHash<Common::Name::HashType>(""));
    }

    DOCTEST_SUBCASE("Basic")
    {
        Common::Name nameOne = NAME_CONSTEXPR("One");
        Common::Name nameTwo = NAME_CONSTEXPR("Two");

        DOCTEST_CHECK_NE(nameOne, nameTwo);
        DOCTEST_CHECK_EQ(nameOne, Common::Name("One"));
        DOCTEST_CHECK_EQ(nameTwo, Common::Name("Two"));

        DOCTEST_SUBCASE("Copy")
        {
            Common::Name nameOneCopy;
            nameOneCopy = nameOne;
            DOCTEST_CHECK_EQ(nameOneCopy, nameOne);

            Common::Name nameTwoCopy(nameTwo);
            DOCTEST_CHECK_EQ(nameTwoCopy, nameTwo);
        }

        DOCTEST_SUBCASE("From identifier")
        {
            Common::Name NameIdentifier(Common::StringHash<Common::Name::HashType>("One"));

            DOCTEST_CHECK_EQ(nameOne, NameIdentifier);
            DOCTEST_CHECK_NE(nameTwo, NameIdentifier);
        }
    }

    DOCTEST_SUBCASE("Hash")
    {
        Common::Name nameTest = NAME_CONSTEXPR("TestNameType");
        DOCTEST_CHECK_EQ(nameTest.GetHash(), Reflection::StaticType<TestNameType>().Identifier);
    }

    DOCTEST_SUBCASE("String")
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

        DOCTEST_CHECK_EQ(string, compare);
    }
}
