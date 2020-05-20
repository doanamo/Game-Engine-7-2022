/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include <TestHelpers.hpp>
#include <Common/Utility.hpp>

bool TestClamp()
{
    TEST_EQ(Common::Clamp(0, 1, 2), 1);
    TEST_EQ(Common::Clamp(3, 1, 2), 2);
    TEST_EQ(Common::Clamp(0.0f, 1.0f, 2.0f), 1.0f);
    TEST_EQ(Common::Clamp(3.0f, 1.0f, 2.0f), 2.0f);

    return true;
}

bool TestStaticArraySize()
{
    int array[4] = { 0 };

    TEST_EQ(Common::StaticArraySize(array), 4);

    return true;
}

bool TestNumericCast()
{
    uint64_t source = (uint64_t)std::numeric_limits<uint32_t>::max();
    uint32_t target = Common::NumericalCast<uint32_t>(source);

    return true;
}

bool TestClearContainer()
{
    std::vector<int> elements;
    elements.push_back(234);
    elements.push_back(243);
    elements.push_back(432);
    elements.push_back(342);
    Common::ClearContainer(elements);

    TEST_EQ(elements.size(), 0);
    TEST_EQ(elements.capacity(), 0);

    return true;
}

const std::string filePathValid = "C:/secret\\project\\file.cpp";
const std::string filePathWithoutDirectory = "file.cpp";
const std::string filePathWithoutName = "C:/secret\\project/.cpp";
const std::string filePathWithoutExtension = "C:/secret\\project\\file";
const std::string filePathEmpty = "";

bool TestGetFileDirectory()
{
    TEST_EQ(Common::GetFileDirectory(filePathValid), "C:/secret\\project\\");
    TEST_EQ(Common::GetFileDirectory(filePathWithoutDirectory), "");
    TEST_EQ(Common::GetFileDirectory(filePathWithoutName), "C:/secret\\project/");
    TEST_EQ(Common::GetFileDirectory(filePathWithoutExtension), "C:/secret\\project\\");
    TEST_EQ(Common::GetFileDirectory(filePathEmpty), "");
    
    return true;
}

bool TestGetFileName()
{
    TEST_EQ(Common::GetFileName(filePathValid), "file");
    TEST_EQ(Common::GetFileName(filePathWithoutDirectory), "file");
    TEST_EQ(Common::GetFileName(filePathWithoutName), "");
    TEST_EQ(Common::GetFileName(filePathWithoutExtension), "file");
    TEST_EQ(Common::GetFileName(filePathEmpty), "");

    return true;
}

bool TestGetFileExtension()
{
    TEST_EQ(Common::GetFileExtension(filePathValid), "cpp");
    TEST_EQ(Common::GetFileExtension(filePathWithoutDirectory), "cpp");
    TEST_EQ(Common::GetFileExtension(filePathWithoutName), "cpp");
    TEST_EQ(Common::GetFileExtension(filePathWithoutExtension), "");
    TEST_EQ(Common::GetFileExtension(filePathEmpty), "");

    return true;
}

bool TestGetTextFileContent()
{
    std::string text = Common::GetTextFileContent(TESTS_DIRECTORY "/Resources/TextFile.txt");
    TEST_EQ(text, "Hello world!");

    return true;
}

bool TestGetBinaryFileContent()
{
    std::vector<char> binary = Common::GetBinaryFileContent(TESTS_DIRECTORY "/Resources/BinaryFile.bin");

    TEST_EQ(binary.size(), 4);
    TEST_EQ(binary[0], '\0');
    TEST_EQ(binary[1], 'H');
    TEST_EQ(binary[2], 'i');
    TEST_EQ(binary[3], '\0');

    return true;
}

bool TestStringLowerCase()
{
    std::string text = "HeLLo WoNDERfUL WoRlD! :)";
    TEST_EQ(Common::StringLowerCase(text), "hello wonderful world! :)");

    return true;
}

bool TestStringTokenize()
{
    std::string text = "Hello wonderful world! :)";
    std::vector<std::string> tokens = Common::StringTokenize(text, ' ');

    TEST_EQ(tokens.size(), 4);
    TEST_EQ(tokens[0], "Hello");
    TEST_EQ(tokens[1], "wonderful");
    TEST_EQ(tokens[2], "world!");
    TEST_EQ(tokens[3], ":)")

    return true;
}

