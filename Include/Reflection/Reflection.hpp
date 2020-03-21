/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Reflection

    Implementation is based on excellent refl-cpp library:
    - https://github.com/veselink1/refl-cpp
    - https://medium.com/@vesko.karaganev/compile-time-reflection-in-c-17-55c14ee8106b
    - https://veselink1.github.io/blog/cpp/metaprogramming/2019/07/13/refl-cpp-deep-dive.html
*/

// Compile time utilities.
namespace Reflection::Detail
{
    template<typename... Types>
    struct ObjectList
    {
        using TupleType = std::tuple<Types...>;

        constexpr ObjectList(TupleType&& Objects) :
            Objects(Objects)
        {
        }

        template<std::size_t Index>
        constexpr const std::tuple_element_t<Index, TupleType>& Get() const
        {
            return std::get<Index>(Objects);
        }

        const TupleType Objects;
        static constexpr std::size_t Count = sizeof...(Types);
    };

    constexpr ObjectList<> MakeEmptyObjectList()
    {
        return ObjectList<>(std::make_tuple());
    }
}

// Reflection implementation detail.
namespace Reflection::Detail
{
    struct TypeInfoBase
    {
        static constexpr bool Reflected = false;
        static constexpr auto Name = "<UnknownType>";
        static constexpr auto Attributes = MakeEmptyObjectList();
        static constexpr auto Members = MakeEmptyObjectList();
    };

    template<typename Type>
    struct TypeInfo : public TypeInfoBase
    {
    };
}

// Reflection interface.
namespace Reflection
{
    template<typename OwnerType, typename AttributeType, std::size_t Index>
    struct AttributeDescription
    {
        using Type = AttributeType;
    };

    template<typename OwnerType, typename MemberType, std::size_t Index>
    struct MemberDescription
    {
        using Type = MemberType;

        static constexpr auto Name = Detail::TypeInfo<OwnerType>::Members.template Get<Index>().Name;
        static constexpr auto Pointer = Detail::TypeInfo<OwnerType>::Members.template Get<Index>().Pointer;
        static constexpr auto Attributes = Detail::TypeInfo<OwnerType>::Members.template Get<Index>().Attributes;
    };

    template<typename Type_>
    struct TypeDescription
    {
        using Type = Type_;

        static constexpr auto Reflected = Detail::TypeInfo<Type>::Reflected;
        static constexpr auto Name = Detail::TypeInfo<Type>::Name;
        static constexpr auto Attributes = Detail::TypeInfo<Type>::Attributes;
        static constexpr auto Members = Detail::TypeInfo<Type>::Members;

        template<std::size_t Index>
        constexpr auto Attribute() const -> AttributeDescription<Type, decltype(Attributes.template Get<Index>()), Index>
        {
            return {};
        }

        template<std::size_t Index>
        constexpr auto Member() const -> MemberDescription<Type, decltype(Members.template Get<Index>()), Index>
        {
            return {};
        }
    };

    template<typename Type>
    constexpr TypeDescription<Type> Reflect()
    {
        return {};
    }

    template<typename Type>
    constexpr TypeDescription<Type> Reflect(const Type&)
    {
        return {};
    }

    template<typename Type>
    constexpr bool IsReflected()
    {
        return Detail::TypeInfo<Type>::Reflected;
    }

    template<typename Type>
    constexpr bool IsReflected(const Type& type)
    {
        return Detail::TypeInfo<Type>::Reflected;
    }
}

// Attribute handling.
namespace Reflection
{
    struct TypeAttribute
    {
    };

    struct FieldAttribute
    {
    };

    struct MethodAttribute
    {
    };
}

namespace Reflection::Detail
{
    template<typename... Attributes>
    constexpr bool ValidateAttributeReflection()
    {
        return (... && IsReflected<Attributes>());
    }

    template<typename FirstAttribute, typename... Attributes>
    constexpr bool ValidateAttributeUniqueness()
    {
        return (... && (!std::is_same_v<FirstAttribute, Attributes> && ValidateAttributeUniqueness<Attributes...>()));
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
        static_assert(ValidateAttributeReflection<Attributes...>(), "Detected attribute that is not reflected!");
        static_assert(ValidateAttributeUniqueness<Attributes...>(), "Detected attribute that is not unique!");
        static_assert(ValidateAttributeUsage<Requirement, Attributes...>(), "Detected attribute with incorrect usage!");

        return ObjectList<Attributes...>(std::make_tuple(attributes...));
    }
}

