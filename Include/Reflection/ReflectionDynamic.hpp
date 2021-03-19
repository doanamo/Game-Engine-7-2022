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
    struct NullType;

    template<typename ReflectedType>
    struct StaticTypeInfo;

    template<typename ReflectedType>
    constexpr StaticTypeInfo<ReflectedType> StaticType();

    template<typename ReflectedType>
    constexpr IdentifierType GetIdentifier();

    class DynamicTypeInfo final
    {
    public:
        friend class Registry;
        static const DynamicTypeInfo Invalid;
        
        DynamicTypeInfo() = default;
        ~DynamicTypeInfo() = default;

        bool IsRegistered() const
        {
            return m_registered;
        }

        bool IsNullType() const
        {
            return m_registered && m_identifier == Reflection::GetIdentifier<NullType>();
        }

        bool HasBaseType() const
        {
            return m_registered && !m_baseType->IsNullType();
        }

        const std::string_view& GetName() const
        {
            return m_name;
        }

        IdentifierType GetIdentifier() const
        {
            return m_identifier;
        }

        const DynamicTypeInfo& GetBaseType() const
        {
            return *m_baseType;
        }

        template<typename OtherType>
        bool IsType() const
        {
            // #todo: This needs to support polymorphism!
            return m_registered && m_identifier == StaticType<OtherType>().Identifier;
        }

        template<typename OtherType>
        bool IsBaseOf() const
        {
            // #todo: This needs to support polymorphism!
            return m_registered && m_identifier == StaticType<OtherType>().GetBaseType().Identifier;
        }

        template<typename OtherType>
        bool IsDerivedFrom() const
        {
            // #todo: This needs to support polymorphism!
            return m_registered && m_baseType->m_identifier == StaticType<OtherType>().Identifier;
        }

    private:
        void Register(std::string_view name, IdentifierType identifier, DynamicTypeInfo* baseType);

        bool m_registered = false;
        std::string_view m_name = "<UnregisteredType>";
        IdentifierType m_identifier = InvalidIdentifier;
        const DynamicTypeInfo* m_baseType = &Invalid;
        std::vector<const DynamicTypeInfo*> m_derivedTypes;
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
        return Detail::GetRegistry().LookupType(identifier).IsRegistered();
    }

    template<typename RegisteredType>
    constexpr bool IsRegistered()
    {
        return Detail::GetRegistry().LookupType(
            StaticType<RegisteredType>().Identifier).IsRegistered();
    }

    template<typename RegisteredType>
    constexpr bool IsRegistered(const RegisteredType& instance)
    {
        return Detail::GetRegistry().LookupType(
            StaticType<RegisteredType>().Identifier).IsRegistered();
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
