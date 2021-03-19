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

    class Registry final : public Detail::ReflectionRegistry
    {
    public:
        using TypeInfoMap = std::unordered_map<IdentifierType, DynamicTypeInfo&>;

    public:
        Registry();
        ~Registry();

        template<typename Type>
        void RegisterType();

        const DynamicTypeInfo& LookupType(IdentifierType identifier) const override;

    private:
        TypeInfoMap m_types;
    };

    template<typename Type>
    void Registry::RegisterType()
    {
        constexpr auto staticType = StaticType<Type>();
        if(!staticType.Reflected)
        {
            LOG_WARNING("Attempted to register type \"{}\" that is not reflected!",
                REFLECTION_STRINGIFY(Type));
            return;
        }

        static_assert(std::is_same<Type::Super, decltype(staticType)::BaseType>::value,
            "Mismatched base types between dynamic and static reflection declarations!");

        const DynamicTypeInfo& baseType = LookupType(staticType.GetBaseType().Identifier);
        if(!baseType.Registered && !staticType.GetBaseType().IsNullType() )
        {
            LOG_WARNING("Attempter to register type \"{}\" with unregistered base type \"{}\"!",
                staticType.Name, staticType.GetBaseType().Name);
            return;
        }

        auto result = m_types.emplace(staticType.Identifier, Type::GetTypeStorage().DynamicType);
        DynamicTypeInfo& dynamicType = result.first->second;

        if(!result.second && dynamicType.Name != staticType.Name)
        {
            ASSERT(false, "Detected name hash collision between types \"{}\" ({}) and \"{}\" ({})!",
                staticType.Name, staticType.Identifier, dynamicType.Name, dynamicType.Identifier);
            return;
        }
        
        dynamicType.Registered = true;
        dynamicType.Name = staticType.Name;
        dynamicType.Identifier = staticType.Identifier;
        dynamicType.BaseType = &baseType;

        if(staticType.IsNullType())
        {
            dynamicType.BaseType = &dynamicType;
        }

        LOG_INFO("Registered reflection type: \"{}\" ({})",
            dynamicType.Name, dynamicType.Identifier);
    }

    Registry& GetRegistry();
}

#define REFLECTION_REGISTER(Type) \
    Reflection::GetRegistry().RegisterType<Type>()