// Member handling.
namespace Reflection::Detail
{
    template<typename Type, std::size_t MemberIndex>
    using MemberEntry = typename TypeInfo<Type>::template MemberInfo<MemberIndex>;

    template<typename Type, std::size_t... MemberIndices>
    constexpr ObjectList<MemberEntry<Type, MemberIndices>...> MakeMemberList(std::index_sequence<MemberIndices...>)
    {
        return ObjectList<MemberEntry<Type, MemberIndices>...>(std::make_tuple(MemberEntry<Type, MemberIndices>()...));
    }
}

// Macro helpers.
#define REFLECTION_EXPAND(x) x
#define REFLECTION_STRINGIFY(expression) #expression

// Type declaration macros.
#define REFLECTION_TYPE_INFO_BEGIN(Type_) \
    template<> struct Reflection::Detail::TypeInfo<Type_> : public TypeInfoBase \
    { \
    private: \
        static constexpr std::size_t MemberIndexOffset = __COUNTER__ + 1; \
    public: \
        using Type = Type_; \
        static constexpr bool Reflected = true; \
        static constexpr std::string_view Name = REFLECTION_STRINGIFY(Type_); \
        template<std::size_t, typename Dummy = void> struct MemberInfo;

#define REFLECTION_ATTRIBUTES(...) \
        static constexpr auto Attributes = MakeAttributeList<Reflection::TypeAttribute>(__VA_ARGS__);

#define REFLECTION_TYPE_INFO_END \
        static constexpr std::size_t MemberCount = __COUNTER__ - MemberIndexOffset; \
        static constexpr auto Members = MakeMemberList<Type>(std::make_index_sequence<MemberCount>()); \
    };

#define REFLECTION_TYPE_BASE_BEGIN(Type) \
    REFLECTION_TYPE_INFO_BEGIN(Type)

#define REFLECTION_TYPE_DERIVED_BEGIN(DerivedType, BaseType) \
    REFLECTION_TYPE_INFO_BEGIN(DerivedType)

#define REFLECTION_TYPE_BEGIN_DEDUCE(arg1, arg2, arg3, ...) arg3
#define REFLECTION_TYPE_BEGIN_CHOOSER(...) REFLECTION_EXPAND(REFLECTION_TYPE_BEGIN_DEDUCE(__VA_ARGS__, REFLECTION_TYPE_DERIVED_BEGIN, REFLECTION_TYPE_BASE_BEGIN))
#define REFLECTION_TYPE_BEGIN(...) REFLECTION_EXPAND(REFLECTION_TYPE_BEGIN_CHOOSER(__VA_ARGS__)(__VA_ARGS__))
#define REFLECTION_TYPE_END REFLECTION_TYPE_INFO_END

#define REFLECTION_TYPE_BASE(Type) \
    REFLECTION_TYPE_INFO_BEGIN(Type) \
    REFLECTION_TYPE_INFO_END

#define REFLECTION_TYPE_DERIVED(DerivedType, BaseType) \
    REFLECTION_TYPE_INFO_BEGIN(DerivedType) \
    REFLECTION_TYPE_INFO_END

#define REFLECTION_TYPE_DEDUCE(arg1, arg2, arg3, ...) arg3
#define REFLECTION_TYPE_CHOOSER(...) REFLECTION_EXPAND(REFLECTION_TYPE_DEDUCE(__VA_ARGS__, REFLECTION_TYPE_DERIVED, REFLECTION_TYPE_BASE))
#define REFLECTION_TYPE(...) REFLECTION_EXPAND(REFLECTION_TYPE_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

// Field declaration macros.
#define REFLECTION_FIELD_BEGIN(Field) \
    template<typename Dummy> struct MemberInfo<__COUNTER__ - MemberIndexOffset, Dummy> \
    { \
        static constexpr std::string_view Name = REFLECTION_STRINGIFY(Field); \
        static constexpr auto Pointer = &Type::Field; \
        using Type = decltype(Type::Field);

#define REFLECTION_FIELD_ATTRIBUTES(...) \
        static constexpr auto Attributes = Reflection::Detail::MakeAttributeList<Reflection::FieldAttribute>(__VA_ARGS__);

#define REFLECTION_FIELD_END \
    };

#define REFLECTION_FIELD(Field, ...) \
    REFLECTION_FIELD_BEGIN(Field) \
    REFLECTION_FIELD_ATTRIBUTES(__VA_ARGS__) \
    REFLECTION_FIELD_END
