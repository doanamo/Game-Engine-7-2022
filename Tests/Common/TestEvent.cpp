/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <memory>
#include <functional>
#include <Common/Utility/Delegate.hpp>
#include <Common/Events/EventCollector.hpp>
#include <Common/Events/EventDispatcher.hpp>
#include <Common/Events/EventReceiver.hpp>
#include <Common/Events/EventBroker.hpp>
#include <Common/Testing/InstanceCounter.hpp>
#include <gtest/gtest.h>

using Test::InstanceCounter;

/*
    Helpers
*/

static const char* Text = "0123456789";

char Function(InstanceCounter<> instance, int index)
{
    return Text[index];
}

class BaseClass
{
public:
    virtual char Method(InstanceCounter<> instance, int index)
    {
        return Text[index];
    }
};

class DerivedClass : public BaseClass
{
public:
    char Method(InstanceCounter<> instance, int index) override
    {
        return Text[index];
    }
};

/*
    Event Delegate
*/

class EventDelegateBinding : public testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_FALSE(delegate.IsBound());
    }

    void TearDown() override
    {
        delegate.Bind(nullptr);
        EXPECT_FALSE(delegate.IsBound());
        EXPECT_EQ(counter.GetStats().copies, 1);
    }

protected:
    InstanceCounter<> counter;
    Event::Delegate<char(InstanceCounter<>, int)> delegate;
};

TEST_F(EventDelegateBinding, StaticFunction)
{
    delegate.Bind<&Function>();
    EXPECT_TRUE(delegate.IsBound());
    EXPECT_EQ(delegate.Invoke(counter, 4), '4');
}

TEST_F(EventDelegateBinding, ClassMethod)
{
    BaseClass baseClass;
    delegate.Bind<BaseClass, &BaseClass::Method>(&baseClass);
    EXPECT_TRUE(delegate.IsBound());
    EXPECT_EQ(delegate.Invoke(counter, 6), '6');
}

TEST_F(EventDelegateBinding, VirtualMethod)
{
    DerivedClass derivedClass;
    delegate.Bind<BaseClass, &BaseClass::Method>(&derivedClass);
    EXPECT_TRUE(delegate.IsBound());
    EXPECT_EQ(delegate.Invoke(counter, 1), '1');
}

TEST_F(EventDelegateBinding, LambdaFunction)
{
    auto functor = [](InstanceCounter<> counter, int index) -> char
    {
        return Text[index];
    };

    delegate.Bind(&functor);
    EXPECT_TRUE(delegate.IsBound());
    EXPECT_EQ(delegate.Invoke(counter, 9), '9');
}

TEST_F(EventDelegateBinding, LambdaCapture)
{
    int modifier = 4;
    delegate = Event::Delegate<char(InstanceCounter<>, int)>(
        [&modifier](InstanceCounter<> counter, int index) -> char
    {
        return Text[index + modifier];
    });

    EXPECT_TRUE(delegate.IsBound());
    EXPECT_EQ(delegate.Invoke(counter, 3), '7');
}

TEST_F(EventDelegateBinding, LambdaArgumentBinding)
{
    auto functor = [](InstanceCounter<> counter, int index, int modifier) -> char
    {
        return Text[index + modifier];
    };

    delegate = std::bind(functor, std::placeholders::_1, std::placeholders::_2, 4);

    EXPECT_TRUE(delegate.IsBound());
    EXPECT_EQ(delegate.Invoke(counter, 3), '7');
}

class EventDelegateLifetime : public testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_EQ(counter.GetStats().instances, expectedInstances += 1);
    }

    void TearDown() override
    {
        EXPECT_EQ(counter.GetStats().instances, expectedInstances);
        EXPECT_EQ(counter.GetStats().copies, expectedCopies);

        delegate.Invoke();
        EXPECT_EQ(currentValue, expectedValue);

        EXPECT_EQ(counter.GetStats().instances, expectedInstances);
        EXPECT_EQ(counter.GetStats().copies, expectedCopies);

        delegate = nullptr;

        EXPECT_EQ(counter.GetStats().instances, expectedInstances -= 1);
        EXPECT_EQ(counter.GetStats().copies, expectedCopies);
    }

