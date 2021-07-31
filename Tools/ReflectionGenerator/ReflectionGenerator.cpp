/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <cassert>
#include <string>
#include <regex>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <unordered_map>

namespace fs = std::filesystem;

struct GeneratorParameters
{
    std::string_view targetType;
    std::string_view targetName;
    std::vector<std::string_view> targetDependencies;

    std::string_view outputDir;
    std::vector<fs::path> sourceDirs;

    bool isExecutable = false;
    bool isValid = false;
};

GeneratorParameters ParseCommandLineArguments(const int argc, const char* argv[])
{
    if(argc < 5)
    {
        std::cerr << "ReflectionGenerator: Unexpected number of arguments!\n";

        for(int arg = 0; arg < argc; ++arg)
        {
            std::cerr << "ReflectionGenerator: argv[" << arg << "] = " << argv[arg] << "\n";
        }

        return {};
    }

    GeneratorParameters output;
    output.targetType = argv[1];
    output.targetName = argv[2];

    const std::string_view dependencyList = argv[3];
    {
        std::size_t start = 0U, end = 0U;

        do
        {
            end = dependencyList.find(';', start);

            if(end == std::string::npos)
            {
                end = dependencyList.length();
            }

            const std::string_view token = dependencyList.substr(start, end - start);
            if(!token.empty())
            {
                if(!std::regex_match(std::string(token), std::regex("^[A-Za-z]+$")))
                {
                    std::cerr << "ReflectionGenerator: "
                        "Target dependency token contains invalid characters!\n";
                    std::cerr << "ReflectionGenerator: \"" << token << "\"\n";
                    return {};
                }

                output.targetDependencies.push_back(token);
            }

            start = end + 1;
        }
        while(start < dependencyList.length());
    }

    output.outputDir = argv[4];

    for(int arg = 5; arg < argc; ++arg)
    {
        output.sourceDirs.emplace_back(fs::path(argv[arg]));
    }

    output.isExecutable = (output.targetType == "EXECUTABLE");
    output.isValid = true;
    return output;
}

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
    // Parse command line arguments.
    const GeneratorParameters parameters = ParseCommandLineArguments(argc, argv);
    if(!parameters.isValid)
        return -1;

    // Create list of header files.
    std::vector<fs::path> headerFileList;
    
    for(const auto& sourceDirPath : parameters.sourceDirs)
    {
        if(!fs::exists(sourceDirPath))
        {
            std::cerr << "ReflectionGenerator: Source directory path does not exist - \""
                << sourceDirPath.generic_string() << "\"\n";
            return -1;
        }

        if(!fs::is_directory(sourceDirPath))
        {
            std::cerr << "ReflectionGenerator: Provided source path is not a directory!\n";
            std::cerr << "ReflectionGenerator: \"" << sourceDirPath.generic_string() << "\"\n";
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
        "#include \"" << parameters.targetName << "/ReflectionGenerated.hpp\"\n";

    for(const auto& header : reflectedHeaders)
    {
        fs::path relativeHeaderPath = fs::relative(header, fs::path(parameters.outputDir));
        reflectionBinding <<
            "#include \"" << relativeHeaderPath.generic_string() << "\"\n";
    }

    reflectionBinding <<
        "\n"
        "namespace Reflection::Generated\n"
        "{";

    if(parameters.isExecutable)
    {
        if(!parameters.targetDependencies.empty())
        {
            reflectionBinding <<
                "\n";
        }

        for(const auto& dependency : parameters.targetDependencies)
        {
            reflectionBinding <<
                "    void RegisterModule" << dependency << "();\n";
        }
    }

    reflectionBinding <<
        "\n"
        "    void RegisterModule" << parameters.targetName << "()\n"
        "    {\n"
        "        LOG(\"Registering types from " << parameters.targetName << " module...\");\n"
        "        LOG_SCOPED_INDENT();\n\n"
        "        static bool registered = false;\n"
        "        if(registered)\n"
        "            return;\n";

    if(!sortedTypes.empty())
    {
        reflectionBinding <<
        "\n";
    }

    for(const auto& type : sortedTypes)
    {
        reflectionBinding <<
            "        ASSERT_EVALUATE(REFLECTION_REGISTER_TYPE(" << type->name << "));";

        reflectionBinding <<
            "\n";
    }

    reflectionBinding <<
        "\n"
        "        registered = true;\n"
        "    }\n";

    if(parameters.isExecutable)
    {
        reflectionBinding <<
            "\n"
            "    void RegisterExecutable()\n"
            "    {\n"
            "        LOG_PROFILE_SCOPE(\"Register reflection types\");\n\n"
            "        {\n"
            "            LOG(\"Registering reflected types...\");\n"
            "            LOG_SCOPED_INDENT();\n\n";

        for(const auto& dependency : parameters.targetDependencies)
        {
            reflectionBinding <<
                "            RegisterModule" << dependency << "();\n";
        }

        reflectionBinding <<
            "            RegisterModule" << parameters.targetName << "();\n"
            "        }\n"
            "    }\n";
    }

    reflectionBinding <<
        "}\n";

    // Determine reflection binding file path.
    std::string reflectionBindingFilename;
    reflectionBindingFilename += "ReflectionGenerated";
    reflectionBindingFilename += ".cpp";

    fs::path reflectionBindingFilePath =
        fs::path(parameters.outputDir) / reflectionBindingFilename;

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
