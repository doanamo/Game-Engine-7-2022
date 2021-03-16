/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Reflection/ReflectionUtility.hpp"

/*
    Reflection Detail
*/

namespace Reflection
{
    using IdentifierType = uint32_t;
    constexpr IdentifierType InvalidIdentifier = 0;

    struct NullType;
    struct TypeAttribute;
    struct FieldAttribute;
    struct MethodAttribute;

    template<typename ReflectedType, typename AttributeType, std::size_t AttributeIndex>
    struct AttributeDescription;

    template<typename ReflectedType, typename AttributeType, std::size_t AttributeIndex>
    struct AttributeDescriptionWithInstance;

    template<typename ReflectedType, typename MemberType, std::size_t MemberIndex>
    struct MemberDescription;

    template<typename ReflectedType>
    constexpr bool IsReflected();

    struct DynamicTypeInfo;
}

namespace Reflection::Detail
{
    struct TypeInfoBase
    {
        using Type = NullType;
        using BaseType = NullType;

        static constexpr bool Reflected = false;
        static constexpr std::string_view Name = "<UnknownType>";
        static constexpr IdentifierType Identifier = InvalidIdentifier;
        static constexpr IdentifierType BaseTypeIdentifier = InvalidIdentifier;
        static constexpr auto Attributes = MakeEmptyObjectList();
        static constexpr auto Members = MakeEmptyObjectList();
    };

    template<typename Type>
    struct TypeInfo : public TypeInfoBase
    {
        template<std::size_t Index, typename ReflectedType, typename Dummy>
        struct MemberInfo;
    };

    template<typename ReflectedType, typename... AttributeTypes, std::size_t... AttributeIndices>
    constexpr ObjectList<AttributeDescriptionWithInstance<ReflectedType, AttributeTypes,
        AttributeIndices>...> MakeAttributeDescriptionWithInstanceList(
            const ObjectList<AttributeTypes...>& attributes,
            std::index_sequence<AttributeIndices...>)
    {
        return { std::make_tuple(AttributeDescriptionWithInstance<ReflectedType, AttributeTypes,
            AttributeIndices>{ attributes.template Get<AttributeIndices>() } ...) };
    }

    template<typename ReflectedType, typename... AttributeTypes, std::size_t... AttributeIndices>
    constexpr ObjectList<AttributeDescription<ReflectedType, AttributeTypes, AttributeIndices>...>
        MakeAttributeDescriptionWithoutInstanceList(const ObjectList<AttributeTypes...>& attributes,
            std::index_sequence<AttributeIndices...>)
    {
        return { std::make_tuple(AttributeDescription<
            ReflectedType, AttributeTypes, AttributeIndices>{} ...) };
    }

    template<typename ReflectedType, std::size_t MemberIndex>
    using MemberDescriptionType = MemberDescription<ReflectedType, typename TypeInfo<
        ReflectedType>::template MemberInfo<MemberIndex, ReflectedType, void>::Type, MemberIndex>;

    template<typename ReflectedType, std::size_t... MemberIndices>
    using MemberDescriptionList = ObjectList<
        MemberDescriptionType<ReflectedType, MemberIndices>...>;

    template<typename ReflectedType, std::size_t... MemberIndices>
    constexpr MemberDescriptionList<ReflectedType, MemberIndices...>
        MakeMemberDescriptionList(std::index_sequence<MemberIndices...>)
    {
        return { std::make_tuple(MemberDescriptionType<ReflectedType, MemberIndices>{} ...) };
    }

    template<typename... Attributes>
    constexpr bool ValidateAttributeReflection()
    {
        return (... && IsReflected<Attributes>());
    }

    template<typename FirstAttribute, typename... Attributes>
    constexpr bool ValidateAttributeUniqueness()
    {
        return (... && (!std::is_same_v<FirstAttribute, Attributes>
            && ValidateAttributeUniqueness<Attributes...>()));
    }

    template<typename Requirement, typename... Attributes>
    constexpr bool ValidateAttributeUsage()
    {
        return (... && std::is_base_of_v<Requirement, Attributes>);
    }

    template<typename Requirement>
    constexpr ObjectList<> MakeAttributeList()
    {
        return MakeEmptyObjectList();
    }

    template<typename Requirement, typename... Attributes>
    constexpr ObjectList<Attributes...> MakeAttributeList(Attributes&&... attributes)
    {
        static_assert(ValidateAttributeReflection<Attributes...>(),
            "Detected attribute that is not reflected!");
        static_assert(ValidateAttributeUniqueness<Attributes...>(),
            "Detected attribute that is not unique!");
        static_assert(ValidateAttributeUsage<Requirement, Attributes...>(),
            "Detected attribute with incorrect usage!");

        return { std::make_tuple(attributes...) };
    }

    template<typename ReflectedType, std::size_t MemberIndex>
    using MemberEntry = typename TypeInfo<ReflectedType>::template MemberInfo<MemberIndex>;

    template<typename ReflectedType, std::size_t... MemberIndices>
    constexpr ObjectList<MemberEntry<ReflectedType, MemberIndices>...>
        MakeMemberList(std::index_sequence<MemberIndices...>)
    {
        return { std::make_tuple(MemberEntry<ReflectedType, MemberIndices>{} ...) };
    }

    class ReflectionRegistry
    {
    public:
        virtual ~ReflectionRegistry() = default;

        virtual const DynamicTypeInfo& LookupType(IdentifierType identifier) const = 0;
    };

    const ReflectionRegistry& GetRegistry();
}
