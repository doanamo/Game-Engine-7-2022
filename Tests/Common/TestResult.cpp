/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#define DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
#include <doctest/doctest.h>

#include <memory>
#include <string>
#include <Common/Test/InstanceCounter.hpp>
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

class ResultWithEnumClass
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

        SuccessResult result = SuccessResult::VeryGood;
        return Common::Success(result);
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

class ResultLifetime
{
public:
    using InitializeResult = Common::Result<Test::InstanceCounter<>, Test::InstanceCounter<>>;

    static InitializeResult Create(bool success)
    {
        if(success)
        {
            auto result = Test::InstanceCounter<>();
            return Common::Success(std::move(result));
        }
        else
        {
            return Common::Failure(Test::InstanceCounter<>());
        }
    }
};

DOCTEST_TEST_CASE("Result")
{
    DOCTEST_SUBCASE("With void type")
    {
        ResultWithVoid instance;
        DOCTEST_CHECK(instance.Initialize(true));
        DOCTEST_CHECK_FALSE(instance.Initialize(false));
    }

    DOCTEST_SUBCASE("With enum type")
    {
        ResultWithEnum instance;
        DOCTEST_CHECK(instance.Initialize(true));
        DOCTEST_CHECK_FALSE(instance.Initialize(false));

        DOCTEST_CHECK_EQ(instance.Initialize(true)
            .UnwrapSuccessOr(ResultWithEnum::JustGood), ResultWithEnum::VeryGood);
        DOCTEST_CHECK_EQ(instance.Initialize(false)
            .UnwrapSuccessOr(ResultWithEnum::JustGood), ResultWithEnum::JustGood);

        DOCTEST_CHECK_EQ(instance.Initialize(false).UnwrapFailureOr(69), 42);
        DOCTEST_CHECK_EQ(instance.Initialize(true).UnwrapFailureOr(69), 69);
    }

    DOCTEST_SUBCASE("With enum class type")
    {
        ResultWithEnumClass instance;
        DOCTEST_CHECK(instance.Initialize(true));
        DOCTEST_CHECK_FALSE(instance.Initialize(false));

        DOCTEST_CHECK_EQ(instance.Initialize(true)
            .UnwrapSuccessOr(ResultWithEnumClass::JustGood), ResultWithEnumClass::VeryGood);
        DOCTEST_CHECK_EQ(instance.Initialize(false)
            .UnwrapSuccessOr(ResultWithEnumClass::JustGood), ResultWithEnumClass::JustGood);

        DOCTEST_CHECK_EQ(instance.Initialize(false).UnwrapFailureOr(69), 42);
        DOCTEST_CHECK_EQ(instance.Initialize(true).UnwrapFailureOr(69), 69);
    }

    DOCTEST_SUBCASE("With string type")
    {
        ResultWithString instance;
        DOCTEST_CHECK_EQ(instance.Initialize(true)
            .UnwrapSuccessOr("goodbye world!"), "hello world!");
        DOCTEST_CHECK_EQ(instance.Initialize(false)
            .UnwrapSuccessOr("goodbye world!"), "goodbye world!");
        DOCTEST_CHECK_EQ(instance.Initialize(false)
            .UnwrapFailureOr("hello world!"), "goodbye world!");
        DOCTEST_CHECK_EQ(instance.Initialize(true)
            .UnwrapSuccessOr("hello world!"), "hello world!");
    }

    DOCTEST_SUBCASE("Unwrap")
    {
        auto result = ResultUnwrap::Create("Hello world!");
        DOCTEST_CHECK(result.IsSuccess());
        DOCTEST_CHECK_FALSE(result.IsFailure());

        auto instance = result.Unwrap();
        DOCTEST_CHECK_EQ(instance->text, "Hello world!");

        DOCTEST_CHECK(result.IsSuccess());
        DOCTEST_CHECK_FALSE(result.IsFailure());
    }

    DOCTEST_SUBCASE("Unwrap or")
    {
        auto result = ResultUnwrap::Create("Goodbye world!");
        DOCTEST_CHECK_FALSE(result.IsSuccess());
        DOCTEST_CHECK(result.IsFailure());

        auto instance = result.UnwrapOr(nullptr);
        DOCTEST_CHECK_EQ(instance, nullptr);

        DOCTEST_CHECK_FALSE(result.IsSuccess());
        DOCTEST_CHECK(result.IsFailure());
    }

    DOCTEST_SUBCASE("Unwrap either")
    {
        ResultWithString instance;

        auto resultSuccess = instance.Initialize(true).UnwrapEither();
        DOCTEST_CHECK_EQ(resultSuccess, "hello world!");

        auto resultFailure = instance.Initialize(false).UnwrapEither();
        DOCTEST_CHECK_EQ(resultFailure, "goodbye world!");
    }

    DOCTEST_SUBCASE("As failure")
    {
        ResultWithString instance;

        auto resultFailure = instance.Initialize(false).AsFailure();
        DOCTEST_CHECK(resultFailure.IsSuccess());
        DOCTEST_CHECK_EQ(resultFailure.Unwrap(), "goodbye world!");
    }

    DOCTEST_SUBCASE("Ownership")
    {
        std::shared_ptr<int> shared = std::make_shared<int>(42);
        auto Create = [&shared]() -> Common::Result<std::shared_ptr<int>, std::shared_ptr<int>>
        {
            return Common::Success(shared);
        };

        auto result = Create();
        auto pointer = result.Unwrap();

        DOCTEST_CHECK_NE(shared.get(), nullptr);
        DOCTEST_CHECK_NE(pointer.get(), nullptr);
        DOCTEST_CHECK_EQ(shared.use_count(), 2);
    }

    DOCTEST_SUBCASE("Lifetime")
    {
        DOCTEST_SUBCASE("Unwrap lvalue")
        {
            Test::InstanceCounter<> counter = ResultLifetime::Create(true).Unwrap();

            DOCTEST_CHECK_EQ(counter.GetStats().instances, 1);
            DOCTEST_CHECK_EQ(counter.GetStats().constructions, 4);
            DOCTEST_CHECK_EQ(counter.GetStats().destructions, 3);
            DOCTEST_CHECK_EQ(counter.GetStats().copies, 0);
            DOCTEST_CHECK_EQ(counter.GetStats().moves, 3);
        }

        DOCTEST_SUBCASE("Unwrap rvalue")
        {
            Test::InstanceCounter<> counter = ResultLifetime::Create(false).UnwrapFailure();

            DOCTEST_CHECK_EQ(counter.GetStats().instances, 1);
            DOCTEST_CHECK_EQ(counter.GetStats().constructions, 4);
            DOCTEST_CHECK_EQ(counter.GetStats().destructions, 3);
            DOCTEST_CHECK_EQ(counter.GetStats().copies, 0);
            DOCTEST_CHECK_EQ(counter.GetStats().moves, 3);
        }

        DOCTEST_SUBCASE("Unwrap assign")
        {
            Test::InstanceCounter<> counter;
            counter = ResultLifetime::Create(true).Unwrap();

            DOCTEST_CHECK_EQ(counter.GetStats().instances, 1);
            DOCTEST_CHECK_EQ(counter.GetStats().constructions, 4);
            DOCTEST_CHECK_EQ(counter.GetStats().destructions, 3);
            DOCTEST_CHECK_EQ(counter.GetStats().copies, 0);
            DOCTEST_CHECK_EQ(counter.GetStats().moves, 4);
        }

        DOCTEST_SUBCASE("Unwrap stored")
        {
            auto result = ResultLifetime::Create(true);
            Test::InstanceCounter<> counter = result.Unwrap();

            DOCTEST_CHECK_EQ(counter.GetStats().instances, 2);
            DOCTEST_CHECK_EQ(counter.GetStats().constructions, 4);
            DOCTEST_CHECK_EQ(counter.GetStats().destructions, 2);
            DOCTEST_CHECK_EQ(counter.GetStats().copies, 0);
            DOCTEST_CHECK_EQ(counter.GetStats().moves, 3);
        }

        DOCTEST_SUBCASE("Unwrap success or")
        {
            auto result = ResultLifetime::Create(false);
            Test::InstanceCounter<> counter = result.UnwrapSuccessOr(Test::InstanceCounter<>());

            DOCTEST_CHECK_EQ(counter.GetStats().instances, 1);
            DOCTEST_CHECK_LE(counter.GetStats().constructions, 3);
            DOCTEST_CHECK_LE(counter.GetStats().destructions, 2);
            DOCTEST_CHECK_EQ(counter.GetStats().copies, 0);
            DOCTEST_CHECK_LE(counter.GetStats().moves, 2);
        }

        DOCTEST_SUBCASE("Unwrap failure or")
        {
            auto result = ResultLifetime::Create(true);
            Test::InstanceCounter<> counter = result.UnwrapFailureOr(Test::InstanceCounter<>());

            DOCTEST_CHECK_EQ(counter.GetStats().instances, 1);
            DOCTEST_CHECK_LE(counter.GetStats().constructions, 3);
            DOCTEST_CHECK_LE(counter.GetStats().destructions, 2);
            DOCTEST_CHECK_EQ(counter.GetStats().copies, 0);
            DOCTEST_CHECK_LE(counter.GetStats().moves, 2);
        }
    }
}
