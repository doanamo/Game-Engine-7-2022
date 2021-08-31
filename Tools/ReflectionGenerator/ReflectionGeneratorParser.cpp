/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Precompiled.hpp"
#include "ReflectionGeneratorTypes.hpp"
#include "ReflectionGeneratorParser.hpp"
#include "ReflectionGeneratorUtility.hpp"

namespace
{
    using TypeNameMap = std::unordered_map<std::string, std::size_t>;

    bool VisitReflectedType(const ParsedTypeList& parsedTypes, const TypeNameMap& parsedTypeMap,
        SortedTypeList& sortedTypes, VisitedTypeList& visitedTypes,
        DependencyTypeStack& dependencyStack, const std::size_t typeIndex)
    {
        const ReflectedType& reflectedType = parsedTypes[typeIndex];
        if(!dependencyStack.emplace(typeIndex).second)
        {
            std::cerr << "ReflectionGenerator: Detected cyclic dependency!\n\t\""
                << reflectedType.name << "\" from \"" << reflectedType.headerPath.generic_string()
                << "(" << reflectedType.headerLine << ")\"\n";
            return false;
        }

        if(reflectedType.base != "Reflection::NullType")
        {
            auto it = parsedTypeMap.find(reflectedType.base);
            if(it != parsedTypeMap.end())
            {
                if(!VisitReflectedType(parsedTypes, parsedTypeMap, sortedTypes,
                    visitedTypes, dependencyStack, it->second))
                {
                    return false;
                }
            }
        }

        if(!visitedTypes[typeIndex])
        {
            sortedTypes.push_back(&reflectedType);
            visitedTypes[typeIndex] = true;
        }

        return true;
    };
}

ReflectionGeneratorParser::ReflectionGeneratorParser() = default;
ReflectionGeneratorParser::~ReflectionGeneratorParser() = default;

bool ReflectionGeneratorParser::ParseReflectedTypes(const HeaderFileList& headerFiles)
{
    if(!m_reflectedHeaders.empty() || !m_parsedTypes.empty() || !m_sortedTypes.empty())
    {
        std::cerr << "ReflectionGenerator: Cannot collect reflected types more than once!\n";
        return false;
    }

    // Parse header files and map types with reflection enabled.
    for(const auto& headerPath : headerFiles)
    {
        if(headerPath.filename() == "ReflectionDeclare.hpp")
            continue;

        std::ifstream file(headerPath);
        if(!file.is_open())
        {
            std::cerr << "ReflectionGenerator: Failed to open header file - \""
                << headerPath << "\"\n";
            return false;
        }

        std::string line;
        std::size_t lineCount = 0;

        while(std::getline(file, line))
        {
            ++lineCount;

            std::string_view reflectionTokenName = "REFLECTION_TYPE(";
            std::size_t reflectionTokenBegin = line.find(reflectionTokenName);
            if(reflectionTokenBegin == std::string::npos)
            {
                reflectionTokenName = "REFLECTION_TYPE_BEGIN(";
                reflectionTokenBegin = line.find(reflectionTokenName);

                if(reflectionTokenBegin == std::string::npos)
                    continue;
            }

            std::size_t reflectionTokenEnd = line.find(')', reflectionTokenBegin);
            if(reflectionTokenEnd == std::string::npos)
            {
                PrintMalformedDeclaration(headerPath, lineCount);
                return false;
            }

            std::size_t delimiterToken = line.find(',', reflectionTokenBegin);

            std::size_t typeTokenBegin = reflectionTokenBegin + reflectionTokenName.size();
            std::size_t typeTokenEnd = delimiterToken != std::string::npos ?
                delimiterToken : reflectionTokenEnd;

            TrimWhiteSpacesLeft(line, typeTokenBegin);
            TrimWhiteSpacesRight(line, typeTokenEnd);
            std::string typeName = line.substr(typeTokenBegin, typeTokenEnd - typeTokenBegin);

            std::string typeBase;
            if(delimiterToken != std::string::npos)
            {
                if(delimiterToken > reflectionTokenEnd)
                {
                    PrintMalformedDeclaration(headerPath, lineCount);
                    return false;
                }

                std::size_t baseTokenBegin = delimiterToken + 1;
                std::size_t baseTokenEnd = reflectionTokenEnd;

                TrimWhiteSpacesLeft(line, baseTokenBegin);
                TrimWhiteSpacesRight(line, baseTokenEnd);
                typeBase = line.substr(baseTokenBegin, baseTokenEnd - baseTokenBegin);
            }
            else
            {
                typeBase = "Reflection::NullType";
            }

            ReflectedType& parsedType = m_parsedTypes.emplace_back();
            parsedType.name = std::move(typeName);
            parsedType.base = std::move(typeBase);
            parsedType.headerPath = headerPath;
            parsedType.headerLine = lineCount;
        }
    }

    // Collect unique headers.
    for(const auto& type : m_parsedTypes)
    {
        m_reflectedHeaders.insert(type.headerPath);
    }

    // Create hash map of reflected type name strings for performance.
    TypeNameMap typeNameMap;
    for(std::size_t i = 0; i < m_parsedTypes.size(); ++i)
    {
        const ReflectedType& reflectedType = m_parsedTypes[i];
        auto result = typeNameMap.emplace(reflectedType.name, i);

        if(!result.second)
        {
            const ReflectedType& duplicatedType = m_parsedTypes[result.first->second];

            std::cerr << "ReflectionGenerator: Found two reflected types with same names!\n"
                << "\t\"" << reflectedType.name << "\" from \""
                << reflectedType.headerPath.generic_string()
                << "(" << reflectedType.headerLine << ")\"\n"
                << "\t\"" << duplicatedType.name << "\" from \""
                << duplicatedType.headerPath.generic_string()
                << "(" << duplicatedType.headerLine << ")\"\n";

            return false;
        }
    }

    // Perform topological sort of types by their dependencies to ensure
    // that base types are always registered before their derived types.
    m_sortedTypes.reserve(m_parsedTypes.size());

    VisitedTypeList visitedTypes(m_parsedTypes.size(), false);
    for(std::size_t i = 0; i < m_parsedTypes.size(); ++i)
    {
        DependencyTypeStack dependencyStack;
        if(!VisitReflectedType(m_parsedTypes, typeNameMap, m_sortedTypes,
            visitedTypes, dependencyStack, i))
        {
            return false;
        }
    }

    return true;
}
