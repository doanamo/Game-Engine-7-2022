/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <Common/Testing/InstanceCounter.hpp>
#include <Common/Utility/Result.hpp>

using Test::InstanceCounter;

/*
    Helpers
*/

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
    using InitializeResult = Common::Result<InstanceCounter<>, InstanceCounter<>>;

    static InitializeResult Create(bool success)
    {
        if(success)
        {
            auto result = InstanceCounter<>();
            return Common::Success(std::move(result));
        }
        else
        {
            return Common::Failure(InstanceCounter<>());
        }
    }
};

/*
    Result
*/

TEST(Result, VoidType)
{
    ResultWithVoid instance;
    EXPECT_TRUE(instance.Initialize(true));
    EXPECT_FALSE(instance.Initialize(false));
}

TEST(Result, EnumType)
{
    ResultWithEnum instance;
    EXPECT_TRUE(instance.Initialize(true));
    EXPECT_FALSE(instance.Initialize(false));

    EXPECT_EQ(instance.Initialize(true)
        .UnwrapSuccessOr(ResultWithEnum::JustGood), ResultWithEnum::VeryGood);
    EXPECT_EQ(instance.Initialize(false)
        .UnwrapSuccessOr(ResultWithEnum::JustGood), ResultWithEnum::JustGood);

    EXPECT_EQ(instance.Initialize(false).UnwrapFailureOr(69), 42);
    EXPECT_EQ(instance.Initialize(true).UnwrapFailureOr(69), 69);
}

TEST(Result, EnumClassType)
{
    ResultWithEnumClass instance;
    EXPECT_TRUE(instance.Initialize(true));
    EXPECT_FALSE(instance.Initialize(false));

    EXPECT_EQ(instance.Initialize(true)
        .UnwrapSuccessOr(ResultWithEnumClass::JustGood), ResultWithEnumClass::VeryGood);
    EXPECT_EQ(instance.Initialize(false)
        .UnwrapSuccessOr(ResultWithEnumClass::JustGood), ResultWithEnumClass::JustGood);

    EXPECT_EQ(instance.Initialize(false).UnwrapFailureOr(69), 42);
    EXPECT_EQ(instance.Initialize(true).UnwrapFailureOr(69), 69);
}

TEST(Result, StringType)
{
    ResultWithString instance;
    EXPECT_EQ(instance.Initialize(true).UnwrapSuccessOr("goodbye world!"), "hello world!");
    EXPECT_EQ(instance.Initialize(false).UnwrapSuccessOr("goodbye world!"), "goodbye world!");
    EXPECT_EQ(instance.Initialize(false).UnwrapFailureOr("hello world!"), "goodbye world!");
    EXPECT_EQ(instance.Initialize(true).UnwrapSuccessOr("hello world!"), "hello world!");
}

TEST(Result, Unwrap)
{
    auto result = ResultUnwrap::Create("Hello world!");
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_FALSE(result.IsFailure());

    auto instance = result.Unwrap();
    EXPECT_EQ(instance->text, "Hello world!");

    EXPECT_TRUE(result.IsSuccess());
    EXPECT_FALSE(result.IsFailure());
}

TEST(Result, UnwrapOr)
{
    auto result = ResultUnwrap::Create("Goodbye world!");
    EXPECT_FALSE(result.IsSuccess());
    EXPECT_TRUE(result.IsFailure());

    auto instance = result.UnwrapOr(nullptr);
    EXPECT_EQ(instance, nullptr);

    EXPECT_FALSE(result.IsSuccess());
    EXPECT_TRUE(result.IsFailure());
}

TEST(Result, UnwrapEither)
{
    ResultWithString instance;

    auto resultSuccess = instance.Initialize(true).UnwrapEither();
    EXPECT_EQ(resultSuccess, "hello world!");

    auto resultFailure = instance.Initialize(false).UnwrapEither();
    EXPECT_EQ(resultFailure, "goodbye world!");
}

