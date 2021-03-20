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
        bool RegisterType();

        const DynamicTypeInfo& LookupType(IdentifierType identifier) const override;

    private:
        DynamicTypeInfo* FindTypeInfo(IdentifierType identifier);

    private:
        TypeInfoMap m_types;
    };

    template<typename Type>
    bool Registry::RegisterType()
    {
        constexpr auto staticType = StaticType<Type>();
        if(!staticType.Reflected)
        {
            LOG_WARNING("Attempted to register type \"{}\" that is not reflected!",
                REFLECTION_STRINGIFY(Type));
            return false;
        }

        ASSERT(staticType.Identifier != InvalidIdentifier, "Attempted to register type "
            "\"{}\" ({}) with static identifier equal to invalid identifier!",
            staticType.Name, staticType.Identifier);

        using BaseType = typename decltype(staticType)::BaseType;
        static_assert(std::is_same<typename Type::Super, BaseType>::value,
            "Mismatched base types between dynamic and static reflection declarations!");

        DynamicTypeInfo* baseType = FindTypeInfo(staticType.GetBaseType().Identifier);
        if(baseType == nullptr)
        {
            if(!staticType.GetBaseType().IsNullType())
            {
                LOG_WARNING("Attempted to register type \"{}\" ({}) with unregistered "
                    "base type \"{}\" ({})!", staticType.Name, staticType.Identifier, 
                    staticType.GetBaseType().Name, staticType.GetBaseType().Identifier);
                return false;
            }
        }
        else
        {
            ASSERT(baseType->IsRegistered(), "Retrieved unregistered non-null base type "
                "info pointer for type \"{}\" ({})!", staticType.Name, staticType.Identifier );
        }

        auto result = m_types.emplace(staticType.Identifier, Type::GetTypeStorage().DynamicType);
        DynamicTypeInfo& dynamicType = result.first->second;

        if(!result.second)
        {
            if(dynamicType.GetName() != staticType.Name)
            {
                ASSERT(false, "Detected name hash collision between types \"{}\" ({})"
                    " and \"{}\" ({})!", staticType.Name, staticType.Identifier,
                    dynamicType.GetName(), dynamicType.GetIdentifier());
            }
            else if(dynamicType.IsRegistered())
            {
                LOG_WARNING("Attempted to register type \"{}\" ({}) twice!",
                    dynamicType.GetName(), dynamicType.GetIdentifier());
            }
            else
            {
                ASSERT(false, "Unknown registration error for type \"{}\" ({})",
                    dynamicType.GetName(), dynamicType.GetIdentifier());
            }

            return false;
        }

        dynamicType.Register(staticType.Name, staticType.Identifier, baseType);
        LOG_INFO("Registered reflection type: \"{}\" ({})",
            dynamicType.GetName(), dynamicType.GetIdentifier());

        return true;
    }

    Registry& GetRegistry();
}

#define REFLECTION_REGISTER(Type) \
    Reflection::GetRegistry().RegisterType<Type>()
