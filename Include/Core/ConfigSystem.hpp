/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Core/EngineSystem.hpp"
#include "Core/ConfigTypes.hpp"

/*
    Config

    Stores engine parametrization for initialization and runtime.

    Calling Get() is not very efficient as marshalling from internal value to target type is
    performed. If variable needs to be retrieved every frame, then particular system should
    subscribe to its changes and cache them locally.
*/

namespace Core
{
    class ConfigSystem final : public EngineSystem
    {
        REFLECTION_ENABLE(ConfigSystem, EngineSystem)

    public:
        using VariableMap = std::map<Common::Name, std::string>;

    public:
        ConfigSystem();
        ~ConfigSystem() override;

        void Load(const ConfigVariableArray& variables);

        template<typename Type>
        Common::Result<Type, Type> Set(Common::Name variable, Type value, bool modify = false);

        template<typename Type>
        Common::Result<Type, void> Get(Common::Name variable);

    private:
        VariableMap m_variables;
    };

    template <typename Type>
    Common::Result<Type, Type> ConfigSystem::Set(Common::Name variable, Type value, bool modify)
    {
        static_assert(ConfigValueType<Type>::IsSupported());

        auto it = m_variables.find(variable);
        if(it != m_variables.end())
        {
            // If variable does exist, set new value only if modify flag is true.
            if(modify)
            {
                it->second = ConfigValueType<Type>::Format(value);
                return Common::Success(value);
            }
            else
            {
                return Common::Failure(ConfigValueType<Type>::Parse(it->second).Unwrap());
            }
        }
        else
        {
            // If variable does not exists, create it with new value.
            m_variables.emplace(variable, ConfigValueType<Type>::Format(value));
            return Common::Success(value);
        }
    }

    template<typename Type>
    Common::Result<Type, void> ConfigSystem::Get(Common::Name variable)
    {
        static_assert(ConfigValueType<Type>::IsSupported());

        auto it = m_variables.find(variable);
        if(it != m_variables.end())
        {
            if(auto result = ConfigValueType<Type>::Parse(it->second))
            {
                return Common::Success(result.Unwrap());
            }
        }
        
        return Common::Failure();
    }
}

REFLECTION_TYPE(Core::ConfigSystem, Core::EngineSystem)
