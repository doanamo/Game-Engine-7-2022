/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Reflection

    Implementation is based on refl-cpp library:
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
        const TupleType Objects;

        constexpr ObjectList(const TupleType& Objects) :
            Objects(Objects)
        {
        }

        constexpr ObjectList(TupleType&& Objects) :
            Objects(Objects)
        {
        }

        template<std::size_t Index>
        constexpr std::tuple_element_t<Index, TupleType> Get() const
        {
            return std::get<Index>(Objects);
        }

        static constexpr std::size_t Count = sizeof...(Types);
    };

    constexpr ObjectList<> MakeEmptyObjectList()
    {
        return { std::tuple<>() };
    }

    template<typename TupleType, std::size_t... Indices>
    constexpr auto PopFrontTuple(TupleType tuple, std::index_sequence<Indices...>)
    {
        return std::make_tuple(std::get<1 + Indices>(tuple)...);
    }

    template<typename TupleType>
    constexpr auto PopFrontTuple(TupleType tuple)
    {
        static_assert(std::tuple_size<TupleType>::value > 0, "Cannot pop element from an empty tuple!");
        return PopFrontTuple(tuple, std::make_index_sequence<std::tuple_size<TupleType>::value - 1>());
    }

    template<typename TupleType, typename Element>
    constexpr auto PushFrontTuple(TupleType tuple, Element element)
    {
        return std::tuple_cat(std::make_tuple(element), tuple);
    }

    template<typename Function, typename... Results>
    constexpr auto Filter(Function function, ObjectList<> list, ObjectList<Results...> results)
    {
        return results;
    }

    template<typename Function, typename Type, typename... Types, typename... Results>
    constexpr auto Filter(Function function, ObjectList<Type, Types...> list, ObjectList<Results...> results)
    {
        const auto element = list.template Get<0>();

        if constexpr(function(Type{}))
        {
            return Filter(function, ObjectList<Types...>(PopFrontTuple(list.Objects)), ObjectList<Type, Results...>(PushFrontTuple(results.Objects, element)));
        }
        else
        {
            return Filter(function, ObjectList<Types...>(PopFrontTuple(list.Objects)), ObjectList<Results...>(results.Objects));
        }
    }

    template<bool FoundResult, std::size_t FoundIndex>
    struct FindIndexResult
    {
        static constexpr auto Found = FoundResult;
        static constexpr auto Index = FoundIndex;
    };

    template<std::size_t Index = 0, typename Function>
    constexpr auto FindFirstIndex(Function function, ObjectList<> list)
    {
        return FindIndexResult<false, Index>{};
    }

    template<std::size_t Index = 0, typename Function, typename Type, typename...Types>
    constexpr auto FindFirstIndex(Function function, ObjectList<Type, Types...> list)
    {
        if constexpr(function(Type{}))
        {
            return FindIndexResult<true, Index>{};
        }
        else
        {
            return FindFirstIndex<Index + 1>(function, ObjectList<Types...>(PopFrontTuple(list.Objects)));
        }
    }
}

namespace Reflection
{
    template<typename Function, typename... Types, std::size_t... Indices>
    constexpr void ForEachSequence(const Detail::ObjectList<Types...>& list, Function&& function, std::index_sequence<Indices...>)
    {
        // Explanation for this for each tuple element implementation:
        // - https://codereview.stackexchange.com/questions/51407/stdtuple-foreach-implementation
        // - https://stackoverflow.com/questions/26902633/how-to-iterate-over-a-stdtuple-in-c-11
        int Ordered[] = { 1, (function(std::get<Indices>(list.Objects)), void(), int()) ... };
    }

    template<typename Function, typename... Types>
    constexpr void ForEach(const Detail::ObjectList<Types...>& list, Function&& function)
    {
        ForEachSequence(list, std::forward<Function>(function), std::make_index_sequence<Detail::ObjectList<Types...>::Count>());
    }

    template<typename Function, typename... Types>
    constexpr auto Filter(Detail::ObjectList<Types...> list, Function function)
    {
        return Detail::Filter(function, list, Detail::MakeEmptyObjectList());
    }

    template<typename Function, typename... Types>
    constexpr auto FindFirst(Detail::ObjectList<Types...> list, Function function)
    {
        const auto results = Filter<Function, Types...>(list, function);
        static_assert(results.Count != 0, "Could not find any results!");
        return results.template Get<0>();
    }

