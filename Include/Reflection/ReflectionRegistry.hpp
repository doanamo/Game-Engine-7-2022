/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <unordered_map>
#include "Reflection/ReflectionDetail.hpp"
#include "Reflection/ReflectionDynamic.hpp"
#include "Reflection/ReflectionUtility.hpp"

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
        constexpr auto staticType = DecayedStaticTypeInfo<Type>();
        static_assert(staticType.Reflected, "Cannot register not reflected type!");

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
#ifdef NAME_REGISTRY_ENABLED
            if(dynamicType.GetName().GetString() != staticType.Name)
            {
                ASSERT(false, "Detected name hash collision between types \"{}\" ({})"
                    " and \"{}\" ({})!", staticType.Name, staticType.Identifier,
                    dynamicType.GetName(), dynamicType.GetIdentifier());
            }
#endif

            if(dynamicType.IsRegistered())
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

        static_assert(std::is_constructible<Type>::value,
            "Reflected type must be constructible without any parameters!");

        auto instantiateFunction = []() -> void*
        {
            return new Type();
        };

        dynamicType.Register(staticType.Name, instantiateFunction, baseType);
        LOG_INFO("Registered reflection type: \"{}\" ({})",
            dynamicType.GetName(), dynamicType.GetIdentifier());

        return true;
    }

    Registry& GetRegistry();
}

#define REFLECTION_REGISTER_TYPE(Type) \
    Reflection::GetRegistry().RegisterType<Type>()
