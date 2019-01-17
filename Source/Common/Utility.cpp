/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Common/Utility.hpp"

std::string Utility::GetFileDirectory(std::string filePath)
{
    std::string directory;

    std::size_t it = filePath.find_last_of("/\\");
    if(it != std::string::npos)
    {
        directory = filePath.substr(0, it + 1);
    }

    return directory;
}

std::string Utility::GetFileName(std::string filePath)
{
    std::size_t begin = filePath.find_last_of("/\\");
    std::size_t end = filePath.find_last_of(".");

    if(begin == std::string::npos)
    {
        begin = 0;
    }
    else
    {
        begin += 1;
    }

    if(end == std::string::npos)
    {
        end = filePath.size();
    }

    return filePath.substr(begin, end - begin);
}

std::string Utility::GetFileExtension(std::string filePath)
{
    std::string extension;

    std::size_t it = filePath.find_last_of(".");
    if(it != std::string::npos)
    {
        extension = filePath.substr(it + 1);
    }

    return extension;
}

std::string Utility::GetTextFileContent(std::string filePath)
{
    std::ifstream file(filePath);
    std::string content;

    if(file)
    {
        file.seekg(0, std::ios::end);
        content.resize((unsigned int)file.tellg());
        file.seekg(0, std::ios::beg);

        file.read(&content[0], content.size());
    }

    return content;
}

std::vector<char> Utility::GetBinaryFileContent(std::string filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    std::vector<char> content;

    if(file)
    {
        file.seekg(0, std::ios::end);
        content.resize((unsigned int)file.tellg());
        file.seekg(0, std::ios::beg);

        file.read(&content[0], content.size());
    }

    return content;
}

std::vector<std::string> Utility::TokenizeString(std::string text, char delimeter)
{
    std::vector<std::string> result;

    auto begin = text.begin();
    auto it = text.begin();

    while(it != text.end())
    {
        if(*it == delimeter)
        {
            result.push_back(std::string(begin, it));
            begin = ++it;
        }
        else
        {
            ++it;
        }

        if(it == text.end())
        {
            result.push_back(std::string(begin, it));
        }
    }

    return result;
}

std::string Utility::StringTrimLeft(std::string text, const char* characters)
{
    return text.erase(0, text.find_first_not_of(characters));
}

std::string Utility::StringTrimRight(std::string text, const char* characters)
{
    return text.erase(text.find_last_not_of(characters) + 1);
}

std::string Utility::StringTrim(std::string text, const char* characters)
{
    return StringTrimLeft(StringTrimRight(text, characters), characters);
}