    template<typename Function, typename... Types>
    constexpr std::size_t FindFirstIndex(Detail::ObjectList<Types...> list, Function function)
    {
        const auto result = Detail::FindFirstIndex(function, list);
        static_assert(result.Found, "Could not find desired index!");
        return result.Index;
    }

    template<typename Function, typename... Types>
    constexpr auto FindOne(Detail::ObjectList<Types...> list, Function function)
    {
        const auto results = Filter<Function, Types...>(list, function);
        static_assert(results.Count != 0, "Could not find any results!");
        static_assert(results.Count == 1, "Found more than one result!");
        return results.template Get<0>();
    }
}

// Reflection implementation detail.
namespace Reflection
{
    struct NullType;

    template<typename ReflectedType, typename AttributeType, std::size_t AttributeIndex>
    struct AttributeDescription;

    template<typename ReflectedType, typename AttributeType, std::size_t AttributeIndex>
    struct AttributeDescriptionWithInstance;

    template<typename ReflectedType, typename MemberType, std::size_t MemberIndex>
    struct MemberDescription;
}

namespace Reflection::Detail
{
    struct TypeInfoBase
    {
        using Type = NullType;
        using BaseType = NullType;

        static constexpr bool Reflected = false;
        static constexpr auto Name = "<UnknownType>";
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
    constexpr ObjectList<AttributeDescriptionWithInstance<ReflectedType, AttributeTypes, AttributeIndices>...> MakeAttributeDescriptionWithInstanceList(const ObjectList<AttributeTypes...>& attributes, std::index_sequence<AttributeIndices...>)
    {
        return { std::make_tuple(AttributeDescriptionWithInstance<ReflectedType, AttributeTypes, AttributeIndices>{ attributes.template Get<AttributeIndices>() } ...) };
    }

    template<typename ReflectedType, typename... AttributeTypes, std::size_t... AttributeIndices>
    constexpr ObjectList<AttributeDescription<ReflectedType, AttributeTypes, AttributeIndices>...> MakeAttributeDescriptionWithoutInstanceList(const ObjectList<AttributeTypes...>& attributes, std::index_sequence<AttributeIndices...>)
    {
        return { std::make_tuple(AttributeDescription<ReflectedType, AttributeTypes, AttributeIndices>{} ...) };
    }

    template<typename ReflectedType, std::size_t MemberIndex>
    using MemberDescriptionType = MemberDescription<ReflectedType, typename TypeInfo<ReflectedType>::template MemberInfo<MemberIndex, ReflectedType, void>::Type, MemberIndex>;

    template<typename ReflectedType, std::size_t... MemberIndices>
    using MemberDescriptionList = ObjectList<MemberDescriptionType<ReflectedType, MemberIndices>...>;

    template<typename ReflectedType, std::size_t... MemberIndices>
    constexpr MemberDescriptionList<ReflectedType, MemberIndices...> MakeMemberDescriptionList(std::index_sequence<MemberIndices...>)
    {
        return { std::make_tuple(MemberDescriptionType<ReflectedType, MemberIndices>{} ...) };
    }
}

// Reflection interface.
namespace Reflection
{
    struct NullType
    {
    };

    template<typename ReflectedType, typename AttributeType, std::size_t AttributeIndex>
    struct AttributeDescription
    {
        using Type = std::decay_t<AttributeType>;
        static constexpr auto TypeInfo = Detail::TypeInfo<AttributeType>{};
        static constexpr auto Index = AttributeIndex;
        static constexpr auto Name = TypeInfo.Name;

        template<typename OtherType>
        constexpr bool IsType() const
        {
            return std::is_same<Type, OtherType>::value;
        }
    };

    template<typename ReflectedType, typename AttributeType, std::size_t AttributeIndex>
    struct AttributeDescriptionWithInstance : public AttributeDescription<ReflectedType, AttributeType, AttributeIndex>
    {
        using TypeWithoutInstance = AttributeDescription<ReflectedType, AttributeType, AttributeIndex>;

        constexpr AttributeDescriptionWithInstance(AttributeType&& Instance) :
            Instance(Instance)
        {
        }

        const AttributeType Instance;
    };

    template<typename ReflectedType, typename MemberType, std::size_t MemberIndex>
    struct MemberDescription
    {
        using Type = MemberType;
        static constexpr auto TypeInfo = Detail::TypeInfo<ReflectedType>::Members.template Get<MemberIndex>();
        static constexpr auto Index = MemberIndex;
        static constexpr auto Name = TypeInfo.Name;
        static constexpr auto Pointer = TypeInfo.Pointer;
        static constexpr auto Attributes = Detail::MakeAttributeDescriptionWithInstanceList<ReflectedType>(TypeInfo.Attributes, std::make_index_sequence<TypeInfo.Attributes.Count>());
        static constexpr auto AttributeTypes = Detail::MakeAttributeDescriptionWithoutInstanceList<ReflectedType>(TypeInfo.Attributes, std::make_index_sequence<TypeInfo.Attributes.Count>());

