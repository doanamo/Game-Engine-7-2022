/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <variant>
#include "Debug.hpp"

/*
    Result

    Wrapper inspired by Rust language for return values that can indicate
    either success or failure. See unit tests for example usage.
*/

namespace Common::Detail
{
    struct Empty
    {
    };

    template<typename Type>
    struct Success
    {
        Success(Type value) :
            value(std::forward<Type>(value))
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
        Failure(Type value) :
            value(std::forward<Type>(value))
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
    template<typename Type>
    constexpr Detail::Success<const Type&> Success(const Type& value)
    {
        return Detail::Success<const Type&>(value);
    }

    template<typename Type>
    constexpr Detail::Success<Type&&> Success(Type&& value)
    {
        return Detail::Success<Type&&>(std::forward<Type>(value));
    }

    constexpr Detail::Success<void> Success()
    {
        return Detail::Success<void>();
    }

    template<typename Type>
    constexpr Detail::Failure<const Type&> Failure(const Type& value)
    {
        return Detail::Failure<const Type&>(value);
    }

    template<typename Type>
    constexpr Detail::Failure<Type&&> Failure(Type&& value)
    {
        return Detail::Failure<Type&&>(std::forward<Type>(value));
    }

    constexpr Detail::Failure<void> Failure()
    {
        return Detail::Failure<void>();
    }

    template<typename SuccessType, typename FailureType>
    class Result
    {
    public:
        using DeductedSuccessType =
            typename std::conditional<std::is_same<SuccessType, void>::value, 
            Detail::Empty, SuccessType>::type;

        using DeductedFailureType =
            typename std::conditional<std::is_same<FailureType, void>::value,
            Detail::Empty, FailureType>::type;

        using DeductedSharedType =
            typename std::conditional<std::is_same<SuccessType, FailureType>::value,
            SuccessType, Detail::Empty>::type;

        using StorageType = std::variant<DeductedSuccessType, DeductedFailureType>;

        static constexpr size_t StorageSuccessIndex = 0;
        static constexpr size_t StorageFailureIndex = 1;

        template<typename Type>
        Result(Detail::Success<Type>&& success) :
            m_storage(std::in_place_index<StorageSuccessIndex>, std::move(success.value))
        {
        }

        template<typename Type>
        Result(Detail::Failure<Type>&& failure) :
            m_storage(std::in_place_index<StorageFailureIndex>, std::move(failure.value))
        {
        }

        Result(const Result&) = delete;
        Result& operator=(const Result&) = delete;
        
        Result(Result&& other)
        {
            *this = std::move(other);
        }

        Result& operator=(Result&& other)
        {
            if(this != &other)
            {
                m_storage = std::move(other.m_storage);
            }

            return *this;
        }

        DeductedSuccessType Unwrap()
        {
            return UnwrapSuccess();
        }

        DeductedSuccessType UnwrapSuccess()
        {
            ASSERT(IsSuccess(), "Invalid result unwrap!");
            return std::get<StorageSuccessIndex>(std::move(m_storage));
        }

        DeductedFailureType UnwrapFailure()
        {
            ASSERT(IsFailure(), "Invalid result unwrap!");
            return std::get<StorageFailureIndex>(std::move(m_storage));
        }

        template<typename DefaultReturn>
        DeductedSuccessType UnwrapOr(DefaultReturn defaultReturn)
        {
            return UnwrapSuccessOr(std::forward<DefaultReturn>(defaultReturn));
        }

        template<typename DefaultReturn>
        DeductedSuccessType UnwrapSuccessOr(DefaultReturn defaultReturn)
        {
            return IsSuccess() ? UnwrapSuccess() : std::forward<DefaultReturn>(defaultReturn);
        }

        template<typename DefaultReturn>
        DeductedFailureType UnwrapFailureOr(DefaultReturn defaultReturn)
        {
            return IsFailure() ? UnwrapFailure() : std::forward<DefaultReturn>(defaultReturn);
        }

        DeductedSharedType UnwrapEither()
        {
            static_assert(std::is_same<SuccessType, FailureType>::value,
                "Both success and failure types must be the same to use UnwrapEither()!");
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
            return std::get_if<StorageSuccessIndex>(&m_storage) != nullptr;
        }

        bool IsFailure() const
        {
            return std::get_if<StorageFailureIndex>(&m_storage) != nullptr;
        }

        operator bool() const
        {
            return IsSuccess();
        }

    private:
        StorageType m_storage;
    };

    using GenericResult = Result<void, void>;
}

/*
    Macro Helpers
*/

#define SUCCESS_OR_RETURN_RESULT(expression) \
    { \
        auto result = (expression); \
        if(!result) \
        { \
            return result; \
        } \
    }
