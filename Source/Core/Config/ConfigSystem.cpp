/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Core/Config/ConfigSystem.hpp"
#include "Core/System/SystemStorage.hpp"
using namespace Core;

ConfigSystem::ConfigSystem() = default;
ConfigSystem::~ConfigSystem() = default;

bool ConfigSystem::OnAttach(const EngineSystemStorage& systemStorage)
{
    for(const auto& variable : systemStorage.GetContext().initialConfigVars)
    {
        Set<std::string>(variable.first, variable.second);
    }

    return true;
}
