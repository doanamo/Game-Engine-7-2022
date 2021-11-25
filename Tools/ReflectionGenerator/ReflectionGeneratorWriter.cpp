/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "ReflectionGeneratorTypes.hpp"
#include "ReflectionGeneratorWriter.hpp"
#include "ReflectionGeneratorParser.hpp"

ReflectionGeneratorWriter::ReflectionGeneratorWriter() = default;
ReflectionGeneratorWriter::~ReflectionGeneratorWriter() = default;

bool ReflectionGeneratorWriter::WriteReflectionBinding(
    const ReflectionGeneratorParameters& parameters,
    const ReflectionGeneratorParser& parser)
{
    // Generate reflection binding file.
    std::ostringstream reflectionBinding;

    reflectionBinding <<
        "/*\n"
        "    Copyright(c) 2018 - 2021 Piotr Doan.All rights reserved.\n"
        "    Software distributed under the permissive MIT License.\n"
        "*/\n\n";

    reflectionBinding <<
        "#include <Common/Debug.hpp>\n"
        "#include <Common/Profile.hpp>\n"
        "#include <Reflection/Reflection.hpp>\n";

    reflectionBinding <<
        "#include \"" << parameters.targetName << "/ReflectionGenerated.hpp\"\n";

    for(const auto& header : parser.GetReflectedHeaders())
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

    if(!parser.GetSortedTypes().empty())
    {
        reflectionBinding <<
        "\n";
    }

    for(const auto& type : parser.GetSortedTypes())
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
        return true;

    existingBindingFile.close();

    // Create new reflection binding file.
    std::ofstream reflectionBindingFile(reflectionBindingFilePath);
    
    if(!reflectionBindingFile.is_open())
    {
        std::cerr << "ReflectionGenerator: Failed to open file for writing - \""
            << reflectionBindingFilePath.generic_string() << "\"\n";
        return false;
    }

    reflectionBindingFile << reflectionBinding.str();

    if(!reflectionBindingFile.good())
    {
        std::cerr << "ReflectionGenerator: Failed to write file - \""
            << reflectionBindingFilePath.generic_string() << "\"\n";
        return false;
    }

    reflectionBindingFile.close();

    return true;
}
