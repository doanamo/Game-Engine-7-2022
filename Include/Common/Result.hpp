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
        Success(const Type& value) :
            value(value)
        {
        }

        Success(Type&& value) :
            value(std::move(value))
        {
        }

        Type value;
    };

    template<>
    struct Success<void>
    {
        Empty value;
    };

    template<typename Type>
    struct Failure
    {
        Failure(const Type& value) :
            value(value)
        {
        }

        Failure(Type&& value) :
            value(std::move(value))
        {
        }

        Type value;
    };

    template<>
    struct Failure<void>
    {
        Empty value;
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

    Result(Detail::Success<SuccessType>&& success) :
        m_variant(CreateSuccessVariant(std::forward<Detail::Success<SuccessType>>(success)))
    {
    }

    Result(Detail::Failure<FailureType>&& failure) :
        m_variant(CreateFailureVariant(std::forward<Detail::Failure<FailureType>>(failure)))
    {
    }

    DeductedSuccessType Unwrap()
    {
        return UnwrapSuccess();
    }

    DeductedSuccessType UnwrapSuccess()
    {
        ASSERT(IsSuccess(), "Invalid result unwrap!");
        return std::move(std::get<0>(m_variant));
    }

    DeductedFailureType UnwrapFailure()
    {
        ASSERT(IsFailure(), "Invalid result unwrap!");
        return std::move(std::get<1>(m_variant));
    }

    DeductedSuccessType UnwrapOr(DeductedSuccessType default)
    {
        return UnwrapSuccessOr(std::move(default));
    }

    DeductedSuccessType UnwrapSuccessOr(DeductedSuccessType default)
    {
        if(IsSuccess())
        {
            return UnwrapSuccess();
        }
        else
        {
            return std::move(default);
        }
    }

    DeductedFailureType UnwrapFailureOr(DeductedFailureType default)
    {
        if(IsFailure())
        {
            return UnwrapFailure();
        }
        else
        {
            return std::move(default);
        }
    }

    bool IsSuccess() const
    {
        return std::get_if<0>(&m_variant) != nullptr;
    }

    bool IsFailure() const
    {
        return std::get_if<1>(&m_variant) != nullptr;
    }

    bool operator==(const bool boolean) const
    {
        return IsSuccess() == boolean;
    }

    bool operator!=(const bool boolean) const
    {
        return IsSuccess() != boolean;
    }

    explicit operator bool() const
    {
        return IsSuccess();
    }

private:
    static constexpr VariantType CreateSuccessVariant(Detail::Success<SuccessType>&& success)
    {
        VariantType variant;
        variant.emplace<0>(std::move(success.value));
        return variant;
    }

    static constexpr VariantType CreateFailureVariant(Detail::Failure<FailureType>&& failure)
    {
        VariantType variant;
        variant.emplace<1>(std::move(failure.value));
        return variant;
    }

    VariantType m_variant;
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
