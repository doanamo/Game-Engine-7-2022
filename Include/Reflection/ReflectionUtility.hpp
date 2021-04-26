/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Utility.hpp>
#include "Reflection/ReflectionDynamic.hpp"

/*
    Reflection Utility
*/

namespace Reflection
{
    template<typename ReflectedType>
    constexpr DecayedStaticTypeInfo<ReflectedType> StaticType()
    {
        return {};
    }

    template<typename ReflectedType>
    constexpr DecayedStaticTypeInfo<ReflectedType> StaticType(const ReflectedType& instance)
    {
        return {};
    }

    template<typename RegisteredType>
    constexpr const DynamicTypeInfo& DynamicType()
    {
        return RegisteredType::GetTypeStorage().GetTypeInfo();
    }

    template<typename RegisteredType>
    constexpr const DynamicTypeInfo& DynamicType(const RegisteredType& instance)
    {
        ASSERT(Common::Pointer(instance));
        return Common::Pointer(instance)->GetTypeInfo();
    }

    template<>
    inline const DynamicTypeInfo& DynamicType(const TypeIdentifier& identifier)
    {
        return Detail::GetRegistry().LookupType(identifier);
    }

    template<typename ReflectedType>
    constexpr bool IsReflected()
    {
        return StaticType<ReflectedType>().Reflected;
    }

    template<typename ReflectedType>
    constexpr bool IsReflected(const ReflectedType& type)
    {
        return StaticType<ReflectedType>().Reflected;
    }

    template<typename RegisteredType>
    constexpr bool IsRegistered()
    {
        return DynamicType<RegisteredType>().IsRegistered();
    }

    template<typename RegisteredType>
    constexpr bool IsRegistered(const RegisteredType& instance)
    {
        return DynamicType(instance).IsRegistered();
    }

    template<>
    inline bool IsRegistered(const TypeIdentifier& identifier)
    {
        return DynamicType(identifier).IsRegistered();
    }

    template<typename ReflectedType>
    constexpr TypeIdentifier GetIdentifier()
    {
        return StaticType<ReflectedType>().Identifier;
    }

    template<typename RegisteredType>
    constexpr TypeIdentifier GetIdentifier(const RegisteredType& instance)
    {
        return DynamicType(instance).GetIdentifier();
    }

    template<std::size_t Size>
    constexpr TypeIdentifier GetIdentifier(const char (&typeName)[Size])
    {
        return Common::StringHash<TypeIdentifier>(typeName);
    }

    template<typename ReflectedType>
    constexpr Common::Name GetName()
    {
        return NAME_CONSTEXPR(StaticType<ReflectedType>().Name);
    }

    template<typename RegisteredType>
    constexpr Common::Name GetName(const RegisteredType& instance)
    {
        return DynamicType(instance).GetName();
    }

    template<>
    inline Common::Name GetName(const TypeIdentifier& identifier)
    {
        return DynamicType(identifier).GetName();
    }

    template<typename RegisteredType>
    RegisteredType* Construct()
    {
        return static_cast<RegisteredType*>(
            RegisteredType::GetTypeStorage().GetTypeInfo().Construct());
    }

    template<typename RegisteredType>
    RegisteredType* Construct(const TypeIdentifier identifier)
    {
        const DynamicTypeInfo& typeInfo = Detail::GetRegistry().LookupType(identifier);
        if(typeInfo.IsType<RegisteredType>())
        {
            return static_cast<RegisteredType*>(typeInfo.Construct());
        }

        return nullptr;
    }

    template<typename TargetType, typename SourceType>
    TargetType* Cast(SourceType* instance)
    {
        if(instance && instance->GetTypeInfo().IsType(StaticType<TargetType>().Identifier))
        {
            return reinterpret_cast<TargetType*>(instance);
        }

        return nullptr;
    }

    template<typename TargetType, typename SourceType>
    std::unique_ptr<TargetType> Cast(std::unique_ptr<SourceType>& instance)
    {
        if(instance && instance->GetTypeInfo().IsType(StaticType<TargetType>().Identifier))
        {
            return std::unique_ptr<TargetType>(reinterpret_cast<TargetType*>(instance.release()));
        }

        return nullptr;
    }
}

#define REFLECTION_IDENTIFIER(Type) \
    Reflection::StaticType<Type>().Identifier
