/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Reflection/ReflectionDetail.hpp"
#include "Reflection/ReflectionStatic.hpp"

/*
    Reflection Dynamic

    Run-time reflection type info.
*/

namespace Reflection
{
    class BaseAttribute;

    template<typename ReflectedType>
    constexpr DecayedStaticTypeInfo<ReflectedType> StaticType();

    class DynamicTypeInfo final : private Common::NonCopyable
    {
    public:
        static_assert(std::is_same<TypeIdentifier, Common::Name::HashType>::value,
            "Both are a result of same string hash and should be interchangeable!");

        friend class Registry;
        static const DynamicTypeInfo Invalid;

        using ConstructFunction = void* (*)();
        using DynamicTypeList = std::vector<const DynamicTypeInfo*>;
        using AttributeList = std::vector<const BaseAttribute*>;
        
    public:
        DynamicTypeInfo() = default;
        ~DynamicTypeInfo() = default;

        void* Construct() const;

        bool IsNullType() const;
        bool IsType(TypeIdentifier identifier) const;
        bool IsBaseOf(TypeIdentifier identifier) const;
        bool IsDerivedFrom(TypeIdentifier identifier) const;
        bool HasAttribute(TypeIdentifier identifier) const;
        const BaseAttribute* GetAttribute(TypeIdentifier identifier) const;
        const BaseAttribute* GetAttributeByIndex(size_t index) const;

        bool IsRegistered() const
        {
            return m_registered;
        }

        bool IsConstructible() const
        {
            return m_constructFunction != nullptr;
        }

        bool HasBaseType() const
        {
            return m_registered && !m_baseType->IsNullType();
        }

        bool HasDerivedTypes() const
        {
            return !m_attributes.empty();
        }

        bool HasAttributes() const
        {
            return !m_attributes.empty();
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

        const AttributeList& GetAttributes() const
        {
            return m_attributes;
        }

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

        template<typename AttributeType>
        bool HasAttribute() const
        {
            return HasAttribute(StaticType<AttributeType>().Identifier);
        }

        template<typename AttributeType>
        const AttributeType* GetAttribute() const
        {
            const BaseAttribute* attribute = GetAttribute(StaticType<AttributeType>().Identifier);
            return reinterpret_cast<const AttributeType*>(attribute);
        }

    private:
        void SetName(const Common::Name& name);
        void SetConstructible(ConstructFunction constructFunction);
        void SetBaseType(DynamicTypeInfo* baseType);
        void AddAttribute(const BaseAttribute* attribute);
        void AddDerivedType(const DynamicTypeInfo* typeInfo);
        void MarkRegistered();

    private:
        Common::Name m_name = NAME("<UnregisteredType>");
        ConstructFunction m_constructFunction = nullptr;
        const DynamicTypeInfo* m_baseType = &Invalid;
        DynamicTypeList m_derivedTypes;
        AttributeList m_attributes;
        bool m_registered = false;
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
