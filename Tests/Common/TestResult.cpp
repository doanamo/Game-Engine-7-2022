/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <doctest/doctest.h>
#include <Common/Result.hpp>

class ResultWithVoid
{
public:
    using InitializeResult = Common::Result<void, void>;

    InitializeResult Initialize(bool success)
    {
        if(!success)
        {
            return Common::Failure();
        }

        return Common::Success();
    }
};

class ResultWithEnum
{
public:
    enum SuccessResult
    {
        VeryGood,
        JustGood,
    };

    using FailureResult = int;
    using InitializeResult = Common::Result<SuccessResult, FailureResult>;

    InitializeResult Initialize(bool success)
    {
        if(!success)
        {
            return Common::Failure(42);
        }

        return Common::Success(VeryGood);
    }
};

class ResultWithString
{
public:
    using InitializeResult = Common::Result<std::string, std::string>;

    InitializeResult Initialize(bool success)
    {
        if(!success)
        {
            std::string text = "goodbye world!";
            return Common::Failure(text);
        }

        return Common::Success(std::string("hello world!"));
    }
};

class ResultUnwrap
{
public:
    using InitializeResult = Common::Result<std::unique_ptr<ResultUnwrap>, void>;

    static InitializeResult Create(std::string text)
    {
        if(text == "Goodbye world!")
            return Common::Failure();

        auto instance = std::make_unique<ResultUnwrap>();
        instance->text = text;

        return Common::Success(std::move(instance));
    }

    std::string text;
};

TEST_CASE("Result")
{
    SUBCASE("With void type")
    {
        ResultWithVoid instance;
        CHECK(instance.Initialize(true));
        CHECK_FALSE(instance.Initialize(false));
    }

    SUBCASE("With enum type")
    {
        ResultWithEnum instance;
        CHECK(instance.Initialize(true));
        CHECK_FALSE(instance.Initialize(false));

        CHECK_EQ(instance.Initialize(true)
            .UnwrapSuccessOr(ResultWithEnum::JustGood), ResultWithEnum::VeryGood);
        CHECK_EQ(instance.Initialize(false)
            .UnwrapSuccessOr(ResultWithEnum::JustGood), ResultWithEnum::JustGood);

        CHECK_EQ(instance.Initialize(false).UnwrapFailureOr(69), 42);
        CHECK_EQ(instance.Initialize(true).UnwrapFailureOr(69), 69);
    }

    SUBCASE("With string type")
    {
        ResultWithString instance;
        CHECK_EQ(instance.Initialize(true).UnwrapSuccessOr("goodbye world!"), "hello world!");
        CHECK_EQ(instance.Initialize(false).UnwrapSuccessOr("goodbye world!"), "goodbye world!");
        CHECK_EQ(instance.Initialize(false).UnwrapFailureOr("hello world!"), "goodbye world!");
        CHECK_EQ(instance.Initialize(true).UnwrapSuccessOr("hello world!"), "hello world!");
    }

    SUBCASE("Unwrap")
    {
        auto result = ResultUnwrap::Create("Hello world!");
        auto instance = result.Unwrap();

        CHECK_EQ(instance->text, "Hello world!");
        CHECK(result.IsSuccess());
        CHECK_FALSE(result.IsFailure());
    }

    SUBCASE("Unwrap or")
    {
        auto result = ResultUnwrap::Create("Goodbye world!");
        auto instance = result.UnwrapOr(nullptr);

        CHECK_EQ(instance, nullptr);
        CHECK_FALSE(result.IsSuccess());
        CHECK(result.IsFailure());
    }

    SUBCASE("Unwrap either")
    {
        ResultWithString instance;

        auto resultSuccess = instance.Initialize(true).UnwrapEither();
        CHECK_EQ(resultSuccess, "hello world!");

        auto resultFailure = instance.Initialize(false).UnwrapEither();
        CHECK_EQ(resultFailure, "goodbye world!");
    }

    SUBCASE("As failure")
    {
        ResultWithString instance;

        auto resultFailure = instance.Initialize(false).AsFailure();
        CHECK(resultFailure.IsSuccess());
        CHECK_EQ(resultFailure.Unwrap(), "goodbye world!");
    }
}
