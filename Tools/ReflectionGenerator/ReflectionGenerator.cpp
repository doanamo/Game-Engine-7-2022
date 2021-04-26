/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <unordered_map>

namespace fs = std::filesystem;

void PrintMalformedDeclaration(const fs::path& headerPath, const std::size_t headerLine)
{
    std::cerr << "ReflectionGenerator: Detected malformed REFLECTION_ENABLE() declaration"
        " in \"" << headerPath << "(" << headerLine << ")\"\n";
};

void TrimWhitespaceLeft(const std::string_view& string, std::size_t& position)
{
    while(std::isspace(string.at(position)))
        ++position;
};

void TrimWhitespaceRight(const std::string_view& string, std::size_t& position)
{
    while(std::isspace(string.at(position)))
        --position;
};

struct ReflectedType
{
    std::string name;
    std::string base;

    fs::path headerPath;
    std::size_t headerLine = 0;
};

using ParsedTypeList = std::vector<ReflectedType>;
using ParsedTypeMap = std::unordered_map<std::string, std::size_t>;
using SortedTypeList = std::vector<const ReflectedType*>;
using VisitedTypeList = std::vector<bool>;
using DependencyTypeStack = std::set<std::size_t>;

bool VisitReflectedType(const ParsedTypeList& parsedTypes, const ParsedTypeMap& parsedTypeMap,
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

int main(int argc, const char* argv[])
{
    // Check provided arguments.
    if(argc < 4)
    {
        std::cerr << "ReflectionGenerator: Unexpected number of arguments!\n";

        for(int arg = 0; arg < argc; ++arg)
        {
            std::cerr << "ReflectionGenerator: argv[" << arg << "] = " << argv[arg] << "\n";
        }

        return -1;
    }

    std::string_view targetName = argv[1];
    std::string_view outputDir = argv[2];

    // Create list of source directories.
    std::vector<std::string_view> sourceDirList;

    for(int arg = 3; arg < argc; ++arg)
    {
        sourceDirList.emplace_back(argv[arg]);
    }

    // Create list of header files.
    std::vector<fs::path> headerFileList;
    
    for(const auto& sourceDir : sourceDirList)
    {
        fs::path sourceDirPath(sourceDir);
        if(!fs::exists(sourceDirPath))
        {
            std::cerr << "ReflectionGenerator: Source directory path does not exist - \""
                << sourceDir << "\"\n";
            return -1;
        }

        for(const auto& dirEntry : fs::recursive_directory_iterator(sourceDirPath))
        {
            if(!dirEntry.is_regular_file())
                continue;

            if(dirEntry.path().extension() == ".hpp" || dirEntry.path().extension() == ".h")
            {
                headerFileList.push_back(dirEntry);
            }
        }
    }

    // Parse header files and map types with reflection enabled.
    ParsedTypeList parsedTypes;
    for(const auto& headerPath : headerFileList)
    {
        if(headerPath.filename() == "ReflectionDeclare.hpp")
            continue;

        std::ifstream file(headerPath);
        if(!file.is_open())
        {
            std::cerr << "ReflectionGenerator: Failed to open header file - \""
                << headerPath << "\"\n";
            return -1;
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
                return -1;
            }

            std::size_t delimiterToken = line.find(',', reflectionTokenBegin);

            std::size_t typeTokenBegin = reflectionTokenBegin + reflectionTokenName.size();
            std::size_t typeTokenEnd = delimiterToken != std::string::npos ?
                delimiterToken : reflectionTokenEnd;

            TrimWhitespaceLeft(line, typeTokenBegin);
            TrimWhitespaceRight(line, typeTokenEnd);
            std::string typeName = line.substr(typeTokenBegin, typeTokenEnd - typeTokenBegin);

            if(typeName == "Reflection::NullType")
            {
                continue; // Allow reflection registry to register this type manually.
            }

            std::string typeBase;
            if(delimiterToken != std::string::npos)
            {
                if(delimiterToken > reflectionTokenEnd)
                {
                    PrintMalformedDeclaration(headerPath, lineCount);
                    return -1;
                }

                std::size_t baseTokenBegin = delimiterToken + 1;
                std::size_t baseTokenEnd = reflectionTokenEnd;

                TrimWhitespaceLeft(line, baseTokenBegin);
                TrimWhitespaceRight(line, baseTokenEnd);
                typeBase = line.substr(baseTokenBegin, baseTokenEnd - baseTokenBegin);
            }
            else
            {
                typeBase = "Reflection::NullType";
            }

            ReflectedType& parsedType = parsedTypes.emplace_back();
            parsedType.name = std::move(typeName);
            parsedType.base = std::move(typeBase);
            parsedType.headerPath = headerPath;
            parsedType.headerLine = lineCount;
        }
    }

    // Collect unique headers.
    std::set<fs::path> reflectedHeaders;
    for(const auto& type : parsedTypes)
    {
        reflectedHeaders.insert(type.headerPath);
    }

    // Create hash map of reflected type name strings for performance.
    ParsedTypeMap parsedTypeMap;
    for(std::size_t i = 0; i < parsedTypes.size(); ++i)
    {
        const ReflectedType& reflectedType = parsedTypes[i];
        auto result = parsedTypeMap.emplace(reflectedType.name, i);

        if(!result.second)
        {
            const ReflectedType& duplicatedType = parsedTypes[result.first->second];

            std::cerr << "ReflectionGenerator: Found two reflected types with same names!\n"
                <<  "\t\"" << reflectedType.name << "\" from \""
                << reflectedType.headerPath.generic_string()
                << "(" << reflectedType.headerLine << ")\"\n"
                <<  "\t\"" << duplicatedType.name << "\" from \""
                << duplicatedType.headerPath.generic_string()
                << "(" << duplicatedType.headerLine << ")\"\n";

            return -1;
        }
    }

    // Perform topological sort of types by their dependencies to ensure
    //  that base types are always registered before their derived types.
    SortedTypeList sortedTypes;
    sortedTypes.reserve(parsedTypes.size());

    VisitedTypeList visitedTypes(parsedTypes.size(), false);
    for(std::size_t i = 0; i < parsedTypes.size(); ++i)
    {
        DependencyTypeStack dependencyStack;
        if(!VisitReflectedType(parsedTypes, parsedTypeMap, sortedTypes,
            visitedTypes, dependencyStack, i))
        {
            return - 1;
        }
    }

    // Generate reflection binding.
    std::ostringstream reflectionBinding;

    reflectionBinding <<
        "/*\n"
        "    Copyright(c) 2018 - 2021 Piotr Doan.All rights reserved.\n"
        "    Software distributed under the permissive MIT License.\n"
        "*/\n\n";

    reflectionBinding <<
        "#include <Common/Debug.hpp>\n"
        "#include <Reflection/Reflection.hpp>\n";

    reflectionBinding <<
        "#include \"" << targetName << "/ReflectionGenerated.hpp\"\n";

    for(const auto& header : reflectedHeaders)
    {
        fs::path relativeHeaderPath = fs::relative(header, fs::path(outputDir));
        reflectionBinding <<
            "#include \"" << relativeHeaderPath.generic_string() << "\"\n";
    }

    reflectionBinding <<
        "\n"
        "namespace Reflection::Generated\n"
        "{\n"
        "    void RegisterModule" << targetName << "()\n"
        "    {\n"
        "        static bool registered = false;\n"
        "        if(registered)\n"
        "            return;\n";

    for(const auto& type : sortedTypes)
    {
        reflectionBinding <<
            "\n        ASSERT_EVALUATE(REFLECTION_REGISTER_TYPE(" << type->name << "));";

#if 0 // Disabled as this makes generated binding file too sensitive to changes.
        reflectionBinding <<
            " // " << type->headerPath.generic_string() << "(" << type->headerLine << ")";
#endif

        reflectionBinding <<
            "\n";
    }

    reflectionBinding <<
        "\n"
        "        registered = true;\n"
        "    }\n"
        "}\n";

    // Determine reflection binding file path.
    std::string reflectionBindingFilename;
    reflectionBindingFilename += "ReflectionGenerated";
    reflectionBindingFilename += ".cpp";

    fs::path reflectionBindingFilePath = fs::path(outputDir) / reflectionBindingFilename;

    // Check existing reflection binding file.
    std::ifstream existingBindingFile(reflectionBindingFilePath);
    std::string existingReflectionBinding;

    if(existingBindingFile.good())
    {
        existingBindingFile.seekg(0, std::ios::end);
        existingReflectionBinding.reserve(existingBindingFile.tellg());
        existingBindingFile.seekg(0, std::ios::beg);

        existingReflectionBinding.assign(
        std::istreambuf_iterator<char>(existingBindingFile),
        std::istreambuf_iterator<char>());
    }

    if(reflectionBinding.str() == existingReflectionBinding)
        return 0;

    existingBindingFile.close();

    // Create new reflection binding file.
    std::ofstream reflectionBindingFile(reflectionBindingFilePath);
    
    if(!reflectionBindingFile.is_open())
    {
        std::cerr << "ReflectionGenerator: Failed to open file for writing - \""
            << reflectionBindingFilePath.generic_string() << "\"\n";
        return -1;
    }

    reflectionBindingFile << reflectionBinding.str();

    if(!reflectionBindingFile.good())
    {
        std::cerr << "ReflectionGenerator: Failed to write file - \""
            << reflectionBindingFilePath.generic_string() << "\"\n";
        return -1;
    }

    reflectionBindingFile.close();

    return 0;
}