        template<typename OtherType>
        constexpr bool IsType() const
        {
            return std::is_same<Type, OtherType>::value;
        }

        constexpr bool HasAttributes() const
        {
            return Attributes.Count > 0;
        }

        template<std::size_t AttributeIndex>
        constexpr auto Attribute() const -> decltype(Attributes.template Get<AttributeIndex>())
        {
            return Attributes.template Get<AttributeIndex>();
        }

        template<auto& AttributeName>
        constexpr auto FindAttribute() const
        {
            constexpr auto Index = FindFirstIndex(AttributeTypes, [](auto Attribute) -> bool { return Attribute.Name == AttributeName; });
            return Attributes.template Get<Index>();
        }
    };

    template<typename ReflectedType>
    struct TypeDescription
    {
        using Type = ReflectedType;
        static constexpr auto TypeInfo = Detail::TypeInfo<Type>{};

        using BaseType = typename decltype(TypeInfo)::BaseType;
        static constexpr auto BaseTypeInfo = Detail::TypeInfo<BaseType>{};

        static constexpr auto Reflected = TypeInfo.Reflected;
        static constexpr auto Name = TypeInfo.Name;
        static constexpr auto Attributes = Detail::MakeAttributeDescriptionWithInstanceList<Type>(TypeInfo.Attributes, std::make_index_sequence<TypeInfo.Attributes.Count>());
        static constexpr auto AttributeTypes = Detail::MakeAttributeDescriptionWithoutInstanceList<Type>(TypeInfo.Attributes, std::make_index_sequence<TypeInfo.Attributes.Count>());
        static constexpr auto Members = Detail::MakeMemberDescriptionList<Type>(std::make_index_sequence<TypeInfo.Members.Count>());

        constexpr bool IsNullType() const
        {
            return std::is_same<Type, NullType>::value;
        }

        template<typename OtherType>
        constexpr bool IsType() const
        {
            return std::is_same<Type, OtherType>::value;
        }

        constexpr bool HasBaseType() const
        {
            return !std::is_same<BaseType, NullType>::value;
        }

        constexpr TypeDescription<BaseType> GetBaseType() const
        {
            return {};
        }

        constexpr bool HasAttributes() const
        {
            return Attributes.Count > 0;
        }

        template<std::size_t AttributeIndex>
        constexpr auto Attribute() const -> decltype(Attributes.template Get<AttributeIndex>())
        {
            return Attributes.template Get<AttributeIndex>();
        }

        template<auto& AttributeName>
        constexpr auto FindAttribute() const
        {
            constexpr auto Index = FindFirstIndex(AttributeTypes, [](auto Attribute) -> bool { return Attribute.Name == AttributeName; });
            return Attributes.template Get<Index>();
        }

        constexpr bool HasMembers() const
        {
            return Members.Count > 0;
        }

        template<std::size_t MemberIndex>
        constexpr auto Member() const -> decltype(Members.template Get<MemberIndex>())
        {
            return Members.template Get<MemberIndex>();
        }

        template<auto& MemberName>
        constexpr auto FindMember() const
        {
            return FindOne(Members, [](auto Member) -> bool { return Member.Name == MemberName; });
        }
    };

    template<typename ReflectedType>
    constexpr TypeDescription<ReflectedType> Reflect()
    {
        return {};
    }

    template<typename ReflectedType>
    constexpr TypeDescription<ReflectedType> Reflect(const ReflectedType& type)
    {
        return {};
    }

    template<typename ReflectedType>
    constexpr bool IsReflected()
    {
        return Detail::TypeInfo<ReflectedType>::Reflected;
    }

    template<typename ReflectedType>
    constexpr bool IsReflected(const ReflectedType& type)
    {
        return Detail::TypeInfo<ReflectedType>::Reflected;
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

        return { std::make_tuple(attributes...) };
    }
}

// Member handling.
namespace Reflection::Detail
{
    template<typename ReflectedType, std::size_t MemberIndex>
    using MemberEntry = typename TypeInfo<ReflectedType>::template MemberInfo<MemberIndex>;

