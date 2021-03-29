/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <tuple>
#include <string_view>
#include <Common/Utility.hpp>

/*
    Reflection Detail
*/

#define REFLECTION_EXPAND(x) x
#define REFLECTION_STRINGIFY(expression) #expression

namespace Reflection::Detail
{
    constexpr std::string_view ParseFieldName(std::string_view name)
    {
        for(std::size_t i = 0; i < std::min(name.size(), std::size_t(2)); ++i)
        {
            if(name[i] == '_')
            {
                return name.substr(i + 1);
            }
        }

        return name;
    }

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
            static_assert(Count > Index, "Out of bounds object index!");
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
        static_assert(std::tuple_size<TupleType>::value > 0,
            "Cannot pop element from an empty tuple!");
        return PopFrontTuple(tuple, std::make_index_sequence<
            std::tuple_size<TupleType>::value - 1>());
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
    constexpr auto Filter(Function function, ObjectList<Type, Types...> list,
        ObjectList<Results...> results)
    {
        const auto element = list.template Get<0>();

        if constexpr(function(Type{}))
        {
            return Filter(function, ObjectList<Types...>(PopFrontTuple(list.Objects)),
                ObjectList<Type, Results...>(PushFrontTuple(results.Objects, element)));
        }
        else
        {
            return Filter(function, ObjectList<Types...>(PopFrontTuple(list.Objects)),
                ObjectList<Results...>(results.Objects));
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
            return FindFirstIndex<Index + 1>(function,
                ObjectList<Types...>(PopFrontTuple(list.Objects)));
        }
    }
}

namespace Reflection
{
    template<typename Function, typename... Types, std::size_t... Indices>
    constexpr void ForEachSequence(const Detail::ObjectList<Types...>& list, Function&& function,
        std::index_sequence<Indices...>)
    {
        // Explanation of this for each tuple element implementation:
        // - https://codereview.stackexchange.com/questions/51407/stdtuple-foreach-implementation
        // - https://stackoverflow.com/questions/26902633/how-to-iterate-over-a-stdtuple-in-c-11
        using Ordered = int[];
        (void)Ordered {
            1, (function(std::get<Indices>(list.Objects)), void(), int()) ...
        };
    }

    template<typename Function, typename... Types>
    constexpr void ForEach(const Detail::ObjectList<Types...>& list, Function&& function)
    {
        ForEachSequence(list, std::forward<Function>(function),
            std::make_index_sequence<Detail::ObjectList<Types...>::Count>());
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

namespace Reflection
{
    using TypeIdentifier = uint32_t;
    constexpr TypeIdentifier InvalidIdentifier = 0;

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
        static constexpr TypeIdentifier Identifier = InvalidIdentifier;
        static constexpr TypeIdentifier BaseTypeIdentifier = InvalidIdentifier;
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

        virtual const DynamicTypeInfo& LookupType(TypeIdentifier identifier) const = 0;
    };

    const ReflectionRegistry& GetRegistry();
}
