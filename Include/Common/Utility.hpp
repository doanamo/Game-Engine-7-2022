/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include "Common/Debug.hpp"

/*
    Utility

    Collection of small utility functions.
*/

namespace Utility
{
    // Clamps a value between lower and upper range.
    template<typename Type>
    Type Clamp(const Type& value, const Type& lower, const Type& upper)
    {
        return std::max(lower, std::min(value, upper));
    }

    // Gets the size of a static array.
    template<typename Type, size_t Size>
    size_t StaticArraySize(const Type(&)[Size])
    {
        return Size;
    }

    // Frees container's memory by swapping it with an empty container.
    template<typename Type>
    void ClearContainer(Type& container)
    {
        container.swap(Type());
    }

    // Gets the directory from a file path.
    std::string GetFileDirectory(std::string filePath);

    // Gets the extension from a file path.
    std::string GetFileExtension(std::string filePath);

    // Gets the content of a text file.
    std::string GetTextFileContent(std::string filePath);

    // Gets the content of a binary file.
    std::vector<char> GetBinaryFileContent(std::string filePath);

    // Splits a string into tokens.
    std::vector<std::string> TokenizeString(std::string text, char character = ' ');

    // Removes leading characters in a string.
    std::string StringTrimLeft(std::string& text, const char* characters = " ");

    // Removes trailing character in a string.
    std::string StringTrimRight(std::string& text, const char* characters = " ");

    // Removes characters from both ends of a string.
    std::string StringTrim(std::string& text, const char* characters = " ");

    // Reorders a vector using an array of indices.
    template<typename Type>
    void Reorder(std::vector<Type>& values, const std::vector<std::size_t>& order)
    {
        ASSERT(values.size() == order.size(), "Array sizes must match!");

        // Create an array of indices.
        std::vector<std::size_t> indices(order.size());
        std::iota(indices.begin(), indices.end(), 0);

        // Rearange values in a vector.
        for(std::size_t i = 0; i < values.size() - 1; ++i)
        {
            std::size_t desired = order[i];

            for(std::size_t j = i; j < values.size(); ++j)
            {
                if(desired == indices[j])
                {
                    std::swap(values[i], values[j]);
                    std::swap(indices[i], indices[j]);
                    break;
                }
            }
        }
    }
}