    template<typename ReflectedType, std::size_t... MemberIndices>
    constexpr ObjectList<MemberEntry<ReflectedType, MemberIndices>...> MakeMemberList(std::index_sequence<MemberIndices...>)
    {
        return { std::make_tuple(MemberEntry<ReflectedType, MemberIndices>{} ...) };
    }
}

// Macro helpers.
#define REFLECTION_EXPAND(x) x
#define REFLECTION_STRINGIFY(expression) #expression

// Type declaration macros.
#define REFLECTION_TYPE_INFO_BEGIN(ReflectedType, ReflectedBaseType) \
    template<> struct Reflection::Detail::TypeInfo<ReflectedType> : public TypeInfoBase \
    { \
    private: \
        static constexpr std::size_t MemberIndexOffset = __COUNTER__ + 1; \
    public: \
        using Type = ReflectedType; \
        using BaseType = ReflectedBaseType; \
        static constexpr bool Reflected = true; \
        static constexpr std::string_view Name = REFLECTION_STRINGIFY(ReflectedType); \
        template<std::size_t Index, typename Type = ReflectedType, typename Dummy = void> struct MemberInfo;

#define REFLECTION_ATTRIBUTES(...) \
        static constexpr auto Attributes = MakeAttributeList<Reflection::TypeAttribute>(__VA_ARGS__);

#define REFLECTION_TYPE_INFO_END \
        static constexpr std::size_t MemberCount = __COUNTER__ - MemberIndexOffset; \
        static constexpr auto Members = MakeMemberList<Type>(std::make_index_sequence<MemberCount>()); \
    };

#define REFLECTION_TYPE_BASE_BEGIN(ReflectedType) \
    REFLECTION_TYPE_INFO_BEGIN(ReflectedType, NullType)

#define REFLECTION_TYPE_DERIVED_BEGIN(ReflectedType, ReflectedBaseType) \
    REFLECTION_TYPE_INFO_BEGIN(ReflectedType, ReflectedBaseType)

#define REFLECTION_TYPE_BEGIN_DEDUCE(arg1, arg2, arg3, ...) arg3
#define REFLECTION_TYPE_BEGIN_CHOOSER(...) REFLECTION_EXPAND(REFLECTION_TYPE_BEGIN_DEDUCE(__VA_ARGS__, REFLECTION_TYPE_DERIVED_BEGIN, REFLECTION_TYPE_BASE_BEGIN))
#define REFLECTION_TYPE_BEGIN(...) REFLECTION_EXPAND(REFLECTION_TYPE_BEGIN_CHOOSER(__VA_ARGS__)(__VA_ARGS__))
#define REFLECTION_TYPE_END REFLECTION_TYPE_INFO_END

#define REFLECTION_TYPE_BASE(ReflectedType) \
    REFLECTION_TYPE_INFO_BEGIN(ReflectedType, NullType) \
    REFLECTION_TYPE_INFO_END

#define REFLECTION_TYPE_DERIVED(ReflectedType, ReflectedBaseType) \
    REFLECTION_TYPE_INFO_BEGIN(ReflectedType, ReflectedBaseType) \
    REFLECTION_TYPE_INFO_END

#define REFLECTION_TYPE_DEDUCE(arg1, arg2, arg3, ...) arg3
#define REFLECTION_TYPE_CHOOSER(...) REFLECTION_EXPAND(REFLECTION_TYPE_DEDUCE(__VA_ARGS__, REFLECTION_TYPE_DERIVED, REFLECTION_TYPE_BASE))
#define REFLECTION_TYPE(...) REFLECTION_EXPAND(REFLECTION_TYPE_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

// Field declaration macros.
#define REFLECTION_FIELD_BEGIN(Field) \
    template<typename ReflectedType, typename Dummy> struct MemberInfo<__COUNTER__ - MemberIndexOffset, ReflectedType, Dummy> \
    { \
        using Type = decltype(ReflectedType::Field); \
        static constexpr std::string_view Name = REFLECTION_STRINGIFY(Field); \
        static constexpr auto Pointer = &ReflectedType::Field;

#define REFLECTION_FIELD_ATTRIBUTES(...) \
        static constexpr auto Attributes = Reflection::Detail::MakeAttributeList<Reflection::FieldAttribute>(__VA_ARGS__);

#define REFLECTION_FIELD_END \
    };

#define REFLECTION_FIELD(Field, ...) \
    REFLECTION_FIELD_BEGIN(Field) \
    REFLECTION_FIELD_ATTRIBUTES(__VA_ARGS__) \
    REFLECTION_FIELD_END

// Built-in type declarations.
REFLECTION_TYPE(Reflection::NullType)