protected:
    int currentValue = 0;
    int expectedValue = 0;
    int expectedCopies = 0;
    int expectedInstances = 0;

    InstanceCounter<> counter;
    Event::Delegate<void()> delegate;
};

TEST_F(EventDelegateLifetime, BindLvalueLambda)
{
    {
        auto lambda = [&currentValue = currentValue, counter = counter]()
        {
            currentValue += 1;
        };

        EXPECT_EQ(counter.GetStats().instances, expectedInstances += 1);
        EXPECT_EQ(counter.GetStats().copies, expectedCopies += 1);

        delegate = lambda;
        expectedValue += 1;

        EXPECT_EQ(counter.GetStats().instances, expectedInstances += 1);
        EXPECT_EQ(counter.GetStats().copies, expectedCopies += 1);
    }

    EXPECT_EQ(counter.GetStats().instances, expectedInstances -= 1);
    EXPECT_EQ(counter.GetStats().copies, expectedCopies);
}

TEST_F(EventDelegateLifetime, BindRvalueLambda)
{
    delegate.Bind([&currentValue = currentValue, counter = counter]()
    {
        currentValue += 10;
    });

    expectedValue += 10;

    EXPECT_EQ(counter.GetStats().instances, expectedInstances += 1);
    EXPECT_EQ(counter.GetStats().copies, expectedCopies += 1);
}

TEST_F(EventDelegateLifetime, CopyDelegate)
{
    Event::Delegate<void()> delegateCopy([&currentValue = currentValue, counter = counter]()
    {
        currentValue += 100;
    });

    expectedValue += 100;

    EXPECT_EQ(counter.GetStats().instances, expectedInstances += 1);
    EXPECT_EQ(counter.GetStats().copies, expectedCopies += 1);

    delegate = delegateCopy;

    EXPECT_EQ(counter.GetStats().instances, expectedInstances += 1);
    EXPECT_EQ(counter.GetStats().copies, expectedCopies += 1);

    delegateCopy = nullptr;

    EXPECT_EQ(counter.GetStats().instances, expectedInstances -= 1);
    EXPECT_EQ(counter.GetStats().copies, expectedCopies);
}

TEST_F(EventDelegateLifetime, MoveDelegate)
{
    Event::Delegate<void()> delegateMove([&currentValue = currentValue, counter = counter]()
    {
        currentValue += 1000;
    });

    expectedValue += 1000;

    EXPECT_EQ(counter.GetStats().instances, expectedInstances += 1);
    EXPECT_EQ(counter.GetStats().copies, expectedCopies += 1);

    delegate = std::move(delegateMove);

    EXPECT_EQ(counter.GetStats().instances, expectedInstances);
    EXPECT_EQ(counter.GetStats().copies, expectedCopies);

    delegateMove = nullptr;

    EXPECT_EQ(counter.GetStats().instances, expectedInstances);
    EXPECT_EQ(counter.GetStats().copies, expectedCopies);
}

TEST(EventDelegate, LambdaSimilarSignatures)
{
    int i = 0, y = 0;

    Event::Delegate<void()> delegateOne([&i]() { i = 3; });
    Event::Delegate<void()> delegateTwo([&y]() { y = 7; });

    delegateOne.Invoke();
    delegateTwo.Invoke();

    EXPECT_EQ(i, 3);
    EXPECT_EQ(y, 7);
}

/*
    Event Collector
*/

TEST(EventCollector, CollectNothing)
{
    Event::CollectNothing collectNothing;
    EXPECT_TRUE(collectNothing.ShouldContinue());
}

TEST(EventCollector, CollectLast)
{
    Event::CollectLast<int> collectLast(0);

    for(int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(collectLast.GetResult(), i);
        collectLast.ConsumeResult(i + 1);
        EXPECT_TRUE(collectLast.ShouldContinue());
        EXPECT_EQ(collectLast.GetResult(), i + 1);
    }
}

