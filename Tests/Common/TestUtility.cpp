/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <doctest/doctest.h>
#include <Common/Utility.hpp>

TEST_CASE("Utility")
{
    SUBCASE("Clamp")
    {
        CHECK_EQ(Common::Clamp(0, 1, 2), 1);
        CHECK_EQ(Common::Clamp(3, 1, 2), 2);
        CHECK_EQ(Common::Clamp(0.0f, 1.0f, 2.0f), 1.0f);
        CHECK_EQ(Common::Clamp(3.0f, 1.0f, 2.0f), 2.0f);
    }

    SUBCASE("Static array size")
    {
        int array[4] = { 0 };

        CHECK_EQ(Common::StaticArraySize(array), 4);
    }

    SUBCASE("Numeric cast")
    {
        uint64_t source = static_cast<uint64_t>(std::numeric_limits<uint32_t>::max());
        uint32_t target = Common::NumericalCast<uint32_t>(source);

        CHECK_EQ(target, 4294967295);
    }

    SUBCASE("Clear container")
    {
        std::vector<int> elements;
        elements.push_back(234);
        elements.push_back(243);
        elements.push_back(432);
        elements.push_back(342);
        Common::ClearContainer(elements);

        CHECK_EQ(elements.size(), 0);
        CHECK_EQ(elements.capacity(), 0);
    }

    SUBCASE("Get text file content")
    {
        std::string text = Common::GetTextFileContent(TESTS_DIRECTORY "/Resources/TextFile.txt");

        CHECK_EQ(text, "Hello world!");
    }

    SUBCASE("Get binary file content")
    {
        std::vector<char> binary = Common::GetBinaryFileContent(TESTS_DIRECTORY "/Resources/BinaryFile.bin");

        CHECK_EQ(binary.size(), 4);
        CHECK_EQ(binary[0], '\0');
        CHECK_EQ(binary[1], 'H');
        CHECK_EQ(binary[2], 'i');
        CHECK_EQ(binary[3], '\0');
    }

    SUBCASE("String lower case")
    {
        std::string text = "HeLLo WoNDERfUL WoRlD! :)";

        CHECK_EQ(Common::StringLowerCase(text), "hello wonderful world! :)");
    }

    SUBCASE("String tokenize")
    {
        std::string text = "Hello wonderful world! :)";
        std::vector<std::string> tokens = Common::StringTokenize(text, ' ');

        CHECK_EQ(tokens.size(), 4);
        CHECK_EQ(tokens[0], "Hello");
        CHECK_EQ(tokens[1], "wonderful");
        CHECK_EQ(tokens[2], "world!");
        CHECK_EQ(tokens[3], ":)");
    }

    SUBCASE("String trim")
    {
        std::string text = "   @)#($*%&^  hello world !   )*(&$^%#@     ";

        CHECK_EQ(Common::StringTrimLeft(text, " @#$%^&*()"), "hello world !   )*(&$^%#@     ");
        CHECK_EQ(Common::StringTrimRight(text, " @#$%^&*()"), "   @)#($*%&^  hello world !");
        CHECK_EQ(Common::StringTrim(text, " @#$%^&*()"), "hello world !");
    }

    SUBCASE("String hash")
    {
        CHECK_NE(Common::StringHash<uint32_t>("Armored orange"), 0);
        CHECK_NE(Common::StringHash<uint64_t>("Naked banana"),
            Common::StringHash<uint64_t>("Dressed apple"));
    }

    SUBCASE("CRC")
    {
        uint8_t dataFirst[4] = { '2', '0', '3', '5' };
        uint8_t dataSecond[4] = { '1', '9', '4', '5' };

        CHECK_NE(Common::CalculateCrc32(0, &dataFirst[0], Common::StaticArraySize(dataFirst)), 0);
        CHECK_NE(Common::CalculateCrc32(0, &dataFirst[0], Common::StaticArraySize(dataFirst)),
            Common::CalculateCrc32(0, &dataSecond[0], Common::StaticArraySize(dataSecond)));
    }

    SUBCASE("Reorder with indices")
    {
        SUBCASE("Zero elements")
        {
            std::vector<std::string> array;
            std::vector<std::size_t> indices;

            CHECK(Common::ReorderWithIndices(array, indices));
        }

        SUBCASE("One elements")
        {
            std::vector<std::string> array = { "First" };
            std::vector<std::size_t> indices = { 0 };

            CHECK(Common::ReorderWithIndices(array, indices));
            CHECK_EQ(array[0], "First");
        }

        SUBCASE("Two elements")
        {
            std::vector<std::string> array = { "Second", "First" };
            std::vector<std::size_t> indices = { 1, 0 };

            CHECK(Common::ReorderWithIndices(array, indices));
            CHECK_EQ(array[0], "First");
            CHECK_EQ(array[1], "Second");
        }

        SUBCASE("Three elements")
        {
            std::vector<std::string> array = { "First", "Third", "Second" };
            std::vector<std::size_t> indices = { 0, 2, 1 };

            CHECK(Common::ReorderWithIndices(array, indices));
            CHECK_EQ(array[0], "First");
            CHECK_EQ(array[1], "Second");
            CHECK_EQ(array[2], "Third");
        }

        SUBCASE("Four elements")
        {
            std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
            std::vector<std::size_t> indices = { 1, 3, 2, 0 };

            CHECK(Common::ReorderWithIndices(array, indices));
            CHECK_EQ(array[0], "First");
            CHECK_EQ(array[1], "Second");
            CHECK_EQ(array[2], "Third");
            CHECK_EQ(array[3], "Fourth");
        }

        SUBCASE("Non-matching sizes")
        {
            std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
            std::vector<std::size_t> indices = { 1, 3, 2 };

            CHECK_FALSE(Common::ReorderWithIndices(array, indices));
            CHECK_EQ(array[0], "Fourth");
            CHECK_EQ(array[1], "First");
            CHECK_EQ(array[2], "Third");
            CHECK_EQ(array[3], "Second");
        }

        SUBCASE("Out of bounds indices")
        {
            std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
            std::vector<std::size_t> indices = { 1, 4, 2, 0 };

            CHECK_FALSE(Common::ReorderWithIndices(array, indices));
            CHECK_EQ(array[0], "Fourth");
            CHECK_EQ(array[1], "First");
            CHECK_EQ(array[2], "Third");
            CHECK_EQ(array[3], "Second");
        }
    }
}
