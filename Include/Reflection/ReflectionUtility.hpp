/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <tuple>
#include <string_view>

/*
    Reflection Utility
*/

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