TEST(EventCollector, CollectWhileTrue)
{
    Event::CollectWhileTrue collectWhileTrue(true);
    EXPECT_TRUE(collectWhileTrue.GetResult());

    collectWhileTrue.ConsumeResult(true);
    EXPECT_TRUE(collectWhileTrue.ShouldContinue());
    EXPECT_TRUE(collectWhileTrue.GetResult());

    collectWhileTrue.ConsumeResult(false);
    EXPECT_FALSE(collectWhileTrue.ShouldContinue());
    EXPECT_FALSE(collectWhileTrue.GetResult());

    collectWhileTrue.Reset();

    collectWhileTrue.ConsumeResult(true);
    EXPECT_TRUE(collectWhileTrue.ShouldContinue());
    EXPECT_TRUE(collectWhileTrue.GetResult());

    collectWhileTrue.ConsumeResult(false);
    EXPECT_FALSE(collectWhileTrue.ShouldContinue());
    EXPECT_FALSE(collectWhileTrue.GetResult());
}

TEST(EventCollector, CollectWhileFalse)
{
    Event::CollectWhileFalse collectWhileFalse(false);
    EXPECT_FALSE(collectWhileFalse.GetResult());

    collectWhileFalse.ConsumeResult(false);
    EXPECT_TRUE(collectWhileFalse.ShouldContinue());
    EXPECT_FALSE(collectWhileFalse.GetResult());

    collectWhileFalse.ConsumeResult(true);
    EXPECT_FALSE(collectWhileFalse.ShouldContinue());
    EXPECT_TRUE(collectWhileFalse.GetResult());

    collectWhileFalse.Reset();

    collectWhileFalse.ConsumeResult(false);
    EXPECT_TRUE(collectWhileFalse.ShouldContinue());
    EXPECT_FALSE(collectWhileFalse.GetResult());

    collectWhileFalse.ConsumeResult(true);
    EXPECT_FALSE(collectWhileFalse.ShouldContinue());
    EXPECT_TRUE(collectWhileFalse.GetResult());
}

/*
    Event Dispatcher
*/

class EventDispatcherCollector : public testing::Test
{
protected:
    void SetUp() override
    {
        receiverAddOne.Bind([](int& i) { i += 1; return i; });
        receiverAddTwo.Bind([](int& i) { i += 2; return i; });
        receiverTrue.Bind([](int& i) { i += 1; return true; });
        receiverFalse.Bind([](int& i) { i += 2; return false; });
        receiverDummy.Bind([](int& i) { i += 9999; return true; });

        ASSERT_TRUE(receiverAddOne.IsBound());
        ASSERT_TRUE(receiverAddTwo.IsBound());
        ASSERT_TRUE(receiverTrue.IsBound());
        ASSERT_TRUE(receiverFalse.IsBound());
        ASSERT_TRUE(receiverDummy.IsBound());
    }

    void TearDown() override
    {
        EXPECT_FALSE(receiverAddOne.IsSubscribed());
        EXPECT_FALSE(receiverAddTwo.IsSubscribed());
        EXPECT_FALSE(receiverTrue.IsSubscribed());
        EXPECT_FALSE(receiverFalse.IsSubscribed());
        EXPECT_FALSE(receiverDummy.IsSubscribed());
    }

protected:
    Event::Receiver<int(int&)> receiverAddOne;
    Event::Receiver<int(int&)> receiverAddTwo;
    Event::Receiver<bool(int&)> receiverTrue;
    Event::Receiver<bool(int&)> receiverFalse;
    Event::Receiver<bool(int&)> receiverDummy;
};

