/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Reflection/ReflectionDetail.hpp"

/*
    Reflection Dynamic

    Run-time reflection interface.
*/

namespace Reflection
{
    template<typename ReflectedType>
    struct StaticTypeInfo;

    template<typename ReflectedType>
    constexpr StaticTypeInfo<ReflectedType> StaticType();

    struct DynamicTypeInfo
    {
        DynamicTypeInfo() = default;

        template<typename ReflectedType>
        DynamicTypeInfo(const StaticTypeInfo<ReflectedType>& staticType) :
            Reflected(staticType.Reflected),
            Name(staticType.Name),
            Identifier(staticType.Identifier)
        {
        }

        bool Reflected = false;
        bool Registered = false;
        std::string_view Name = "<UnregisteredType>";
        IdentifierType Identifier = InvalidIdentifier;

        bool IsNullType() const;

        template<typename OtherType>
        bool IsType() const
        {
            // #todo: This needs to support polymorphisms!
            return Identifier == StaticType<OtherType>().Identifier;
        }
    };

    inline const DynamicTypeInfo& DynamicType(IdentifierType identifier)
    {
        return Detail::GetRegistry().LookupType(identifier);
    }

    template<typename RegisteredType>
    const DynamicTypeInfo& DynamicType(const RegisteredType& instance)
    {
        return Detail::GetRegistry().LookupType(StaticType<RegisteredType>().Identifier);
    }

    template<typename RegisteredType>
    constexpr bool IsRegistered()
    {
        return Detail::GetRegistry().LookupType(StaticType<RegisteredType>().Identifier).Registered;
    }

    template<typename RegisteredType>
    constexpr bool IsRegistered(const RegisteredType& instance)
    {
        return Detail::GetRegistry().LookupType(StaticType<RegisteredType>().Identifier).Registered;
    }

    inline bool IsRegistered(IdentifierType identifier)
    {
        return Detail::GetRegistry().LookupType(identifier).Registered;
    }
}
