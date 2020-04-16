/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <variant>
#include "Debug.hpp"

/*
    Result

    Based on implementation from:
    - https://github.com/oktal/result
*/

namespace Detail
{
    struct Empty
    {
    };

    template<typename Type>
    struct Success
    {
        constexpr Success(const Type& value) :
            value(value)
        {
        }

        constexpr Success(Type&& value) :
            value(std::move(value))
        {
        }

        const Type value;
    };

    template<>
    struct Success<void>
    {
        const Empty value;
    };

    template<typename Type>
    struct Failure
    {
        constexpr Failure(const Type& value) :
            value(value)
        {
        }

        constexpr Failure(Type&& value) :
            value(std::move(value))
        {
        }

        const Type value;
    };

    template<>
    struct Failure<void>
    {
        const Empty value;
    };
}

template<typename Type, typename DecayedType = typename std::decay<Type>::type>
constexpr Detail::Success<DecayedType> Success(Type&& value)
{
    return Detail::Success<DecayedType>(std::forward<Type>(value));
}

constexpr Detail::Success<void> Success()
{
    return Detail::Success<void>();
}

template<typename Type, typename DecayedType = typename std::decay<Type>::type>
constexpr Detail::Failure<DecayedType> Failure(Type&& value)
{
    return Detail::Failure<DecayedType>(std::forward<Type>(value));
}

constexpr Detail::Failure<void> Failure()
{
    return Detail::Failure<void>();
}

template<typename SuccessType, typename FailureType>
class Result
{
public:
    using DeductedSuccessType = typename std::conditional<std::is_same<SuccessType, void>::value, Detail::Empty, SuccessType>::type;
    using DeductedFailureType = typename std::conditional<std::is_same<FailureType, void>::value, Detail::Empty, FailureType>::type;

    using ResultType = Result<DeductedSuccessType, DeductedFailureType>;
    using VariantType = std::variant<DeductedSuccessType, DeductedFailureType>;

    constexpr Result(Detail::Success<SuccessType>&& success) :
        m_variant(CreateSuccessVariant(std::forward<Detail::Success<SuccessType>>(success)))
    {
    }

    constexpr Result(Detail::Failure<FailureType>&& failure) :
        m_variant(CreateFailureVariant(std::forward<Detail::Failure<FailureType>>(failure)))
    {
    }

    constexpr DeductedSuccessType Unwrap() const
    {
        return UnwrapSuccess();
    }

    constexpr DeductedSuccessType UnwrapSuccess() const
    {
        return std::get<0>(m_variant);
    }

    constexpr DeductedFailureType UnwrapFailure() const
    {
        return std::get<1>(m_variant);
    }

    constexpr DeductedSuccessType UnwrapOr(DeductedSuccessType default) const
    {
        return UnwrapSuccessOr(default);
    }

    constexpr DeductedSuccessType UnwrapSuccessOr(DeductedSuccessType default) const
    {
        if(IsSuccess())
        {
            return std::get<0>(m_variant);
        }
        else
        {
            return default;
        }
    }

    constexpr DeductedFailureType UnwrapFailureOr(DeductedFailureType default) const
    {
        if(IsFailure())
        {
            return std::get<1>(m_variant);
        }
        else
        {
            return default;
        }
    }

    constexpr bool IsSuccess() const
    {
        return std::get_if<0>(&m_variant) != nullptr;
    }

    constexpr bool IsFailure() const
    {
        return std::get_if<1>(&m_variant) != nullptr;
    }

    constexpr bool operator==(const bool boolean) const
    {
        return IsSuccess() == boolean;
    }

    constexpr bool operator!=(const bool boolean) const
    {
        return IsSuccess() != boolean;
    }

    constexpr explicit operator bool() const
    {
        return IsSuccess();
    }

private:
    static constexpr VariantType CreateSuccessVariant(Detail::Success<SuccessType>&& success)
    {
        VariantType variant;
        variant.emplace<0>(success.value);
        return variant;
    }

    static constexpr VariantType CreateFailureVariant(Detail::Failure<FailureType>&& failure)
    {
        VariantType variant;
        variant.emplace<1>(failure.value);
        return variant;
    }

    const VariantType m_variant;
};

using GenericResult = typename Result<void, void>;

// Macro helpers.
#define SUCCESS_OR_RETURN_RESULT(expression) \
    { \
        auto result = (expression); \
        if(!result) \
        { \
            return result; \
        } \
    }