TEST_F(EventDispatcherCollector, CollectLast)
{
    int i = 0;

    Event::Dispatcher<int(int&)> dispatcher(0);
    EXPECT_EQ(dispatcher.Dispatch(i), 0);

    EXPECT_TRUE(dispatcher.Subscribe(receiverAddOne));
    EXPECT_TRUE(dispatcher.Subscribe(receiverAddOne));
    EXPECT_EQ(dispatcher.Dispatch(i), 1);

    EXPECT_TRUE(dispatcher.Subscribe(receiverAddTwo));
    EXPECT_TRUE(dispatcher.Subscribe(receiverAddTwo));
    EXPECT_EQ(dispatcher.Dispatch(i), 4);

    EXPECT_TRUE(receiverAddOne.Unsubscribe());
    EXPECT_FALSE(receiverAddOne.Unsubscribe());
    EXPECT_EQ(dispatcher.Dispatch(i), 6);

    EXPECT_TRUE(dispatcher.Unsubscribe(receiverAddTwo));
    EXPECT_FALSE(dispatcher.Unsubscribe(receiverAddTwo));
    EXPECT_EQ(dispatcher.Dispatch(i), 0);
}

TEST_F(EventDispatcherCollector, CollectBoolWhileTrue)
{
    int i = 0;

    auto collector = std::make_unique<Event::CollectWhileTrue>(true);
    Event::Dispatcher<bool(int&)> dispatcherWhileTrue(std::move(collector));
    EXPECT_TRUE(dispatcherWhileTrue.Dispatch(i));
    EXPECT_EQ(i, 0);

    EXPECT_TRUE(dispatcherWhileTrue.Subscribe(receiverTrue));
    EXPECT_TRUE(dispatcherWhileTrue.Subscribe(receiverTrue));
    EXPECT_TRUE(dispatcherWhileTrue.Dispatch(i));
    EXPECT_EQ(i, 1);

    EXPECT_TRUE(dispatcherWhileTrue.Subscribe(receiverFalse));
    EXPECT_TRUE(dispatcherWhileTrue.Subscribe(receiverFalse));
    EXPECT_FALSE(dispatcherWhileTrue.Dispatch(i));
    EXPECT_EQ(i, 4);

    EXPECT_TRUE(dispatcherWhileTrue.Subscribe(receiverDummy));
    EXPECT_TRUE(dispatcherWhileTrue.Subscribe(receiverDummy));
    EXPECT_FALSE(dispatcherWhileTrue.Dispatch(i));
    EXPECT_EQ(i, 7);
}

TEST_F(EventDispatcherCollector, CollectBoolWhileFalse)
{
    int i = 0;

    auto collector = std::make_unique<Event::CollectWhileFalse>(false);
    Event::Dispatcher<bool(int&)> dispatcherWhileFalse(std::move(collector));
    EXPECT_FALSE(dispatcherWhileFalse.Dispatch(i));
    EXPECT_EQ(i, 0);

    EXPECT_TRUE(dispatcherWhileFalse.Subscribe(receiverFalse));
    EXPECT_TRUE(dispatcherWhileFalse.Subscribe(receiverFalse));
    EXPECT_FALSE(dispatcherWhileFalse.Dispatch(i));
    EXPECT_EQ(i, 2);

    EXPECT_TRUE(dispatcherWhileFalse.Subscribe(receiverTrue));
    EXPECT_TRUE(dispatcherWhileFalse.Subscribe(receiverTrue));
    EXPECT_TRUE(dispatcherWhileFalse.Dispatch(i));
    EXPECT_EQ(i, 5);

    EXPECT_TRUE(dispatcherWhileFalse.Subscribe(receiverDummy));
    EXPECT_TRUE(dispatcherWhileFalse.Subscribe(receiverDummy));
    EXPECT_TRUE(dispatcherWhileFalse.Dispatch(i));
    EXPECT_EQ(i, 8);
}

TEST_F(EventDispatcherCollector, CollectBoolInitialFalse)
{
    int i = 0;

    auto collector = std::make_unique<Event::CollectWhileTrue>(false);
    Event::Dispatcher<bool(int&)> dispatcherWhileTrue(std::move(collector));
    EXPECT_FALSE(dispatcherWhileTrue.Dispatch(i));
    EXPECT_EQ(i, 0);

    dispatcherWhileTrue.Subscribe(receiverTrue);
    EXPECT_FALSE(dispatcherWhileTrue.Dispatch(i));
    EXPECT_EQ(i, 0);
}

