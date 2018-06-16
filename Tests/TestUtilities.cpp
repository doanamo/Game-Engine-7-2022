/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include <Common/Utility.hpp>

bool TestClamp()
{
    if(Utility::Clamp(0, 1, 2) != 1)
        return false;

    if(Utility::Clamp(3, 1, 2) != 2)
        return false;

    if(Utility::Clamp(0.0f, 1.0f, 2.0f) != 1.0f)
        return false;

    if(Utility::Clamp(3.0f, 1.0f, 2.0f) != 2.0f)
        return false;

    return true;
}

bool TestStaticArraySize()
{
    int array[4];
    return Utility::StaticArraySize(array) == 4;
}

bool TestClearContainer()
{
    std::vector<int> elements;
    elements.push_back(234);
    elements.push_back(243);
    elements.push_back(432);
    elements.push_back(342);
    Utility::ClearContainer(elements);

    return elements.size() == 0 && elements.capacity() == 0;
}

bool TestGetFileDirectoryNameExtenstion()
{
    std::string filePathValid = "C:/secret\\project\\file.cpp";
    std::string filePathWithoutDirectory = "file.cpp";
    std::string filePathWithoutName = "C:/secret\\project/.cpp";
    std::string filePathWithoutExtension = "C:/secret\\project\\file";
    std::string filePathEmpty = "";

    if(Utility::GetFileDirectory(filePathValid) != "C:/secret\\project\\")
        return false;

    if(Utility::GetFileDirectory(filePathWithoutDirectory) != "")
        return false;

    if(Utility::GetFileDirectory(filePathWithoutName) != "C:/secret\\project/")
        return false;

    if(Utility::GetFileDirectory(filePathWithoutExtension) != "C:/secret\\project\\")
        return false;

    if(Utility::GetFileDirectory(filePathEmpty) != "")
        return false;

    if(Utility::GetFileName(filePathValid) != "file")
        return false;

    if(Utility::GetFileName(filePathWithoutDirectory) != "file")
        return false;

    if(Utility::GetFileName(filePathWithoutName) != "")
        return false;

    if(Utility::GetFileName(filePathWithoutExtension) != "file")
        return false;

    if(Utility::GetFileName(filePathEmpty) != "")
        return false;

    if(Utility::GetFileExtension(filePathValid) != "cpp")
        return false;

    if(Utility::GetFileExtension(filePathWithoutDirectory) != "cpp")
        return false;

    if(Utility::GetFileExtension(filePathWithoutName) != "cpp")
        return false;

    if(Utility::GetFileExtension(filePathWithoutExtension) != "")
        return false;

    if(Utility::GetFileExtension(filePathEmpty) != "")
        return false;

    return true;
}

bool TestGetTextFileContent()
{
    std::string text = Utility::GetTextFileContent(TESTS_DIRECTORY "/Resources/TextFile.txt");
    return text == "Hello world!";
}

bool TestGetBinaryFileContent()
{
    std::vector<char> binary = Utility::GetBinaryFileContent(TESTS_DIRECTORY "/Resources/BinaryFile.bin");

    if(binary.size() != 4)
        return false;

    if(binary[0] != '\0')
        return false;

    if(binary[1] != 'H')
        return false;

    if(binary[2] != 'i')
        return false;

    if(binary[3] != '\0')
        return false;

    return true;
}

bool TestTokenizeString()
{
    std::string text = "Hello wonderful world! :)";
    std::vector<std::string> tokens = Utility::TokenizeString(text, ' ');

    if(tokens.size() != 4)
        return false;

    if(tokens[0] != "Hello")
        return false;

    if(tokens[1] != "wonderful")
        return false;

    if(tokens[2] != "world!")
        return false;

    if(tokens[3] != ":)")
        return false;

    return true;
}

bool TestStringTrim()
{
    std::string text = "   @)#($*%&^  hello world !   )*(&$^%#@     ";

    if(Utility::StringTrimLeft(text, " @#$%^&*()") != "hello world !   )*(&$^%#@     ")
        return false;

    if(Utility::StringTrimRight(text, " @#$%^&*()") != "   @)#($*%&^  hello world !")
        return false;

    if(Utility::StringTrim(text, " @#$%^&*()") != "hello world !")
        return false;

    return true;
}

bool TestReorderWithIndices()
{
    std::vector<std::string> array = { "Fourth", "First", "Third", "Second" };
    std::vector<std::size_t> indices = { 1, 3, 2, 0 };

    Utility::ReorderWithIndices(array, indices);

    if(array[0] != "First")
        return false;

    if(array[1] != "Second")
        return false;

    if(array[2] != "Third")
        return false;

    if(array[3] != "Fourth")
        return false;

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