/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Precompiled.hpp"
#include "ReflectionGeneratorTypes.hpp"
#include "ReflectionGeneratorUtility.hpp"

ReflectionGeneratorParameters ParseCommandLineArguments(const int argc, const char* argv[])
{
    // Ensure we have correct number of arguments.
    if(argc < 5)
    {
        std::cerr << "ReflectionGenerator: Unexpected number of arguments!\n";

        for(int arg = 0; arg < argc; ++arg)
        {
            std::cerr << "ReflectionGenerator: argv[" << arg << "] = " << argv[arg] << "\n";
        }

        return {};
    }

    // Read target name and type to differentiate between executables and libraries.
    // See https://cmake.org/cmake/help/latest/prop_tgt/TYPE.html for possible values.
    std::string_view targetType = argv[1];
    std::string_view targetName = argv[2];

    // Read target dependencies.
    // Dependencies are passed in CMake list, as one string without
    // white spaces and with elements separated by semicolon.
    const std::string_view dependencyString = argv[3];
    std::vector<std::string_view> targetDependencies;

    {
        std::size_t start = 0U, end = 0U;

        do
        {
            end = dependencyString.find(';', start);

            if(end == std::string::npos)
            {
                end = dependencyString.find(' ', start);
            }

            if(end == std::string::npos)
            {
                end = dependencyString.length();
            }

            const std::string_view token = dependencyString.substr(start, end - start);
            if(!token.empty())
            {
                if(!std::regex_match(std::string(token), std::regex("^[A-Za-z]+$")))
                {
                    std::cerr << "ReflectionGenerator: "
                        "Target dependency token contains invalid characters!\n";
                    std::cerr << "ReflectionGenerator: \"" << token << "\"\n";
                    return {};
                }

                targetDependencies.push_back(token);
            }

            start = end + 1;
        }
        while(start < dependencyString.length());
    }

    // Read target output directory for generated reflection bindings.
    std::string_view outputDir = argv[4];

    // Read target source directories where to look for reflection declarations.
    std::vector<fs::path> sourceDirs;
    for(int arg = 5; arg < argc; ++arg)
    {
        sourceDirs.emplace_back(fs::path(argv[arg]));
    }

    // Return parsed parameters.
    ReflectionGeneratorParameters output;
    output.targetType = targetType;
    output.targetName = targetName;
    output.targetDependencies = std::move(targetDependencies);
    output.outputDir = outputDir;
    output.sourceDirs = std::move(sourceDirs);
    output.isExecutable = (output.targetType == "EXECUTABLE");
    output.isValid = true;
    return output;
}

void PrintMalformedDeclaration(const fs::path& headerPath, const std::size_t headerLine)
{
    std::cerr << "ReflectionGenerator: Detected malformed REFLECTION_ENABLE() declaration"
        " in \"" << headerPath << "(" << headerLine << ")\"\n";
};

void TrimWhiteSpacesLeft(const std::string_view& string, std::size_t& position)
{
    while(std::isspace(string.at(position)))
        ++position;
};

void TrimWhiteSpacesRight(const std::string_view& string, std::size_t& position)
{
    while(std::isspace(string.at(position)))
        --position;
};