TEST_F(EventDispatcherCollector, CollectBoolInitialTrue)
{
    int i = 0;

    auto collector = std::make_unique<Event::CollectWhileFalse>(true);
    Event::Dispatcher<bool(int&)> dispatcherWhileFalse(std::move(collector));
    EXPECT_TRUE(dispatcherWhileFalse.Dispatch(i));
    EXPECT_EQ(i, 0);

    dispatcherWhileFalse.Subscribe(receiverFalse);
    EXPECT_TRUE(dispatcherWhileFalse.Dispatch(i));
    EXPECT_EQ(i, 0);
}

class EventDispatcherSubscription : public testing::Test
{
protected:
    void DispatchAndValidate()
    {
        dispatcherA.Dispatch(currentA);
        EXPECT_EQ(currentA, expectedA += incrementA);

        dispatcherB.Dispatch(currentB);
        EXPECT_EQ(currentB, expectedB += incrementB);
    }

    void SetUp() override
    {
        receiverAddOne.Bind([](int& i) { i += 1; });
        receiverAddTwo.Bind([](int& i) { i += 2; });
        receiverAddThree.Bind([](int& i) { i += 3; });
        receiverAddFour.Bind([](int& i) { i += 4; });

        ASSERT_TRUE(receiverAddOne.IsBound());
        ASSERT_TRUE(receiverAddTwo.IsBound());
        ASSERT_TRUE(receiverAddThree.IsBound());
        ASSERT_TRUE(receiverAddFour.IsBound());

        ASSERT_TRUE(dispatcherA.Subscribe(receiverAddOne));
        ASSERT_TRUE(dispatcherA.Subscribe(receiverAddTwo));
        ASSERT_TRUE(dispatcherA.Subscribe(receiverAddThree));
        ASSERT_TRUE(dispatcherA.Subscribe(receiverAddFour));
        incrementA = 10;

        DispatchAndValidate();
    }

    void TearDown() override
    {
        DispatchAndValidate();

        dispatcherA.UnsubscribeAll();
        incrementA = 0;

        dispatcherB.UnsubscribeAll();
        incrementB = 0;

        DispatchAndValidate();
    }

protected:
    int currentA = 0, expectedA = 0, incrementA = 0;
    int currentB = 0, expectedB = 0, incrementB = 0;

    Event::Receiver<void(int&)> receiverAddOne;
    Event::Receiver<void(int&)> receiverAddTwo;
    Event::Receiver<void(int&)> receiverAddThree;
    Event::Receiver<void(int&)> receiverAddFour;

    Event::Dispatcher<void(int&)> dispatcherA;
    Event::Dispatcher<void(int&)> dispatcherB;
};

TEST_F(EventDispatcherSubscription, RepeatedSubscription)
{
    for(int i = 0; i < 10; ++i)
    {
        EXPECT_TRUE(dispatcherA.Subscribe(receiverAddOne));
        EXPECT_TRUE(dispatcherA.Subscribe(receiverAddTwo));
        EXPECT_TRUE(dispatcherA.Subscribe(receiverAddThree));
        EXPECT_TRUE(dispatcherA.Subscribe(receiverAddFour));

        DispatchAndValidate();
    }
}

TEST_F(EventDispatcherSubscription, UnsubscribeNonsubscribed)
{
    for(int i = 0; i < 10; ++i)
    {
        EXPECT_FALSE(dispatcherB.Subscribe(receiverAddOne));
        EXPECT_FALSE(dispatcherB.Subscribe(receiverAddTwo));
        EXPECT_FALSE(dispatcherB.Subscribe(receiverAddThree));
        EXPECT_FALSE(dispatcherB.Subscribe(receiverAddFour));

        DispatchAndValidate();
    }
}

