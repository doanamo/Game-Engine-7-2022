/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

namespace fs = std::filesystem;

struct ReflectedType
{
    std::string name;
    std::string base;
};

struct ReflectedHeader
{
    fs::path path;
    std::vector<ReflectedType> types;
};

int main(int argc, const char* argv[])
{
    // Check provided arguments.
    if(argc < 4)
    {
        std::cerr << "ReflectionBinding: Unexpected number of arguments!\n";

        for(int arg = 0; arg < argc; ++arg)
        {
            std::cerr << "ReflectionBinding: argv[" << arg << "] = " << argv[arg] << "\n";
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
            std::cerr << "ReflectionBinding: Source directory path does not exist - \""
                << sourceDir << "\"";
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
    std::vector<ReflectedHeader> reflectedHeaders;

    for(const auto& headerPath : headerFileList)
    {
        std::vector<ReflectedType> types;

        std::ifstream file(headerPath);
        if(!file.is_open())
        {
            std::cerr << "ReflectionBinding: Failed to open header file - \""
                << headerPath << "\"";
            return -1;
        }

        std::string line;
        std::size_t lineCount = 0;

        auto PrintMalformedDeclaration = [&lineCount, &headerPath]()
        {
            std::cerr << "ReflectionBinding: Detected malformed REFLECTION_ENABLE() declaration"
                " in line " << lineCount << " of header file - \"" << headerPath << "\"";
        };

        auto TrimWhitespaceLeft = [](const std::string_view& string, std::size_t& position)
        {
            while(std::isspace(string.at(position)))
                ++position;
        };

        auto TrimWhitespaceRight = [](const std::string_view& string, std::size_t& position)
        {
            while(std::isspace(string.at(position)))
                --position;
        };

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
                PrintMalformedDeclaration();
                return -1;
            }

            std::size_t delimeterToken = line.find(',', reflectionTokenBegin);

            std::size_t typeTokenBegin = reflectionTokenBegin + reflectionTokenName.size();
            std::size_t typeTokenEnd = delimeterToken != std::string::npos ?
                delimeterToken : reflectionTokenEnd;

            ReflectedType& reflectedType = types.emplace_back();

            TrimWhitespaceLeft(line, typeTokenBegin);
            TrimWhitespaceRight(line, typeTokenEnd);
            reflectedType.name = line.substr(typeTokenBegin, typeTokenEnd - typeTokenBegin);

            if(delimeterToken != std::string::npos)
            {
                if(delimeterToken > reflectionTokenEnd)
                {
                    PrintMalformedDeclaration();
                    return -1;
                }

                std::size_t baseTokenBegin = delimeterToken + 1;
                std::size_t baseTokenEnd = reflectionTokenEnd;

                TrimWhitespaceLeft(line, baseTokenBegin);
                TrimWhitespaceRight(line, baseTokenEnd);
                reflectedType.base = line.substr(baseTokenBegin, baseTokenEnd - baseTokenBegin);
            }
            else
            {
                reflectedType.base = "Reflection::NullType";
            }
        }

        if(!types.empty())
        {
            ReflectedHeader& reflectedHeader = reflectedHeaders.emplace_back();
            reflectedHeader.path = headerPath;
            reflectedHeader.types = std::move(types);
        }
    }

    // Create binding source file.
    std::string sourceBindingFilename;
    sourceBindingFilename += "ReflectionGenerated";
    sourceBindingFilename += ".cpp";

    fs::path sourceBindingPath = fs::path(outputDir) / sourceBindingFilename;
    std::ofstream sourceBindingFile(sourceBindingPath);

    if(!sourceBindingFile.is_open())
    {
        std::cerr << "ReflectionBinding: Failed to open file for writing - \""
            << sourceBindingPath << "\"";
        return -1;
    }

    sourceBindingFile <<
        "/*\n"
        "    Copyright(c) 2018 - 2021 Piotr Doan.All rights reserved.\n"
        "    Software distributed under the permissive MIT License.\n"
        "*/\n\n";

    sourceBindingFile <<
        "#include <Common/Debug.hpp>\n"
        "#include <Reflection/Reflection.hpp>\n";

    for(const auto& header : reflectedHeaders)
    {
        fs::path relativeHeaderPath = fs::relative(header.path, fs::path(outputDir));
        sourceBindingFile <<
            "#include \"" << relativeHeaderPath.generic_string() << "\"\n";
    }

    sourceBindingFile <<
        "\n"
        "void RegisterReflectionTypes_" << targetName << "()\n"
        "{\n"
        "    static bool registered = false;\n"
        "    if(registered)\n"
        "        return;\n\n";

    for(const auto& header : reflectedHeaders)
    {
        for(const auto& type : header.types)
        {
            sourceBindingFile <<
                "    ASSERT(REFLECTION_REGISTER_TYPE(" << type.name << "));\n";
        }
    }

    sourceBindingFile <<
        "    \n"
        "    registered = true;\n"
        "}\n";

    if(!sourceBindingFile.good())
    {
        std::cerr << "ReflectionBinding: Failed to write file - \""
            << sourceBindingPath << "\"";
        return -1;
    }

    sourceBindingFile.close();

    return 0;
}
