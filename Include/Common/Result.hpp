/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <tuple>
#include "Debug.hpp"

/*
    Result

    Wrapper inspired by Rust language for return values that can indicate either success or failure.
*/

namespace Common::Detail
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

namespace Common
{
    template<typename Type, typename DecayedType = typename std::conditional<std::is_same<Type, Detail::Empty>::value, void, typename std::decay<Type>::type>::type>
    constexpr Detail::Success<DecayedType> Success(Type&& value)
    {
        return Detail::Success<DecayedType>(std::forward<Type>(value));
    }

    template<>
    constexpr Detail::Success<void> Success<Detail::Empty, void>(Detail::Empty&& value)
    {
        return Detail::Success<void>();
    }

    constexpr Detail::Success<void> Success()
    {
        return Detail::Success<void>();
    }

    template<typename Type, typename DecayedType = typename std::conditional<std::is_same<Type, Detail::Empty>::value, void, typename std::decay<Type>::type>::type>
    constexpr Detail::Failure<DecayedType> Failure(Type&& value)
    {
        return Detail::Failure<DecayedType>(std::forward<Type>(value));
    } 

    template<>
    constexpr Detail::Failure<void> Failure<Detail::Empty, void>(Detail::Empty&& value)
    {
        return Detail::Failure<void>();
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
        using DeductedSharedType = typename std::conditional<std::is_same<SuccessType, FailureType>::value, SuccessType, Detail::Empty>::type;
        using StorageType = typename std::tuple<bool, DeductedSuccessType, DeductedFailureType>;

        Result(Detail::Success<SuccessType>&& success) :
            m_storage(true, std::move(success.value), DeductedFailureType())
        {
        }

        Result(Detail::Failure<FailureType>&& failure) :
            m_storage(false, DeductedSuccessType(), std::move(failure.value))
        {
        }

        Result(const Result&) = delete;
        Result& operator=(const Result&) = delete;
        
        Result(Result&& other)
        {
            this->operator=(std::move(other));
        }

        Result& operator=(Result&& other)
        {
            m_storage = std::move(other.m_storage);
            return *this;
        }

        DeductedSuccessType Unwrap()
        {
            return UnwrapSuccess();
        }

        DeductedSuccessType UnwrapSuccess()
        {
            ASSERT(IsSuccess(), "Invalid result unwrap!");
            return std::move(std::get<1>(m_storage));
        }

        DeductedFailureType UnwrapFailure()
        {
            ASSERT(IsFailure(), "Invalid result unwrap!");
            return std::move(std::get<2>(m_storage));
        }

        DeductedSuccessType UnwrapOr(DeductedSuccessType&& defaultReturn)
        {
            return UnwrapSuccessOr(std::move(defaultReturn));
        }

        DeductedSuccessType UnwrapSuccessOr(DeductedSuccessType&& defaultReturn)
        {
            return IsSuccess() ? UnwrapSuccess() : std::move(defaultReturn);
        }

        DeductedFailureType UnwrapFailureOr(DeductedFailureType&& defaultReturn)
        {
            return IsFailure() ? UnwrapFailure() : std::move(defaultReturn);
        }

        DeductedSharedType UnwrapEither()
        {
            static_assert(std::is_same<SuccessType, FailureType>::value, "Both success and failure types must be the same to use UnwrapEither()!");
            return IsSuccess() ? UnwrapSuccess() : UnwrapFailure();
        }

        Result<FailureType, SuccessType> AsFailure()
        {
            if(IsFailure())
            {
                return Common::Success(UnwrapFailure());
            }
            else
            {
                return Common::Failure(UnwrapSuccess());
            }
        }

        bool IsSuccess() const
        {
            return std::get<0>(m_storage);
        }

        bool IsFailure() const
        {
            return !std::get<0>(m_storage);
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
        StorageType m_storage;
    };

    using GenericResult = Result<void, void>;
}

// Macro helpers.
#define SUCCESS_OR_RETURN_RESULT(expression) \
    { \
        auto result = (expression); \
        if(!result) \
        { \
            return result; \
        } \
    }