TEST(Result, AsFailure)
{
    ResultWithString instance;

    auto resultFailure = instance.Initialize(false).AsFailure();
    EXPECT_TRUE(resultFailure.IsSuccess());
    EXPECT_EQ(resultFailure.Unwrap(), "goodbye world!");
}

TEST(Result, Ownership)
{
    std::shared_ptr<int> shared = std::make_shared<int>(42);
    auto Create = [&shared]() -> Common::Result<std::shared_ptr<int>, std::shared_ptr<int>>
    {
        return Common::Success(shared);
    };

    auto result = Create();
    auto pointer = result.Unwrap();

    EXPECT_NE(shared.get(), nullptr);
    EXPECT_NE(pointer.get(), nullptr);
    EXPECT_EQ(shared.use_count(), 2);
}

TEST(ResultLifetime, UnwrapLvalue)
{
    InstanceCounter<> counter = ResultLifetime::Create(true).Unwrap();

    EXPECT_EQ(counter.GetStats().instances, 1);
    EXPECT_EQ(counter.GetStats().constructions, 4);
    EXPECT_EQ(counter.GetStats().destructions, 3);
    EXPECT_EQ(counter.GetStats().copies, 0);
    EXPECT_EQ(counter.GetStats().moves, 3);
}

TEST(ResultLifetime, UnwrapRvalue)
{
    InstanceCounter<> counter = ResultLifetime::Create(false).UnwrapFailure();

    EXPECT_EQ(counter.GetStats().instances, 1);
    EXPECT_EQ(counter.GetStats().constructions, 4);
    EXPECT_EQ(counter.GetStats().destructions, 3);
    EXPECT_EQ(counter.GetStats().copies, 0);
    EXPECT_EQ(counter.GetStats().moves, 3);
}

TEST(ResultLifetime, UnwrapAssign)
{
    InstanceCounter<> counter;
    counter = ResultLifetime::Create(true).Unwrap();

    EXPECT_EQ(counter.GetStats().instances, 1);
    EXPECT_EQ(counter.GetStats().constructions, 4);
    EXPECT_EQ(counter.GetStats().destructions, 3);
    EXPECT_EQ(counter.GetStats().copies, 0);
    EXPECT_EQ(counter.GetStats().moves, 4);
}

TEST(ResultLifetime, UnwrapStored)
{
    auto result = ResultLifetime::Create(true);
    InstanceCounter<> counter = result.Unwrap();

    EXPECT_EQ(counter.GetStats().instances, 2);
    EXPECT_EQ(counter.GetStats().constructions, 4);
    EXPECT_EQ(counter.GetStats().destructions, 2);
    EXPECT_EQ(counter.GetStats().copies, 0);
    EXPECT_EQ(counter.GetStats().moves, 3);
}

TEST(ResultLifetime, UnwrapSuccessOr)
{
    auto result = ResultLifetime::Create(false);
    InstanceCounter<> counter = result.UnwrapSuccessOr(InstanceCounter<>());

    EXPECT_EQ(counter.GetStats().instances, 1);
    EXPECT_LE(counter.GetStats().constructions, 3);
    EXPECT_LE(counter.GetStats().destructions, 2);
    EXPECT_EQ(counter.GetStats().copies, 0);
    EXPECT_LE(counter.GetStats().moves, 2);
}

TEST(ResultLifetime, UnwrapFailureOr)
{
    auto result = ResultLifetime::Create(true);
    InstanceCounter<> counter = result.UnwrapFailureOr(InstanceCounter<>());

    EXPECT_EQ(counter.GetStats().instances, 1);
    EXPECT_LE(counter.GetStats().constructions, 3);
    EXPECT_LE(counter.GetStats().destructions, 2);
    EXPECT_EQ(counter.GetStats().copies, 0);
    EXPECT_LE(counter.GetStats().moves, 2);
}
