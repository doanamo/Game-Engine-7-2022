/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Precompiled.hpp"
#include "Core/ConfigSystem.hpp"
using namespace Core;

ConfigSystem::ConfigSystem() = default;
ConfigSystem::~ConfigSystem() = default;

void ConfigSystem::Load(const ConfigVariableArray& variables)
{
    for(const auto& variable : variables)
    {
        Set<std::string>(variable.first, variable.second);
    }
}
