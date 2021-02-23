/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <memory>
#include <doctest/doctest.h>
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

    SUBCASE("With enum class type")
    {
        ResultWithEnumClass instance;
        CHECK(instance.Initialize(true));
        CHECK_FALSE(instance.Initialize(false));

        CHECK_EQ(instance.Initialize(true)
            .UnwrapSuccessOr(ResultWithEnumClass::JustGood), ResultWithEnumClass::VeryGood);
        CHECK_EQ(instance.Initialize(false)
            .UnwrapSuccessOr(ResultWithEnumClass::JustGood), ResultWithEnumClass::JustGood);

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
        CHECK(result.IsSuccess());
        CHECK_FALSE(result.IsFailure());

        auto instance = result.Unwrap();
        CHECK_EQ(instance->text, "Hello world!");

        CHECK(result.IsSuccess());
        CHECK_FALSE(result.IsFailure());
    }

    SUBCASE("Unwrap or")
    {
        auto result = ResultUnwrap::Create("Goodbye world!");
        CHECK_FALSE(result.IsSuccess());
        CHECK(result.IsFailure());

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

    SUBCASE("Ownership")
    {
        std::shared_ptr<int> shared = std::make_shared<int>(42);
        auto Create = [&shared]() -> Common::Result<std::shared_ptr<int>, std::shared_ptr<int>>
        {
            return Common::Success(shared);
        };

        auto result = Create();
        auto pointer = result.Unwrap();

        CHECK_NE(shared.get(), nullptr);
        CHECK_NE(pointer.get(), nullptr);
        CHECK_EQ(shared.use_count(), 2);
    }

    SUBCASE("Lifetime")
    {
        SUBCASE("Unwrap lvalue")
        {
            Test::InstanceCounter<> counter = ResultLifetime::Create(true).Unwrap();

            CHECK_EQ(counter.GetStats().instances, 1);
            CHECK_EQ(counter.GetStats().constructions, 4);
            CHECK_EQ(counter.GetStats().destructions, 3);
            CHECK_EQ(counter.GetStats().copies, 0);
            CHECK_EQ(counter.GetStats().moves, 3);
        }

        SUBCASE("Unwrap rvalue")
        {
            Test::InstanceCounter<> counter = ResultLifetime::Create(false).UnwrapFailure();

            CHECK_EQ(counter.GetStats().instances, 1);
            CHECK_EQ(counter.GetStats().constructions, 4);
            CHECK_EQ(counter.GetStats().destructions, 3);
            CHECK_EQ(counter.GetStats().copies, 0);
            CHECK_EQ(counter.GetStats().moves, 3);
        }

        SUBCASE("Unwrap assign")
        {
            Test::InstanceCounter<> counter;
            counter = ResultLifetime::Create(true).Unwrap();

            CHECK_EQ(counter.GetStats().instances, 1);
            CHECK_EQ(counter.GetStats().constructions, 4);
            CHECK_EQ(counter.GetStats().destructions, 3);
            CHECK_EQ(counter.GetStats().copies, 0);
            CHECK_EQ(counter.GetStats().moves, 4);
        }

        SUBCASE("Unwrap stored")
        {
            auto result = ResultLifetime::Create(true);
            Test::InstanceCounter<> counter = result.Unwrap();

            CHECK_EQ(counter.GetStats().instances, 2);
            CHECK_EQ(counter.GetStats().constructions, 4);
            CHECK_EQ(counter.GetStats().destructions, 2);
            CHECK_EQ(counter.GetStats().copies, 0);
            CHECK_EQ(counter.GetStats().moves, 3);
        }

        SUBCASE("Unwrap success or")
        {
            auto result = ResultLifetime::Create(false);
            Test::InstanceCounter<> counter = result.UnwrapSuccessOr(Test::InstanceCounter<>());

            CHECK_EQ(counter.GetStats().instances, 1);
            CHECK_EQ(counter.GetStats().constructions, 3);
            CHECK_EQ(counter.GetStats().destructions, 2);
            CHECK_EQ(counter.GetStats().copies, 0);
            CHECK_EQ(counter.GetStats().moves, 2);
        }

        SUBCASE("Unwrap failure or")
        {
            auto result = ResultLifetime::Create(true);
            Test::InstanceCounter<> counter = result.UnwrapFailureOr(Test::InstanceCounter<>());

            CHECK_EQ(counter.GetStats().instances, 1);
            CHECK_EQ(counter.GetStats().constructions, 3);
            CHECK_EQ(counter.GetStats().destructions, 2);
            CHECK_EQ(counter.GetStats().copies, 0);
            CHECK_EQ(counter.GetStats().moves, 2);
        }
    }
}
