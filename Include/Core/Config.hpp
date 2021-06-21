/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Core/Service.hpp"
#include "Core/ConfigTypes.hpp"

/*
    Config

    Stores engine parametrization for initialization and runtime.

    Calling Get() is not very efficient as marshalling from internal value to
    target type is performed. If variable needs to be retrieved every frame,
    then particular system should subscribe to its changes and cache them
    locally.
*/

namespace Core
{
    class Config final : public Service
    {
        REFLECTION_ENABLE(Config, Service)

    public:
        using VariablePair = std::pair<Common::Name, std::string>;
        using VariableMap = std::map<Common::Name, std::string>;
        using VariableArray = std::vector<VariablePair>;

    public:
        Config();
        ~Config() override;

        void Load(const VariableArray& variables);

        template<typename Type>
        Common::Result<Type, Type> Set(Common::Name variable, Type value, bool modify = false);

        template<typename Type>
        Common::Result<Type, void> Get(Common::Name variable);

    private:
        VariableMap m_variables;
    };

    template <typename Type>
    Common::Result<Type, Type> Config::Set(Common::Name variable, Type value, bool modify)
    {
        static_assert(ConfigValueType<Type>::IsSupported());

        auto it = m_variables.find(variable);
        if(it != m_variables.end())
        {
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
            m_variables.emplace(variable, ConfigValueType<Type>::Format(value));
            return Common::Success(value);
        }
    }

    template<typename Type>
    Common::Result<Type, void> Config::Get(Common::Name variable)
    {
        static_assert(ConfigValueType<Type>::IsSupported());

        auto it = m_variables.find(variable);
        if(it != m_variables.end())
        {
            if(auto result = ConfigValueType<Type>::Parse(it->second))
            {
                return Common::Success(result.Unwrap());
            }
            else
            {
                return Common::Failure();
            }
        }
        else
        {
            return Common::Failure();
        }
    }
}

REFLECTION_TYPE(Core::Config, Core::Service)
