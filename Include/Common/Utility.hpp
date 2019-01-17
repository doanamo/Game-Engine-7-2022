/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>
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
    template<typename Type, std::size_t Size>
    constexpr std::size_t StaticArraySize(const Type(&)[Size])
    {
        return Size;
    }

    // Safely casts numerical types without any data loss.
    // Checks in debug if conversion will lead to any loss of data.
    // This is useful when dealing with libraries which do not fully
    // convert from 32bit to 64bit types on their own (e.g. OpenGL).
    template<typename Target, typename Source>
    Target NumericalCast(const Source& value)
    {
        ASSERT(static_cast<Source>(static_cast<Target>(value)) == value,
            "Numerical conversion failed due to data loss!");

        return static_cast<Target>(value);
    }

    // Frees container's memory by swapping it with an empty container.
    template<typename Type>
    void ClearContainer(Type& container)
    {
        Type temp;
        container.swap(temp);
    }

    // Gets the directory from a file path.
    std::string GetFileDirectory(std::string filePath);

    // Gets the name from a file path.
    std::string GetFileName(std::string filePath);

    // Gets the extension from a file path.
    std::string GetFileExtension(std::string filePath);

    // Gets the content of a text file.
    std::string GetTextFileContent(std::string filePath);

    // Gets the content of a binary file.
    std::vector<char> GetBinaryFileContent(std::string filePath);

    // Splits a string into tokens.
    std::vector<std::string> TokenizeString(std::string text, char character = ' ');

    // Removes leading characters in a string.
    std::string StringTrimLeft(std::string text, const char* characters = " ");

    // Removes trailing character in a string.
    std::string StringTrimRight(std::string text, const char* characters = " ");

    // Removes characters from both ends of a string.
    std::string StringTrim(std::string text, const char* characters = " ");

    // Reorders a vector using an array of indices.
    template<typename Type>
    void ReorderWithIndices(std::vector<Type>& elements, const std::vector<std::size_t>& order)
    {
        VERIFY(elements.size() == order.size(), "Array sizes must match!");

        // Returns if there are no elements to sort.
        if(elements.size() == 0)
            return;

        // Create an array of indices.
        std::vector<std::size_t> indices(order.size());
        std::iota(indices.begin(), indices.end(), 0);

        // Rearrange values in a vector.
        for(std::size_t i = 0; i < elements.size() - 1; ++i)
        {
            std::size_t desired = order[i];

            ASSERT(desired < elements.size(), "Desired index higher than vector size will produce incorrect results!");

            for(std::size_t j = i; j < elements.size(); ++j)
            {
                if(desired == indices[j])
                {
                    std::swap(elements[i], elements[j]);
                    std::swap(indices[i], indices[j]);
                    break;
                }
            }
        }
    }
}
