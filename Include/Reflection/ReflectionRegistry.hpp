/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <unordered_map>
#include "Reflection/ReflectionDetail.hpp"
#include "Reflection/ReflectionDynamic.hpp"

/*
    Reflection Registry
*/

namespace Reflection
{
    template<typename ReflectedType>
    constexpr StaticTypeInfo<ReflectedType> StaticType();

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
        constexpr auto staticType = StaticType<Type>();
        if(!staticType.Reflected)
        {
            LOG_WARNING("Attempted to register type \"{}\" that is not reflected!",
                REFLECTION_STRINGIFY(Type));
            return;
        }

        DynamicTypeInfo dynamicType{ StaticType<Type>() };
        auto result = m_registry.emplace(staticType.Identifier, std::move(dynamicType));
        DynamicTypeInfo& registeredType = result.first->second;

        if(!result.second && registeredType.Name != staticType.Name)
        {
            ASSERT(false, "Detected name hash collision between types \"{}\" ({}) and \"{}\" ({})!",
                registeredType.Name, registeredType.Identifier,
                dynamicType.Name, dynamicType.Identifier);
        }
        else
        {
            registeredType.Registered = true;
            LOG_INFO("Registered reflection type: \"{}\" ({})",
                registeredType.Name, registeredType.Identifier);
        }
    }

    ReflectionRegistry& GetRegistry();
}

#define REFLECTION_REGISTER(Type) \
    Reflection::GetRegistry().RegisterType<Type>()
