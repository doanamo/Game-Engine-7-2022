/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <filesystem>
#include "Common/Debug.hpp"

/*
    Utility

    Collection of small useful functions.
*/

namespace Common
{
    template<typename Type>
    constexpr Type* Pointer(Type& object)
    {
        return &object;
    }

    template<typename Type>
    constexpr Type* Pointer(Type* object)
    {
        return object;
    }

    template<typename Type>
    constexpr Type* Pointer(const std::unique_ptr<Type>& object)
    {
        return object.get();
    }

    template<typename Type>
    constexpr Type Clamp(const Type& value, const Type& lower, const Type& upper)
    {
        return std::max(lower, std::min(value, upper));
    }

    template<typename Type, std::size_t Size>
    constexpr std::size_t StaticArraySize(const Type(&)[Size])
    {
        return Size;
    }

    template<typename Target, typename Source>
    constexpr Target NumericalCast(const Source& value)
    {
        /*
            Casts numerical types with assertion guarantee for data loss. Checks in debug if
            conversion will lead to any loss of data. This is useful when dealing with libraries
            which do not fully convert from 32bit to 64bit types on their own (e.g. OpenGL). Loss
            check is performed by converting to target type and then back to source type, after
            which results are compared.
        */

        ASSERT(static_cast<Source>(static_cast<Target>(value)) == value,
            "Numerical conversion failed due to data loss!");

        return static_cast<Target>(value);
    }

    template<typename Type>
    void FreeContainer(Type& container)
    {
        /*
            Swaps container with new one to ensure that its memory gets deallocated.
        */

        Type empty;
        container.swap(empty);
    }

    std::string GetTextFileContent(const std::filesystem::path path);
    std::vector<char> GetBinaryFileContent(const std::filesystem::path path);

    std::string StringLowerCase(const std::string text);
    std::vector<std::string> StringTokenize(const std::string text, char character = ' ');
    std::string StringTrimLeft(const std::string text, const char* characters = " ");
    std::string StringTrimRight(const std::string text, const char* characters = " ");
    std::string StringTrim(const std::string text, const char* characters = " ");

    template<typename Type>
    constexpr Type StringHash(const std::string_view string) noexcept
    {
        static_assert(std::is_same<Type, uint32_t>::value || std::is_same<Type, uint64_t>::value);

        /*
            Small and simple hashing function for strings. May need to be replaced in case of
            collisions. Use only if you can detect possible collisions, as t is obviously not
            cryptographically secure.

            Current implementation: djb2
        */

        Type hash = 5381;
        for(char c : string)
        {
            hash = ((hash << 5) + hash) + c;
        }

        return hash;
    }

    template<typename Type>
    constexpr Type CombineHash(const Type seed, const Type hash)
    {
        /*
            Combines two hashes into new unique one of same size. Requires good hashing function
            for this to work properly.

            Current implementation: boost::hash_combine
        */

        return seed ^ (hash + 0x9e3779b9 + (seed << 6) + (seed >> 2));
    }

    uint32_t CalculateCrc32(uint32_t crc, const uint8_t* data, std::size_t size);

    template<typename Type>
    bool ReorderWithIndices(std::vector<Type>& elements, const std::vector<std::size_t>& order)
    {
        /*
            Reorders vector using an array of indices. This is useful in case we have two or more
            collections that need to be sorted in same way based on information from each. Sort can
            be performed on array of indices that then can be used to quickly rearrange elements in
            two collections. Result will not make sense if order indices are duplicated!
        */

        if(elements.size() != order.size())
            return false;

        for(std::size_t i : order)
        {
            if(i >= elements.size())
                return false;
        }

        if(elements.size() == 0)
            return true;

        std::vector<std::size_t> indices(order.size());
        std::iota(indices.begin(), indices.end(), 0);

        for(std::size_t i = 0; i < elements.size(); ++i)
        {
            std::size_t desiredPlacement = order[i];

            if(i != elements.size() - 1)
            {
                for(std::size_t j = i; j < elements.size(); ++j)
                {
                    if(desiredPlacement == indices[j])
                    {
                        std::swap(elements[i], elements[j]);
                        std::swap(indices[i], indices[j]);
                        break;
                    }
                }
            }

            ASSERT(indices[i] == order[i],
                "Elements will not be ordered correctly due to detected duplication of indices");
        }

        return true;
    }
}