TEST_F(EventDispatcherSubscription, ReplaceSubscription)
{
    EXPECT_TRUE(dispatcherB.Subscribe(receiverAddOne,
        Event::SubscriptionPolicy::ReplaceSubscription));
    incrementA -= 1;
    incrementB += 1;

    DispatchAndValidate();

    EXPECT_TRUE(dispatcherB.Subscribe(receiverAddThree,
        Event::SubscriptionPolicy::ReplaceSubscription));
    incrementA -= 3;
    incrementB += 3;

    DispatchAndValidate();

    EXPECT_TRUE(dispatcherB.Subscribe(receiverAddFour,
        Event::SubscriptionPolicy::ReplaceSubscription));
    incrementA -= 4;
    incrementB += 4;

    DispatchAndValidate();

    EXPECT_TRUE(dispatcherB.Subscribe(receiverAddTwo,
        Event::SubscriptionPolicy::ReplaceSubscription));
    incrementA -= 2;
    incrementB += 2;

    DispatchAndValidate();

    EXPECT_FALSE(dispatcherA.Subscribe(receiverAddOne,
        Event::SubscriptionPolicy::RetainSubscription));
    EXPECT_FALSE(dispatcherA.Subscribe(receiverAddTwo,
        Event::SubscriptionPolicy::RetainSubscription));
    EXPECT_FALSE(dispatcherA.Subscribe(receiverAddThree,
        Event::SubscriptionPolicy::RetainSubscription));
    EXPECT_FALSE(dispatcherA.Subscribe(receiverAddFour,
        Event::SubscriptionPolicy::RetainSubscription));

    DispatchAndValidate();
}

class EventDispatcherSubscriptionChangeDuringDispatch : public testing::Test
{
protected:
    EventDispatcherSubscriptionChangeDuringDispatch()
        : dispatcher(std::make_unique<Event::CollectWhileTrue>())
    {
    }

    void SetUp() override
    {
        receiverFireOnce.Bind([this]()
        {
            receiverFireOnce.Unsubscribe();
            value += 1;
            return true;
        });

        receiverFireAlways.Bind([this]()
        {
            value += 10;
            return true;
        });

        receiverChainA.Bind([this]()
        {
            value += 100;
            return true;
        });

        receiverChainB.Bind([this]()
        {
            receiverChainA.Subscribe(dispatcher);
            value += 1000;
            return true;
        });

        receiverChainC.Bind([this]()
        {
            dispatcher.Subscribe(receiverChainB);
            dispatcher.Unsubscribe(receiverChainC);
            value += 10000;
            return true;
        });

        receiverChainD.Bind([this]()
        {
            dispatcher.Unsubscribe(receiverChainD);
            dispatcher.Subscribe(receiverChainC);
            value += 100000;
            return true;
        });
    }

    void TearDown() override
    {
        EXPECT_TRUE(dispatcher.Dispatch());
        EXPECT_EQ(value, firstDispatch);

        value = 0;
        EXPECT_TRUE(dispatcher.Dispatch());
        EXPECT_EQ(value, secondDispatch);

        dispatcher.UnsubscribeAll();

        value = 0;
        EXPECT_TRUE(dispatcher.Dispatch());
        EXPECT_EQ(value, 0);
    }

protected:
    int value = 0, firstDispatch = 0, secondDispatch = 0;

    Event::Receiver<bool(void)> receiverFireOnce;
    Event::Receiver<bool(void)> receiverFireAlways;
    Event::Receiver<bool(void)> receiverChainA;
    Event::Receiver<bool(void)> receiverChainB;
    Event::Receiver<bool(void)> receiverChainC;
    Event::Receiver<bool(void)> receiverChainD;

    Event::Dispatcher<bool(void)> dispatcher;
};

TEST_F(EventDispatcherSubscriptionChangeDuringDispatch, SubscribeFireOnceReceiver)
{
    EXPECT_TRUE(dispatcher.Subscribe(receiverFireOnce));

    firstDispatch += 1;
    secondDispatch += 0;
}

