/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include <Common/Result.hpp>
#include <TestHelpers.hpp>

class ResultWithVoid
{
public:
    using InitializeResult = Result<void, void>;

    InitializeResult Initialize(bool success)
    {
        if(!success)
        {
            return Failure();
        }

        return Success();
    }
};

bool TestVoid()
{
    ResultWithVoid instance;

    TEST_TRUE(instance.Initialize(true));
    TEST_FALSE(instance.Initialize(false));

    return true;
}

class ResultWithEnum
{
public:
    enum SuccessResult
    {
        VeryGood,
        JustGood,
    };

    using FailureResult = int;
    using InitializeResult = Result<SuccessResult, FailureResult>;

    InitializeResult Initialize(bool success)
    {
        if(!success)
        {
            return Failure(42);
        }

        return Success(VeryGood);
    }
};

bool TestEnum()
{
    ResultWithEnum instance;
    
    TEST_TRUE(instance.Initialize(true));
    TEST_FALSE(instance.Initialize(false));

    TEST_EQ(instance.Initialize(true).UnwrapSuccessOr(ResultWithEnum::JustGood), ResultWithEnum::VeryGood);
    TEST_EQ(instance.Initialize(false).UnwrapSuccessOr(ResultWithEnum::JustGood), ResultWithEnum::JustGood);
    TEST_EQ(instance.Initialize(false).UnwrapFailureOr(69), 42);
    TEST_EQ(instance.Initialize(true).UnwrapFailureOr(69), 69);

    return true;
}

class ResultWithString
{
public:
    using InitializeResult = Result<std::string, std::string>;

    InitializeResult Initialize(bool success)
    {
        if(!success)
        {
            std::string text = "goodbye world!";
            return Failure(text);
        }

        return Success(std::string("hello world!"));
    }
};

bool TestString()
{
    ResultWithString instance;

    TEST_EQ(instance.Initialize(true).UnwrapSuccessOr("goodbye world!"), "hello world!");
    TEST_EQ(instance.Initialize(false).UnwrapSuccessOr("goodbye world!"), "goodbye world!");
    TEST_EQ(instance.Initialize(false).UnwrapFailureOr("hello world!"), "goodbye world!");
    TEST_EQ(instance.Initialize(true).UnwrapSuccessOr("hello world!"), "hello world!");

    return true;
}

class ResultUnwrap
{
public:
    using InitializeResult = Result<std::unique_ptr<ResultUnwrap>, void>;

    static InitializeResult Create(std::string text)
    {
        if(text == "Goodbye world!")
            return Failure();

        auto instance = std::make_unique<ResultUnwrap>();
        instance->text = text;

        return Success(std::move(instance));
    }

    std::string text;
};

bool TestUnwrap()
{
    {
        auto result = ResultUnwrap::Create("Hello world!");
        auto instance = result.Unwrap();

        TEST_EQ(instance->text, "Hello world!");
        TEST_TRUE(result.IsSuccess());
        TEST_FALSE(result.IsFailure());
    }

    {
        auto result = ResultUnwrap::Create("Goodbye world!");
        auto instance = result.UnwrapOr(nullptr);

        TEST_EQ(instance, nullptr);
        TEST_FALSE(result.IsSuccess());
        TEST_TRUE(result.IsFailure());
    }

    return true;
}

int main()
{
    TEST_RUN(TestVoid);
    TEST_RUN(TestEnum);
    TEST_RUN(TestString);
    TEST_RUN(TestUnwrap);
}
