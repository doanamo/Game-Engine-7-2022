/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Reflection Generator Types
*/

struct ReflectionGeneratorParameters
{
    std::string_view targetType;
    std::string_view targetName;
    std::vector<std::string_view> targetDependencies;

    std::string_view outputDir;
    std::vector<fs::path> sourceDirs;

    bool isExecutable = false;
    bool isValid = false;
};

struct ReflectedType
{
    std::string name;
    std::string base;

    fs::path headerPath;
    std::size_t headerLine = 0;
};

using HeaderFileList = std::vector<fs::path>;
using ReflectedHeaderList = std::set<fs::path>;
using ParsedTypeList = std::vector<ReflectedType>;
using SortedTypeList = std::vector<const ReflectedType*>;
using VisitedTypeList = std::vector<bool>;
using DependencyTypeStack = std::set<std::size_t>;
