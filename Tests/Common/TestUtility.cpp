/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <Common/Utility/Utility.hpp>

/*
    Utility
*/

TEST(Utility, Clamp)
{
    EXPECT_EQ(Common::Clamp(0, 1, 2), 1);
    EXPECT_EQ(Common::Clamp(3, 1, 2), 2);
    EXPECT_EQ(Common::Clamp(0.0f, 1.0f, 2.0f), 1.0f);
    EXPECT_EQ(Common::Clamp(3.0f, 1.0f, 2.0f), 2.0f);
}

TEST(Utility, StaticArraySize)
{
    int array[4] = { 0 };
    EXPECT_EQ(Common::StaticArraySize(array), 4);
}

TEST(Utility, NumericCast)
{
    uint64_t source = static_cast<uint64_t>(std::numeric_limits<uint32_t>::max());
    uint32_t target = Common::NumericalCast<uint32_t>(source);
    EXPECT_EQ(target, 4294967295);
}

TEST(Utility, ClearContainer)
{
    std::vector<int> elements;
    elements.push_back(234);
    elements.push_back(243);
    elements.push_back(432);
    elements.push_back(342);
    Common::FreeContainer(elements);

    EXPECT_EQ(elements.size(), 0);
    EXPECT_EQ(elements.capacity(), 0);
}

TEST(Utility, GetTextFileContent)
{
    std::string text = Common::GetTextFileContent(
        TESTS_DIRECTORY "/Resources/TextFile.txt");
    EXPECT_EQ(text, "Hello world!");
}

TEST(Utility, GetBinaryFileContent)
{
    std::vector<char> binary = Common::GetBinaryFileContent(
        TESTS_DIRECTORY "/Resources/BinaryFile.bin");

    EXPECT_EQ(binary.size(), 4);
    EXPECT_EQ(binary[0], '\0');
    EXPECT_EQ(binary[1], 'H');
    EXPECT_EQ(binary[2], 'i');
    EXPECT_EQ(binary[3], '\0');
}

TEST(Utility, StringLowerCase)
{
    std::string text = "HeLLo WoNDERfUL WoRlD! :)";
    EXPECT_EQ(Common::StringLowerCase(text), "hello wonderful world! :)");
}

TEST(Utility, StringTokenize)
{
    std::string text = "Hello wonderful world! :)";
    std::vector<std::string> tokens = Common::StringTokenize(text, ' ');

    EXPECT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0], "Hello");
    EXPECT_EQ(tokens[1], "wonderful");
    EXPECT_EQ(tokens[2], "world!");
    EXPECT_EQ(tokens[3], ":)");
}

TEST(Utility, StringTrim)
{
    std::string text = "   @)#($*%&^  hello world !   )*(&$^%#@     ";

    EXPECT_EQ(Common::StringTrimLeft(text,
        " @#$%^&*()"), "hello world !   )*(&$^%#@     ");
    EXPECT_EQ(Common::StringTrimRight(text,
        " @#$%^&*()"), "   @)#($*%&^  hello world !");
    EXPECT_EQ(Common::StringTrim(text,
        " @#$%^&*()"), "hello world !");
}

TEST(Utility, StringHash)
{
    EXPECT_NE(Common::StringHash<uint32_t>("Armored orange"), 0);
    EXPECT_NE(Common::StringHash<uint64_t>("Naked banana"),
        Common::StringHash<uint64_t>("Dressed apple"));
}

TEST(Utility, CRC)
{
    uint8_t dataFirst[4] = { '2', '0', '3', '5' };
    uint8_t dataSecond[4] = { '1', '9', '4', '5' };

    EXPECT_NE(Common::CalculateCrc32(0, &dataFirst[0], Common::StaticArraySize(dataFirst)), 0);
    EXPECT_NE(Common::CalculateCrc32(0, &dataFirst[0], Common::StaticArraySize(dataFirst)),
        Common::CalculateCrc32(0, &dataSecond[0], Common::StaticArraySize(dataSecond)));
}

TEST(UtilityReorderWithIndices, ZeroElements)
{
    std::vector<std::string> array;
    std::vector<std::size_t> indices;

    EXPECT_TRUE(Common::ReorderWithIndices(array, indices));
}

TEST(UtilityReorderWithIndices, OneElement)
{
    std::vector<std::string> array = { "First" };
    std::vector<std::size_t> indices = { 0 };

    EXPECT_TRUE(Common::ReorderWithIndices(array, indices));
    EXPECT_EQ(array[0], "First");
}

TEST(UtilityReorderWithIndices, TwoElements)
{
    std::vector<std::string> array = { "Second", "First" };
    std::vector<std::size_t> indices = { 1, 0 };

    EXPECT_TRUE(Common::ReorderWithIndices(array, indices));
    EXPECT_EQ(array[0], "First");
    EXPECT_EQ(array[1], "Second");
}

TEST(UtilityReorderWithIndices, ThreeElements)
{
    std::vector<std::string> array = { "First", "Third", "Second" };
    std::vector<std::size_t> indices = { 0, 2, 1 };

    EXPECT_TRUE(Common::ReorderWithIndices(array, indices));
    EXPECT_EQ(array[0], "First");
    EXPECT_EQ(array[1], "Second");
    EXPECT_EQ(array[2], "Third");
}

TEST(UtilityReorderWithIndices, FourElements)
{
    std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
    std::vector<std::size_t> indices = { 1, 3, 2, 0 };

    EXPECT_TRUE(Common::ReorderWithIndices(array, indices));
    EXPECT_EQ(array[0], "First");
    EXPECT_EQ(array[1], "Second");
    EXPECT_EQ(array[2], "Third");
    EXPECT_EQ(array[3], "Fourth");
}

TEST(UtilityReorderWithIndices, SizeMismatch)
{
    std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
    std::vector<std::size_t> indices = { 1, 3, 2 };

    EXPECT_FALSE(Common::ReorderWithIndices(array, indices));
    EXPECT_EQ(array[0], "Fourth");
    EXPECT_EQ(array[1], "First");
    EXPECT_EQ(array[2], "Third");
    EXPECT_EQ(array[3], "Second");
}

TEST(UtilityReorderWithIndices, OutOfBoundsIndices)
{
    std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
    std::vector<std::size_t> indices = { 1, 4, 2, 0 };

    EXPECT_FALSE(Common::ReorderWithIndices(array, indices));
    EXPECT_EQ(array[0], "Fourth");
    EXPECT_EQ(array[1], "First");
    EXPECT_EQ(array[2], "Third");
    EXPECT_EQ(array[3], "Second");
}
