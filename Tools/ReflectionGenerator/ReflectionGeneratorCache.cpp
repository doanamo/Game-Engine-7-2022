/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "ReflectionGeneratorTypes.hpp"
#include "ReflectionGeneratorCache.hpp"

ReflectionGeneratorCache::ReflectionGeneratorCache() = default;
ReflectionGeneratorCache::~ReflectionGeneratorCache() = default;

bool ReflectionGeneratorCache::CollectHeaderFiles(const ReflectionGeneratorParameters& parameters)
{
    if(!m_headerFiles.empty())
    {
        std::cerr << "ReflectionGenerator: Cannot collect source files more than once!\n";
        return false;
    }

    // Collect header files to parse.
    for(const auto& sourceDirPath : parameters.sourceDirs)
    {
        if(!fs::exists(sourceDirPath))
        {
            std::cerr << "ReflectionGenerator: Source directory path does not exist - \""
                << sourceDirPath.generic_string() << "\"\n";
            return false;
        }

        if(!fs::is_directory(sourceDirPath))
        {
            std::cerr << "ReflectionGenerator: Provided source path is not a directory!\n";
            std::cerr << "ReflectionGenerator: \"" << sourceDirPath.generic_string() << "\"\n";
            return false;
        }

        for(const auto& dirEntry : fs::recursive_directory_iterator(sourceDirPath))
        {
            if(!dirEntry.is_regular_file())
                continue;

            if(dirEntry.path().extension() == ".hpp" || dirEntry.path().extension() == ".h")
            {
                m_headerFiles.push_back(dirEntry);
            }
        }
    }

    return true;
}
