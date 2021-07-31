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
    class Registry final : public Detail::ReflectionRegistry
    {
    public:
        using TypeInfoMap = std::unordered_map<TypeIdentifier, DynamicTypeInfo&>;

    public:
        Registry();
        ~Registry();

        template<typename Type>
        bool RegisterType();

        const DynamicTypeInfo& LookupType(TypeIdentifier identifier) const override;

    private:
        DynamicTypeInfo* FindTypeInfo(TypeIdentifier identifier);

    private:
        TypeInfoMap m_types;
    };

    template<typename Type>
    bool Registry::RegisterType()
    {
        constexpr auto StaticType = DecayedStaticTypeInfo<Type>();
        static_assert(StaticType.Reflected, "Cannot register not reflected type!");

        ASSERT(StaticType.Identifier != InvalidIdentifier, "Attempted to register type "
            "\"{}\" ({}) with static identifier equal to invalid identifier!",
            StaticType.Name, StaticType.Identifier);

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
            ASSERT(baseType->IsRegistered(), "Retrieved unregistered non-null base type "
                "info pointer for type \"{}\" ({})!", StaticType.Name, StaticType.Identifier);
        }

        auto result = m_types.emplace(StaticType.Identifier, Type::GetTypeStorage().m_dynamicType);
        DynamicTypeInfo& dynamicType = result.first->second;

        if(!result.second)
        {
#ifdef NAME_REGISTRY_ENABLED
            if(dynamicType.GetName().GetString() != StaticType.Name)
            {
                ASSERT(false, "Detected name hash collision between types \"{}\" ({})"
                    " and \"{}\" ({})!", StaticType.Name, StaticType.Identifier,
                    dynamicType.GetName().GetString(), dynamicType.GetIdentifier());
            }
#endif

            if(dynamicType.IsRegistered())
            {
                LOG_WARNING("Attempted to register type \"{}\" ({}) twice!",
                    StaticType.Name, dynamicType.GetIdentifier());
            }
            else
            {
                ASSERT(false, "Unknown registration error for type \"{}\" ({})",
                    StaticType.Name, dynamicType.GetIdentifier());
            }

            return false;
        }

        DynamicTypeInfo::ConstructFunction constructFunction = nullptr;
        if constexpr(StaticType.IsConstructible())
        {
            constructFunction = []() -> void*
            {
                return new Type();
            };
        }

        dynamicType.Register(NAME_CONSTEXPR(StaticType.Name), constructFunction, baseType);
        LOG_TRACE("Registered type: \"{}\" ({})", StaticType.Name, dynamicType.GetIdentifier());

        return true;
    }

    Registry& GetRegistry();
}

#define REFLECTION_REGISTER_TYPE(Type) \
    Reflection::GetRegistry().RegisterType<Type>()
