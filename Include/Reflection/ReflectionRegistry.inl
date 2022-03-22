/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

namespace Reflection
{
    template<typename Type>
    void Registry::ValidateType()
    {
        constexpr auto StaticType = DecayedStaticTypeInfo<Type>();
        static_assert(StaticType.Reflected, "Cannot register not reflected type!");

        ASSERT(StaticType.Identifier != InvalidIdentifier, "Attempted to register type "
            "\"{}\" ({}) with static identifier equal to invalid identifier!",
            StaticType.Name, StaticType.Identifier);
    }

    template<typename Type>
    DynamicTypeInfo* Registry::RegisterTypeInfo()
    {
        ValidateType<Type>();

        constexpr auto StaticType = DecayedStaticTypeInfo<Type>();
        DynamicTypeInfo& dynamicType = Type::GetTypeStorage().m_dynamicType;
        auto result = m_types.emplace(StaticType.Identifier, dynamicType);

        if(!result.second)
        {
#ifdef NAME_REGISTRY_ENABLED
            if(dynamicType.GetName().GetString() != StaticType.Name)
            {
                ASSERT(false, "Detected name hash collision between types \"{}\" ({})"
                    " and \"{}\" ({})!", StaticType.Name, StaticType.Identifier,
                    dynamicType.GetName().GetString(), dynamicType.GetIdentifier());
                return nullptr;
            }
#endif

            if(dynamicType.IsRegistered())
            {
                LOG_WARNING("Attempted to register type \"{}\" ({}) twice!",
                    StaticType.Name, dynamicType.GetIdentifier());
                return nullptr;
            }

            ASSERT(false, "Unknown registration error for type \"{}\" ({})",
                StaticType.Name, dynamicType.GetIdentifier());
            return nullptr;
        }

        return &dynamicType;
    }

    template<typename Type>
    bool Registry::RegisterBasics(DynamicTypeInfo* typeInfo)
    {
        constexpr auto StaticType = DecayedStaticTypeInfo<Type>();

        typeInfo->SetName(NAME(StaticType.Name));
        if constexpr(StaticType.IsConstructible())
        {
            typeInfo->SetConstructible(
                []() -> void*
                {
                    return new Type();
                }
            );
        }

        return true;
    }

    template<typename Type>
    bool Registry::RegisterBaseType(DynamicTypeInfo* typeInfo)
    {
        constexpr auto StaticType = DecayedStaticTypeInfo<Type>();

        using BaseType = typename decltype(StaticType)::BaseType;
        static_assert(std::is_same<typename Type::Super, BaseType>::value,
            "Mismatched base types between dynamic and static reflection declarations!");

        DynamicTypeInfo* baseType = FindTypeInfo(StaticType.GetBaseType().Identifier);
        if(baseType == nullptr)
        {
            if(!StaticType.GetBaseType().IsNullType())
            {
                LOG_WARNING("Attempted to register type \"{}\" ({}) with unregistered "
                    "base type \"{}\" ({})!", StaticType.Name, StaticType.Identifier,
                    StaticType.GetBaseType().Name, StaticType.GetBaseType().Identifier);
                return false;
            }
        }
        else
        {
            if(!baseType->IsNullType())
            {
                ASSERT(baseType->IsRegistered(), "Retrieved unregistered non-null base type "
                    "info pointer for type \"{}\" ({})!", StaticType.Name, StaticType.Identifier);
            }
        }

        typeInfo->SetBaseType(baseType);
        return true;
    }

    template<typename Type>
    bool Registry::RegisterType()
    {
        DynamicTypeInfo* typeInfo = RegisterTypeInfo<Type>();
        if(typeInfo == nullptr)
            return false;

        if(!RegisterBasics<Type>(typeInfo))
            return false;

        if(!RegisterBaseType<Type>(typeInfo))
            return false;

        typeInfo->MarkRegistered();
        LOG_DEBUG("Registered type: \"{}\" ({})", typeInfo->GetName(), typeInfo->GetIdentifier());
        return true;
    }
}
