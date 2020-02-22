/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include <Common/Utility.hpp>
#include "TestHelpers.hpp"

bool TestClamp()
{
    TEST_EQ(Utility::Clamp(0, 1, 2), 1);
    TEST_EQ(Utility::Clamp(3, 1, 2), 2);
    TEST_EQ(Utility::Clamp(0.0f, 1.0f, 2.0f), 1.0f);
    TEST_EQ(Utility::Clamp(3.0f, 1.0f, 2.0f), 2.0f);

    return true;
}

bool TestStaticArraySize()
{
    int array[4];

    TEST_EQ(Utility::StaticArraySize(array), 4);

    return true;
}

bool TestClearContainer()
{
    std::vector<int> elements;
    elements.push_back(234);
    elements.push_back(243);
    elements.push_back(432);
    elements.push_back(342);
    Utility::ClearContainer(elements);

    TEST_EQ(elements.size(), 0);
    TEST_EQ(elements.capacity(), 0);

    return true;
}

bool TestGetFileDirectoryNameExtenstion()
{
    std::string filePathValid = "C:/secret\\project\\file.cpp";
    std::string filePathWithoutDirectory = "file.cpp";
    std::string filePathWithoutName = "C:/secret\\project/.cpp";
    std::string filePathWithoutExtension = "C:/secret\\project\\file";
    std::string filePathEmpty = "";

    TEST_EQ(Utility::GetFileDirectory(filePathValid), "C:/secret\\project\\");
    TEST_EQ(Utility::GetFileDirectory(filePathWithoutDirectory), "");
    TEST_EQ(Utility::GetFileDirectory(filePathWithoutName), "C:/secret\\project/");
    TEST_EQ(Utility::GetFileDirectory(filePathWithoutExtension), "C:/secret\\project\\");
    TEST_EQ(Utility::GetFileDirectory(filePathEmpty), "");

    TEST_EQ(Utility::GetFileName(filePathValid), "file");
    TEST_EQ(Utility::GetFileName(filePathWithoutDirectory), "file");
    TEST_EQ(Utility::GetFileName(filePathWithoutName), "");
    TEST_EQ(Utility::GetFileName(filePathWithoutExtension), "file");
    TEST_EQ(Utility::GetFileName(filePathEmpty), "");

    TEST_EQ(Utility::GetFileExtension(filePathValid), "cpp");
    TEST_EQ(Utility::GetFileExtension(filePathWithoutDirectory), "cpp");
    TEST_EQ(Utility::GetFileExtension(filePathWithoutName), "cpp");
    TEST_EQ(Utility::GetFileExtension(filePathWithoutExtension), "");
    TEST_EQ(Utility::GetFileExtension(filePathEmpty), "");

    return true;
}

bool TestGetTextFileContent()
{
    std::string text = Utility::GetTextFileContent(TESTS_DIRECTORY "/Resources/TextFile.txt");

    TEST_EQ(text, "Hello world!");

    return true;
}

bool TestGetBinaryFileContent()
{
    std::vector<char> binary = Utility::GetBinaryFileContent(TESTS_DIRECTORY "/Resources/BinaryFile.bin");

    TEST_EQ(binary.size(), 4);
    TEST_EQ(binary[0], '\0');
    TEST_EQ(binary[1], 'H');
    TEST_EQ(binary[2], 'i');
    TEST_EQ(binary[3], '\0');

    return true;
}

bool TestTokenizeString()
{
    std::string text = "Hello wonderful world! :)";
    std::vector<std::string> tokens = Utility::TokenizeString(text, ' ');

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

    TEST_EQ(Utility::StringTrimLeft(text, " @#$%^&*()"), "hello world !   )*(&$^%#@     ");
    TEST_EQ(Utility::StringTrimRight(text, " @#$%^&*()"), "   @)#($*%&^  hello world !");
    TEST_EQ(Utility::StringTrim(text, " @#$%^&*()"), "hello world !");

    return true;
}

bool TestReorderWithIndices()
{
    // Zero element count.
    {
        std::vector<std::string> array;
        std::vector<std::size_t> indices;

        TEST_TRUE(Utility::ReorderWithIndices(array, indices));
    }

    // One element count.
    {
        std::vector<std::string> array = { "First" };
        std::vector<std::size_t> indices = { 0 };

        TEST_TRUE(Utility::ReorderWithIndices(array, indices));
        TEST_EQ(array[0], "First");
    }

    // Two element count.
    {
        std::vector<std::string> array = { "Second", "First" };
        std::vector<std::size_t> indices = { 1, 0 };

        TEST_TRUE(Utility::ReorderWithIndices(array, indices));
        TEST_EQ(array[0], "First");
        TEST_EQ(array[1], "Second");
    }

    // Three element count.
    {
        std::vector<std::string> array = { "First", "Third", "Second" };
        std::vector<std::size_t> indices = { 0, 2, 1 };

        TEST_TRUE(Utility::ReorderWithIndices(array, indices));
        TEST_EQ(array[0], "First");
        TEST_EQ(array[1], "Second");
        TEST_EQ(array[2], "Third");
    }

    // Four element count.
    {
        std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
        std::vector<std::size_t> indices = { 1, 3, 2, 0 };

        TEST_TRUE(Utility::ReorderWithIndices(array, indices));
        TEST_EQ(array[0], "First");
        TEST_EQ(array[1], "Second");
        TEST_EQ(array[2], "Third");
        TEST_EQ(array[3], "Fourth");
    }
    
    // Non matching sizes.
    {
        std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
        std::vector<std::size_t> indices = { 1, 3, 2 };

        TEST_FALSE(Utility::ReorderWithIndices(array, indices));
        TEST_EQ(array[0], "Fourth");
        TEST_EQ(array[1], "First");
        TEST_EQ(array[2], "Third");
        TEST_EQ(array[3], "Second");
    }

    // Out of bounds indices.
    {
        std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
        std::vector<std::size_t> indices = { 1, 4, 2, 0 };

        TEST_FALSE(Utility::ReorderWithIndices(array, indices));
        TEST_EQ(array[0], "Fourth");
        TEST_EQ(array[1], "First");
        TEST_EQ(array[2], "Third");
        TEST_EQ(array[3], "Second");
    }

    return true;
}

int main()
{
    if(!TestClamp())
        return 1;

    if(!TestStaticArraySize())
        return 1;

    if(!TestClearContainer())
        return 1;
    
    if(!TestGetFileDirectoryNameExtenstion())
        return 1;

    if(!TestGetTextFileContent())
        return 1;

    if(!TestGetBinaryFileContent())
        return 1;

    if(!TestTokenizeString())
        return 1;

    if(!TestStringTrim())
        return 1;

    if(!TestReorderWithIndices())
        return 1;

    return 0;
}