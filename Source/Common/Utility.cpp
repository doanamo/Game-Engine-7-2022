/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Common/Utility.hpp"

std::string Common::GetFileDirectory(const std::string filePath)
{
    std::string directory;

    std::size_t it = filePath.find_last_of("/\\");
    if(it != std::string::npos)
    {
        directory = filePath.substr(0, it + 1);
    }

    return directory;
}

std::string Common::GetFileName(const std::string filePath)
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

std::string Common::GetFileExtension(const std::string filePath)
{
    std::string extension;

    std::size_t it = filePath.find_last_of(".");
    if(it != std::string::npos)
    {
        extension = filePath.substr(it + 1);
    }

    return extension;
}

std::string Common::GetTextFileContent(const std::string filePath)
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

std::vector<char> Common::GetBinaryFileContent(const std::string filePath)
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

std::string Common::StringLowerCase(const std::string text)
{
    std::string result;

    for(char character : text)
    {
        result.push_back(std::tolower(character));
    }

    return result;
}

std::vector<std::string> Common::StringTokenize(const std::string text, char delimeter)
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

std::string Common::StringTrimLeft(const std::string text, const char* characters)
{
    std::string result = text;
    return result.erase(0, text.find_first_not_of(characters));
}

std::string Common::StringTrimRight(const std::string text, const char* characters)
{
    std::string result = text;
    return result.erase(text.find_last_not_of(characters) + 1);
}

std::string Common::StringTrim(std::string text, const char* characters)
{
    return StringTrimLeft(StringTrimRight(text, characters), characters);
}

uint32_t Common::StringHash(const std::string string)
{
    // Small and simple hashing function for strings.
    // May need to be replaced in case of collisions.
    // Use only if you can detect possible collisions.
    // It is obviously not cryptographically secure.
    const uint32_t prime = 257;
    uint32_t hash = 0;

    for(char c : string)
    {
        hash = hash * prime + c;
    }

    return hash;
}

uint32_t Common::CalculateCRC32(uint32_t crc, const char* data, std::size_t size)
{
    // Implementation based on: https://stackoverflow.com/a/27950866
    crc = ~crc;

    while(size--)
    {
        crc ^= (unsigned char)*data++;

        for(int i = 0; i < 8; ++i)
        {
            crc = crc & 1 ? (crc >> 1) ^ 0x82f63b78 : crc >> 1;
        }
    }

    return ~crc;
}
