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
    if(argc != 4)
    {
        std::cerr << "ReflectionBinding: Unexpected number of arguments!";
        return -1;
    }

    std::string_view targetName = argv[1];
    std::string_view outputDir = argv[2];
    std::string_view sourceDirs = argv[3];

    // Parse list of source directories.
    std::size_t sourceDirBegin = 0U;
    std::size_t sourceDirEnd = std::string::npos;
    std::vector<std::string_view> sourceDirList;

    do 
    {
        sourceDirEnd = sourceDirs.find(';', sourceDirBegin);
        sourceDirList.push_back(sourceDirs.substr(sourceDirBegin, sourceDirEnd - sourceDirBegin));
        sourceDirBegin = sourceDirEnd + 1;
    }
    while (sourceDirEnd != std::string::npos);

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
        ReflectedHeader& reflectedHeader = reflectedHeaders.emplace_back();
        reflectedHeader.path = headerPath;

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

            std::string_view reflectionTokenName = "REFLECTION_ENABLE(";
            std::size_t reflectionTokenBegin = line.find(reflectionTokenName);
            std::size_t reflectionTokenEnd = line.find(')', reflectionTokenBegin);
            if(reflectionTokenBegin == std::string::npos)
                continue;

            if(reflectionTokenEnd == std::string::npos)
            {
                PrintMalformedDeclaration();
                return -1;
            }

            std::size_t delimeterToken = line.find(',', reflectionTokenBegin);

            std::size_t typeTokenBegin = reflectionTokenBegin + reflectionTokenName.size();
            std::size_t typeTokenEnd = delimeterToken != std::string::npos ?
                delimeterToken : reflectionTokenEnd;

            ReflectedType& reflectedType = reflectedHeader.types.emplace_back();

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
    }

    // Create binding source file.
    std::string reflectionBindingFilename;
    reflectionBindingFilename += "ReflectionGenerated";
    reflectionBindingFilename += ".cpp";

    fs::path reflectionBindingPath = fs::path(outputDir) / reflectionBindingFilename;
    std::ofstream reflectionBindingFile(reflectionBindingPath);

    if(!reflectionBindingFile.is_open())
    {
        std::cerr << "ReflectionBinding: Failed to open file for writing - \""
            << reflectionBindingPath << "\"";
        return -1;
    }

    reflectionBindingFile <<
        "/*\n"
        "    Copyright(c) 2018 - 2021 Piotr Doan.All rights reserved.\n"
        "    Software distributed under the permissive MIT License.\n"
        "*/\n\n";

    reflectionBindingFile <<
        "#include <Common/Debug.hpp>\n"
        "#include <Reflection/Reflection.hpp>\n";

    for(const auto& header : reflectedHeaders)
    {
        fs::path relativeHeaderPath = fs::relative(header.path, fs::path(outputDir));
        reflectionBindingFile <<
            "#include \"" << relativeHeaderPath.generic_string() << "\"\n";
    }

    reflectionBindingFile <<
        "\nvoid RegisterReflectionTypes_" << targetName << "()\n"
        "{\n"
        "    static bool registered = false;\n"
        "    if(registered)\n"
        "        return;\n\n";

    for(const auto& header : reflectedHeaders)
    {
        for(const auto& type : header.types)
        {
            reflectionBindingFile <<
                "    ASSERT(REFLECTION_REGISTER_TYPE(" << type.name << "));\n";
        }
    }

    reflectionBindingFile <<
        "    \n"
        "    registered = true;\n"
        "}\n";

    if(!reflectionBindingFile.good())
    {
        std::cerr << "ReflectionBinding: Failed to write file - \""
            << reflectionBindingPath << "\"";
        return -1;
    }

    reflectionBindingFile.close();

    return 0;
}
