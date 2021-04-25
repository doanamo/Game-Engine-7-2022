/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Name.hpp>
#include "Reflection/ReflectionDetail.hpp"
#include "Reflection/ReflectionStatic.hpp"

/*
    Reflection Dynamic

    Run-time reflection interface.
*/

namespace Reflection
{
    template<typename ReflectedType>
    constexpr DecayedStaticTypeInfo<ReflectedType> StaticType();

    template<typename ReflectedType>
    constexpr TypeIdentifier GetIdentifier();

    class DynamicTypeInfo final : private Common::NonCopyable
    {
    public:
        static_assert(std::is_same<TypeIdentifier, Common::Name::HashType>::value,
            "Both are a result of same string hash and should be interchangeable!");

        friend class Registry;
        static const DynamicTypeInfo Invalid;

        using DynamicTypeList = std::vector<std::reference_wrapper<const DynamicTypeInfo>>;
        using ConstructFunction = void* (*)();
        
    public:
        DynamicTypeInfo() = default;
        ~DynamicTypeInfo() = default;

        void* Construct() const;

        bool IsRegistered() const
        {
            return m_registered;
        }

        bool IsNullType() const 
        {
            return m_registered && GetIdentifier() == Reflection::GetIdentifier<NullType>();
        }

        bool IsConstructible() const
        {
            return m_constructFunction != nullptr;
        }

        bool HasBaseType() const
        {
            return m_registered && !m_baseType->IsNullType();
        }

        const Common::Name& GetName() const
        {
            return m_name;
        }

        TypeIdentifier GetIdentifier() const
        {
            return m_name.GetHash();
        }

        const DynamicTypeInfo& GetBaseType() const
        {
            return *m_baseType;
        }

        const DynamicTypeList& GetDerivedTypes() const
        {
            return m_derivedTypes;
        }

        bool IsType(TypeIdentifier identifier) const;
        bool IsBaseOf(TypeIdentifier identifier) const;
        bool IsDerivedFrom(TypeIdentifier identifier) const;

        template<typename OtherType>
        bool IsType() const
        {
            return IsType(StaticType<OtherType>().Identifier);
        }

        template<typename OtherType>
        bool IsType(const OtherType& instance) const
        {
            return IsType(instance.GetTypeInfo().GetIdentifier());
        }

        template<typename OtherType>
        bool IsBaseOf() const
        {
            return IsBaseOf(StaticType<OtherType>().Identifier);
        }

        template<typename OtherType>
        bool IsBaseOf(const OtherType& instance) const
        {
            return IsBaseOf(instance.GetTypeInfo().GetIdentifier());
        }

        template<typename OtherType>
        bool IsDerivedFrom() const
        {
            return IsDerivedFrom(StaticType<OtherType>().Identifier);
        }

        template<typename OtherType>
        bool IsDerivedFrom(const OtherType& instance) const
        {
            return IsDerivedFrom(instance.GetTypeInfo().GetIdentifier());
        }

    private:
        void Register(const Common::Name& name,
                      ConstructFunction constructFunction,
                      DynamicTypeInfo* baseType);

        void AddDerivedType(const DynamicTypeInfo& typeInfo);

        bool m_registered = false;
        Common::Name m_name = NAME_CONSTEXPR("<UnregisteredType>");
        ConstructFunction m_constructFunction = nullptr;
        const DynamicTypeInfo* m_baseType = &Invalid;
        DynamicTypeList m_derivedTypes;
    };

    class DynamicTypeStorage
    {
    public:
        const DynamicTypeInfo& GetTypeInfo() const
        {
            return m_dynamicType;
        }

    private:
        friend class Registry;
        DynamicTypeInfo m_dynamicType;
    };
}