bool TestStringTrim()
{
    std::string text = "   @)#($*%&^  hello world !   )*(&$^%#@     ";

    TEST_EQ(Common::StringTrimLeft(text, " @#$%^&*()"), "hello world !   )*(&$^%#@     ");
    TEST_EQ(Common::StringTrimRight(text, " @#$%^&*()"), "   @)#($*%&^  hello world !");
    TEST_EQ(Common::StringTrim(text, " @#$%^&*()"), "hello world !");

    return true;
}

bool TestStringHash()
{
    TEST_NEQ(Common::StringHash("Armored orange"), 0);
    TEST_NEQ(Common::StringHash("Naked banana"), Common::StringHash("Dressed apple"));

    return true;
}

bool TestCalculateCRC()
{
    char dataFirst[4] = { '2', '0', '3', '5' };
    char dataSecond[4] = { '1', '9', '4', '5' };

    TEST_NEQ(Common::CalculateCRC32(0, &dataFirst[0], Common::StaticArraySize(dataFirst)), 0);
    TEST_NEQ(Common::CalculateCRC32(0, &dataFirst[0], Common::StaticArraySize(dataFirst)),
        Common::CalculateCRC32(0, &dataSecond[0], Common::StaticArraySize(dataSecond)));

    return true;
}

bool TestReorderWithIndices()
{
    // Zero element count.
    {
        std::vector<std::string> array;
        std::vector<std::size_t> indices;

        TEST_TRUE(Common::ReorderWithIndices(array, indices));
    }

    // One element count.
    {
        std::vector<std::string> array = { "First" };
        std::vector<std::size_t> indices = { 0 };

        TEST_TRUE(Common::ReorderWithIndices(array, indices));
        TEST_EQ(array[0], "First");
    }

    // Two element count.
    {
        std::vector<std::string> array = { "Second", "First" };
        std::vector<std::size_t> indices = { 1, 0 };

        TEST_TRUE(Common::ReorderWithIndices(array, indices));
        TEST_EQ(array[0], "First");
        TEST_EQ(array[1], "Second");
    }

    // Three element count.
    {
        std::vector<std::string> array = { "First", "Third", "Second" };
        std::vector<std::size_t> indices = { 0, 2, 1 };

        TEST_TRUE(Common::ReorderWithIndices(array, indices));
        TEST_EQ(array[0], "First");
        TEST_EQ(array[1], "Second");
        TEST_EQ(array[2], "Third");
    }

    // Four element count.
    {
        std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
        std::vector<std::size_t> indices = { 1, 3, 2, 0 };

        TEST_TRUE(Common::ReorderWithIndices(array, indices));
        TEST_EQ(array[0], "First");
        TEST_EQ(array[1], "Second");
        TEST_EQ(array[2], "Third");
        TEST_EQ(array[3], "Fourth");
    }
    
    // Non matching sizes.
    {
        std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
        std::vector<std::size_t> indices = { 1, 3, 2 };

        TEST_FALSE(Common::ReorderWithIndices(array, indices));
        TEST_EQ(array[0], "Fourth");
        TEST_EQ(array[1], "First");
        TEST_EQ(array[2], "Third");
        TEST_EQ(array[3], "Second");
    }

    // Out of bounds indices.
    {
        std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
        std::vector<std::size_t> indices = { 1, 4, 2, 0 };

        TEST_FALSE(Common::ReorderWithIndices(array, indices));
        TEST_EQ(array[0], "Fourth");
        TEST_EQ(array[1], "First");
        TEST_EQ(array[2], "Third");
        TEST_EQ(array[3], "Second");
    }

    return true;
}

int main()
{
    TEST_RUN(TestClamp);
    TEST_RUN(TestStaticArraySize);
    TEST_RUN(TestClearContainer);
    TEST_RUN(TestNumericCast);
    TEST_RUN(TestGetFileDirectory);
    TEST_RUN(TestGetFileName);
    TEST_RUN(TestGetFileExtension);
    TEST_RUN(TestGetTextFileContent);
    TEST_RUN(TestGetBinaryFileContent);
    TEST_RUN(TestStringLowerCase);
    TEST_RUN(TestStringTokenize);
    TEST_RUN(TestStringTrim);
    TEST_RUN(TestReorderWithIndices);
}
