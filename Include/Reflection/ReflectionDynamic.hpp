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
        bool Registered = false;
        std::string_view Name = "<UnregisteredType>";
        IdentifierType Identifier = InvalidIdentifier;
        IdentifierType BaseTypeIdentifier = InvalidIdentifier;

        bool IsNullType() const;
        bool HasBaseType() const;

        const DynamicTypeInfo& GetBaseType() const;

        template<typename OtherType>
        bool IsType() const
        {
            // #todo: This needs to support polymorphism!
            return Registered && Identifier == StaticType<OtherType>().Identifier;
        }

        template<typename OtherType>
        bool IsDerivedFrom() const
        {
            // #todo: This needs to support polymorphism!
            return Registered && BaseTypeIdentifier == StaticType<OtherType>().Identifier;
        }

        template<typename OtherType>
        bool IsBaseOf() const
        {
            // #todo: This needs to support polymorphism!
            return Registered && Identifier == StaticType<OtherType>().BaseTypeIdentifier;
        }
    };

    inline const DynamicTypeInfo& DynamicType(IdentifierType identifier)
    {
        return Detail::GetRegistry().LookupType(identifier);
    }

    template<typename RegisteredType>
    constexpr const DynamicTypeInfo& DynamicType()
    {
        return Detail::GetRegistry().LookupType(StaticType<RegisteredType>().Identifier);
    }

    template<typename RegisteredType>
    constexpr const DynamicTypeInfo& DynamicType(const RegisteredType& instance)
    {
        return Detail::GetRegistry().LookupType(StaticType<RegisteredType>().Identifier);
    }

    inline bool IsRegistered(IdentifierType identifier)
    {
        return Detail::GetRegistry().LookupType(identifier).Registered;
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

    struct DynamicTypeStorage
    {
    public:
        const DynamicTypeInfo& GetTypeInfo() const
        {
            return DynamicType;
        }

    private:
        friend class Registry;
        DynamicTypeInfo DynamicType;
    };
}
