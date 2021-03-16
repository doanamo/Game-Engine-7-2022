/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <unordered_map>
#include "Reflection/ReflectionStatic.hpp"
#include "Reflection/ReflectionDynamic.hpp"

/*
    Reflection Registry
*/

namespace Reflection
{
    class ReflectionRegistry final : public Detail::ReflectionRegistry
    {
    public:
        using Registry = std::unordered_map<IdentifierType, DynamicTypeInfo>;
        static DynamicTypeInfo InvalidTypeInfo;

    public:
        ReflectionRegistry();
        ~ReflectionRegistry();

        template<typename Type>
        void RegisterType();

        const DynamicTypeInfo& LookupType(IdentifierType identifier) const override;

    private:
        Registry m_registry;
    };

    template<typename Type>
    void ReflectionRegistry::RegisterType()
    {
        DynamicTypeInfo dynamicType{ Reflect<Type>() };

        if(!dynamicType.Reflected)
        {
            LOG_WARNING("Attempted to register type \"{}\" that is not reflected!",
                REFLECTION_STRINGIFY(Type));
            return;
        }

        auto result = m_registry.emplace(dynamicType.Identifier, std::move(dynamicType));
        DynamicTypeInfo& registeredType = result.first->second;

        if(!result.second && registeredType.Name != dynamicType.Name)
        {
            ASSERT(false, "Detected name hash collision between types \"{}\" ({}) and \"{}\" ({})!",
                registeredType.Name, registeredType.Identifier,
                dynamicType.Name, dynamicType.Identifier);
        }
        else
        {
            LOG_INFO("Registered reflection type: \"{}\" ({})",
                dynamicType.Name, dynamicType.Identifier);
        }
    }

    ReflectionRegistry& GetRegistry();
}
