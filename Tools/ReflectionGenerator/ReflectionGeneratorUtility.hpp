/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Reflection Generator Utility
*/

ReflectionGeneratorParameters ParseCommandLineArguments(const int argc, const char* argv[]);

void PrintMalformedDeclaration(const fs::path& headerPath, const std::size_t headerLine);

void TrimWhiteSpacesLeft(const std::string_view& string, std::size_t& position);
void TrimWhiteSpacesRight(const std::string_view& string, std::size_t& position);
