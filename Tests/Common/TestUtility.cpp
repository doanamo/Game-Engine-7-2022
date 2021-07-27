/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#define DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
#include <doctest/doctest.h>
#include <Common/Utility.hpp>

DOCTEST_TEST_CASE("Utility")
{
    DOCTEST_SUBCASE("Clamp")
    {
        DOCTEST_CHECK_EQ(Common::Clamp(0, 1, 2), 1);
        DOCTEST_CHECK_EQ(Common::Clamp(3, 1, 2), 2);
        DOCTEST_CHECK_EQ(Common::Clamp(0.0f, 1.0f, 2.0f), 1.0f);
        DOCTEST_CHECK_EQ(Common::Clamp(3.0f, 1.0f, 2.0f), 2.0f);
    }

    DOCTEST_SUBCASE("Static array size")
    {
        int array[4] = { 0 };

        DOCTEST_CHECK_EQ(Common::StaticArraySize(array), 4);
    }

    DOCTEST_SUBCASE("Numeric cast")
    {
        uint64_t source = static_cast<uint64_t>(std::numeric_limits<uint32_t>::max());
        uint32_t target = Common::NumericalCast<uint32_t>(source);

        DOCTEST_CHECK_EQ(target, 4294967295);
    }

    DOCTEST_SUBCASE("Clear container")
    {
        std::vector<int> elements;
        elements.push_back(234);
        elements.push_back(243);
        elements.push_back(432);
        elements.push_back(342);
        Common::FreeContainer(elements);

        DOCTEST_CHECK_EQ(elements.size(), 0);
        DOCTEST_CHECK_EQ(elements.capacity(), 0);
    }

    DOCTEST_SUBCASE("Get text file content")
    {
        std::string text = Common::GetTextFileContent(TESTS_DIRECTORY "/Resources/TextFile.txt");

        DOCTEST_CHECK_EQ(text, "Hello world!");
    }

    DOCTEST_SUBCASE("Get binary file content")
    {
        std::vector<char> binary = Common::GetBinaryFileContent(TESTS_DIRECTORY "/Resources/BinaryFile.bin");

        DOCTEST_CHECK_EQ(binary.size(), 4);
        DOCTEST_CHECK_EQ(binary[0], '\0');
        DOCTEST_CHECK_EQ(binary[1], 'H');
        DOCTEST_CHECK_EQ(binary[2], 'i');
        DOCTEST_CHECK_EQ(binary[3], '\0');
    }

    DOCTEST_SUBCASE("String lower case")
    {
        std::string text = "HeLLo WoNDERfUL WoRlD! :)";

        DOCTEST_CHECK_EQ(Common::StringLowerCase(text), "hello wonderful world! :)");
    }

    DOCTEST_SUBCASE("String tokenize")
    {
        std::string text = "Hello wonderful world! :)";
        std::vector<std::string> tokens = Common::StringTokenize(text, ' ');

        DOCTEST_CHECK_EQ(tokens.size(), 4);
        DOCTEST_CHECK_EQ(tokens[0], "Hello");
        DOCTEST_CHECK_EQ(tokens[1], "wonderful");
        DOCTEST_CHECK_EQ(tokens[2], "world!");
        DOCTEST_CHECK_EQ(tokens[3], ":)");
    }

    DOCTEST_SUBCASE("String trim")
    {
        std::string text = "   @)#($*%&^  hello world !   )*(&$^%#@     ";

        DOCTEST_CHECK_EQ(Common::StringTrimLeft(text,
            " @#$%^&*()"), "hello world !   )*(&$^%#@     ");
        DOCTEST_CHECK_EQ(Common::StringTrimRight(text,
            " @#$%^&*()"), "   @)#($*%&^  hello world !");
        DOCTEST_CHECK_EQ(Common::StringTrim(text,
            " @#$%^&*()"), "hello world !");
    }

    DOCTEST_SUBCASE("String hash")
    {
        DOCTEST_CHECK_NE(Common::StringHash<uint32_t>("Armored orange"), 0);
        DOCTEST_CHECK_NE(Common::StringHash<uint64_t>("Naked banana"),
            Common::StringHash<uint64_t>("Dressed apple"));
    }

    DOCTEST_SUBCASE("CRC")
    {
        uint8_t dataFirst[4] = { '2', '0', '3', '5' };
        uint8_t dataSecond[4] = { '1', '9', '4', '5' };

        DOCTEST_CHECK_NE(Common::CalculateCrc32(0, &dataFirst[0],
            Common::StaticArraySize(dataFirst)), 0);
        DOCTEST_CHECK_NE(
            Common::CalculateCrc32(0, &dataFirst[0], Common::StaticArraySize(dataFirst)),
            Common::CalculateCrc32(0, &dataSecond[0], Common::StaticArraySize(dataSecond)));
    }

    DOCTEST_SUBCASE("Reorder with indices")
    {
        DOCTEST_SUBCASE("Zero elements")
        {
            std::vector<std::string> array;
            std::vector<std::size_t> indices;

            DOCTEST_CHECK(Common::ReorderWithIndices(array, indices));
        }

        DOCTEST_SUBCASE("One elements")
        {
            std::vector<std::string> array = { "First" };
            std::vector<std::size_t> indices = { 0 };

            DOCTEST_CHECK(Common::ReorderWithIndices(array, indices));
            DOCTEST_CHECK_EQ(array[0], "First");
        }

        DOCTEST_SUBCASE("Two elements")
        {
            std::vector<std::string> array = { "Second", "First" };
            std::vector<std::size_t> indices = { 1, 0 };

            DOCTEST_CHECK(Common::ReorderWithIndices(array, indices));
            DOCTEST_CHECK_EQ(array[0], "First");
            DOCTEST_CHECK_EQ(array[1], "Second");
        }

        DOCTEST_SUBCASE("Three elements")
        {
            std::vector<std::string> array = { "First", "Third", "Second" };
            std::vector<std::size_t> indices = { 0, 2, 1 };

            DOCTEST_CHECK(Common::ReorderWithIndices(array, indices));
            DOCTEST_CHECK_EQ(array[0], "First");
            DOCTEST_CHECK_EQ(array[1], "Second");
            DOCTEST_CHECK_EQ(array[2], "Third");
        }

        DOCTEST_SUBCASE("Four elements")
        {
            std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
            std::vector<std::size_t> indices = { 1, 3, 2, 0 };

            DOCTEST_CHECK(Common::ReorderWithIndices(array, indices));
            DOCTEST_CHECK_EQ(array[0], "First");
            DOCTEST_CHECK_EQ(array[1], "Second");
            DOCTEST_CHECK_EQ(array[2], "Third");
            DOCTEST_CHECK_EQ(array[3], "Fourth");
        }

        DOCTEST_SUBCASE("Non-matching sizes")
        {
            std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
            std::vector<std::size_t> indices = { 1, 3, 2 };

            DOCTEST_CHECK_FALSE(Common::ReorderWithIndices(array, indices));
            DOCTEST_CHECK_EQ(array[0], "Fourth");
            DOCTEST_CHECK_EQ(array[1], "First");
            DOCTEST_CHECK_EQ(array[2], "Third");
            DOCTEST_CHECK_EQ(array[3], "Second");
        }

        DOCTEST_SUBCASE("Out of bounds indices")
        {
            std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
            std::vector<std::size_t> indices = { 1, 4, 2, 0 };

            DOCTEST_CHECK_FALSE(Common::ReorderWithIndices(array, indices));
            DOCTEST_CHECK_EQ(array[0], "Fourth");
            DOCTEST_CHECK_EQ(array[1], "First");
            DOCTEST_CHECK_EQ(array[2], "Third");
            DOCTEST_CHECK_EQ(array[3], "Second");
        }
    }
}
