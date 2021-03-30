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
            std::cerr << "ReflectionGenerator: Failed to open header file - \""
                << headerPath << "\"";
            return -1;
        }

        std::string line;
        std::size_t lineCount = 0;

        auto PrintMalformedDeclaration = [&lineCount, &headerPath]()
        {
            std::cerr << "ReflectionGenerator: Detected malformed REFLECTION_ENABLE() declaration"
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
        fs::path relativeHeaderPath = fs::relative(header.path, fs::path(outputDir));
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
        "            return;\n\n";

    for(const auto& header : reflectedHeaders)
    {
        for(const auto& type : header.types)
        {
            reflectionBinding <<
                "        ASSERT_EVALUATE(REFLECTION_REGISTER_TYPE(" << type.name << "));\n";
        }
    }

    reflectionBinding <<
        "        \n"
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

    existingBindingFile.seekg(0, std::ios::end);
    existingReflectionBinding.reserve(existingBindingFile.tellg());
    existingBindingFile.seekg(0, std::ios::beg);

    existingReflectionBinding.assign(
        std::istreambuf_iterator<char>(existingBindingFile),
        std::istreambuf_iterator<char>());

    if(reflectionBinding.str() == existingReflectionBinding)
        return 0;

    existingBindingFile.close();

    // Create new reflection binding file.
    std::ofstream reflectionBindingFile(reflectionBindingFilePath);
    
    if(!reflectionBindingFile.is_open())
    {
        std::cerr << "ReflectionGenerator: Failed to open file for writing - \""
            << reflectionBindingFilePath.generic_string() << "\"";
        return -1;
    }

    reflectionBindingFile << reflectionBinding.str();

    if(!reflectionBindingFile.good())
    {
        std::cerr << "ReflectionGenerator: Failed to write file - \""
            << reflectionBindingFilePath.generic_string() << "\"";
        return -1;
    }

    reflectionBindingFile.close();

    return 0;
}
