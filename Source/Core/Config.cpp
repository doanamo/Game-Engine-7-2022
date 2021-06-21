/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Precompiled.hpp"
#include "Core/Config.hpp"
using namespace Core;

Config::Config() = default;
Config::~Config() = default;

void Config::Load(const VariableArray& variables)
{
    for(const auto& variable : variables)
    {
        Set<std::string>(variable.first, variable.second);
    }
}