TEST_F(EventDispatcherSubscriptionChangeDuringDispatch, SubscribeFireAlwaysReceiver)
{
    EXPECT_TRUE(dispatcher.Subscribe(receiverFireAlways));

    firstDispatch += 10;
    secondDispatch += 10;
}

TEST_F(EventDispatcherSubscriptionChangeDuringDispatch, SubscribeReceiverChain)
{
    EXPECT_TRUE(dispatcher.Subscribe(receiverChainD));

    firstDispatch += 111100;
    secondDispatch += 1100;
}

class EventDispatcherLifetimes : public testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(receiver.Subscribe(dispatcher));
    }

protected:
    InstanceCounter<> counter;
    Event::Dispatcher<char(InstanceCounter<>, int)> dispatcher{ '\0' };
    Event::Receiver<char(InstanceCounter<>, int)> receiver;
};

TEST_F(EventDispatcherLifetimes, FunctionDispatch)
{
    receiver.Bind<&Function>();
    EXPECT_EQ(dispatcher.Dispatch(counter, 0), '0');
    EXPECT_EQ(counter.GetStats().copies, 1);
}

TEST_F(EventDispatcherLifetimes, MethodDispatch)
{
    BaseClass baseClass;
    receiver.Bind<BaseClass, &BaseClass::Method>(&baseClass);
    EXPECT_EQ(dispatcher.Dispatch(counter, 3), '3');
    EXPECT_EQ(counter.GetStats().copies, 1);
}

TEST_F(EventDispatcherLifetimes, LambdaDispatch)
{
    receiver.Bind([](InstanceCounter<>, int index) { return Text[index]; });
    EXPECT_EQ(dispatcher.Dispatch(counter, 5), '5');
    EXPECT_EQ(counter.GetStats().copies, 1);
}

TEST(EventDispatcher, InvokeUnboundReceivers)
{
    Event::Dispatcher<int(int&)> dispatcher(0);
    Event::Receiver<int(int&)> receiverUnboundFirst;
    Event::Receiver<int(int&)> receiverUnboundMiddle;
    Event::Receiver<int(int&)> receiverUnboundLast;

    Event::Receiver<int(int&)> receiverBoundFirst;
    receiverBoundFirst.Bind([](int& i) { i += 1; return 1; });

    Event::Receiver<int(int&)> receiverBoundSecond;
    receiverBoundSecond.Bind([](int& i) { i += 2; return 2; });

    EXPECT_TRUE(dispatcher.Subscribe(receiverUnboundFirst));
    EXPECT_TRUE(dispatcher.Subscribe(receiverBoundFirst));
    EXPECT_TRUE(dispatcher.Subscribe(receiverUnboundMiddle));
    EXPECT_TRUE(dispatcher.Subscribe(receiverBoundSecond));
    EXPECT_TRUE(dispatcher.Subscribe(receiverUnboundLast));

    int value = 0;
    EXPECT_EQ(dispatcher.Dispatch(value), 2);
    EXPECT_EQ(value, 3);
}

/*
    Event Broker
*/

struct EventBoolean : public Event::EventBase
{
    REFLECTION_ENABLE(EventBoolean, Event::EventBase)

public:
    EventBoolean(bool boolean) :
        boolean(boolean)
    {
    }

    bool boolean;
};

struct EventInteger : public Event::EventBase
{
    REFLECTION_ENABLE(EventInteger, Event::EventBase)

public:
    EventInteger(int integer) :
        integer(integer)
    {
    }

    int integer;
};

struct EventString : public Event::EventBase
{
    REFLECTION_ENABLE(EventString, Event::EventBase)

public:
    EventString(const std::string string) :
        string(string)
    {
    }

    std::string string;
};

struct EventVector : public Event::EventBase
{
    REFLECTION_ENABLE(EventVector, Event::EventBase)

public:
    EventVector(const std::vector<int> vector) :
        vector(vector)
    {
    }

    std::vector<int> vector;
};

REFLECTION_TYPE(EventBoolean, Event::EventBase)
REFLECTION_TYPE(EventInteger, Event::EventBase)
REFLECTION_TYPE(EventString, Event::EventBase)
REFLECTION_TYPE(EventVector, Event::EventBase)

class EventBroker : public testing::Test
{
protected:
    void SetUp() override
    {
        receiverBooleanVoid.Bind([this](const EventBoolean& event)
        {
            currentValue += event.boolean ? 10 : 100;
        });

        receiverIntegerTrue.Bind([this](const EventInteger& event)
        {
            currentValue += event.integer;
            return true;
        });

        receiverIntegerFalse.Bind([this](const EventInteger& event)
        {
            currentValue += event.integer;
            return false;
        });

        receiverStringTrue.Bind([this](const EventString& event)
        {
            currentValue += event.string.size();
            return true;
        });

        receiverStringFalse.Bind([this](const EventString& event)
        {
            currentValue += event.string.size();
            return false;
        });
    }

protected:
    int currentValue = 0;
    int expectedValue = 0;

    Event::Receiver<void(const EventBoolean&)> receiverBooleanVoid;
    Event::Receiver<bool(const EventInteger&)> receiverIntegerTrue;
    Event::Receiver<bool(const EventInteger&)> receiverIntegerFalse;
    Event::Receiver<bool(const EventString&)> receiverStringTrue;
    Event::Receiver<bool(const EventString&)> receiverStringFalse;

    Event::Broker broker;
};

TEST_F(EventBroker, DispatchEmpty)
{
    EXPECT_FALSE(broker.Dispatch<bool>(EventInteger{ 4 }));
    EXPECT_FALSE(broker.Dispatch<bool>(EventString{ "Null" }));
    EXPECT_EQ(currentValue, expectedValue);
}

TEST_F(EventBroker, DispatchUnregistered)
{
    EXPECT_FALSE(broker.Subscribe(receiverIntegerTrue));
    EXPECT_FALSE(broker.Subscribe(receiverStringFalse));

    EXPECT_FALSE(broker.Dispatch<bool>(EventInteger{ 2 }));
    EXPECT_EQ(currentValue, expectedValue);

    EXPECT_FALSE(broker.Dispatch<bool>(EventString{ "Jelly" }));
    EXPECT_EQ(currentValue, expectedValue);
}

TEST_F(EventBroker, DispatchRegistered)
{
    EXPECT_TRUE((broker.Register<void, EventBoolean>(std::make_unique<Event::CollectNothing>())));
    EXPECT_TRUE((broker.Register<bool, EventInteger>(std::make_unique<Event::CollectWhileTrue>())));
    EXPECT_TRUE((broker.Register<bool, EventString>(std::make_unique<Event::CollectWhileFalse>())));
    broker.Finalize();

    EXPECT_FALSE((broker.Register<bool, EventVector>()));

    EXPECT_TRUE(broker.Subscribe(receiverBooleanVoid));
    EXPECT_TRUE(broker.Subscribe(receiverIntegerTrue));
    EXPECT_TRUE(broker.Subscribe(receiverStringFalse));

    EXPECT_TRUE(broker.Dispatch<void>(EventBoolean{ true }));
    EXPECT_EQ(currentValue, expectedValue += 10);

    EXPECT_EQ(broker.Dispatch<bool>(EventInteger{ 2 }).Unwrap(), true);
    EXPECT_EQ(currentValue, expectedValue += 2);

    EXPECT_EQ(broker.Dispatch<bool>(EventString{ "Jelly" }).Unwrap(), false);
    EXPECT_EQ(currentValue, expectedValue += 5);

    EXPECT_FALSE(broker.Dispatch<bool>(EventBoolean{ true }));
    EXPECT_EQ(currentValue, expectedValue);

    EXPECT_FALSE(broker.Dispatch<int>(EventInteger{ 2 }));
    EXPECT_EQ(currentValue, expectedValue);

    EXPECT_FALSE(broker.Dispatch<float>(EventString{ "Jelly" }));
    EXPECT_EQ(currentValue, expectedValue);
}
