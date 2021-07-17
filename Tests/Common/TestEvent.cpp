/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <memory>
#include <functional>
#include <doctest/doctest.h>
#include <Common/Delegate.hpp>
#include <Common/Event/EventCollector.hpp>
#include <Common/Event/EventDispatcher.hpp>
#include <Common/Event/EventReceiver.hpp>
#include <Common/Event/EventBroker.hpp>
#include <Common/Test/InstanceCounter.hpp>

static const char* Text = "0123456789";

char Function(Test::InstanceCounter<> instance, int index)
{
    return Text[index];
}

class BaseClass
{
public:
    virtual char Method(Test::InstanceCounter<> instance, int index)
    {
        return Text[index];
    }
};

class DerivedClass : public BaseClass
{
public:
    char Method(Test::InstanceCounter<> instance, int index) override
    {
        return Text[index];
    }
};

TEST_CASE("Event Delegate")
{
    SUBCASE("Binding")
    {
        Test::InstanceCounter<> counter;
        Event::Delegate<char(Test::InstanceCounter<>, int)> delegate;
        REQUIRE(!delegate.IsBound());

        SUBCASE("Static function binding")
        {
            delegate.Bind<&Function>();
            CHECK(delegate.IsBound());
            CHECK_EQ(delegate.Invoke(counter, 4), '4');
        }

        SUBCASE("Class method binding")
        {
            BaseClass baseClass;
            delegate.Bind<BaseClass, &BaseClass::Method>(&baseClass);
            CHECK(delegate.IsBound());
            CHECK_EQ(delegate.Invoke(counter, 6), '6');
        }

        SUBCASE("Virtual method binding")
        {
            DerivedClass derivedClass;
            delegate.Bind<BaseClass, &BaseClass::Method>(&derivedClass);
            CHECK(delegate.IsBound());
            CHECK_EQ(delegate.Invoke(counter, 1), '1');
        }

        SUBCASE("Lambda function binding")
        {
            auto functor = [](Test::InstanceCounter<> counter, int index) -> char
            {
                return Text[index];
            };

            delegate.Bind(&functor);
            CHECK(delegate.IsBound());
            CHECK_EQ(delegate.Invoke(counter, 9), '9');
        }

        SUBCASE("Lambda capture binding via constructor")
        {
            int modifier = 4;
            delegate = Event::Delegate<char(Test::InstanceCounter<>, int)>(
                [&modifier](Test::InstanceCounter<> counter, int index) -> char
                {
                    return Text[index + modifier];
                });

            CHECK(delegate.IsBound());
            CHECK_EQ(delegate.Invoke(counter, 3), '7');
        }

        SUBCASE("Lambda with parameter binding")
        {
            auto functor = [](Test::InstanceCounter<> counter, int index, int modifier) -> char
            {
                return Text[index + modifier];
            };

            delegate = std::bind(functor, std::placeholders::_1, std::placeholders::_2, 4 );

            CHECK(delegate.IsBound());
            CHECK_EQ(delegate.Invoke(counter, 3), '7');
        }

        delegate.Bind(nullptr);
        CHECK(!delegate.IsBound());
        CHECK_EQ(counter.GetStats().copies, 1);
    }

    SUBCASE("Similar lambda signatures")
    {
        int i = 0, y = 0;

        Event::Delegate<void()> delegateOne([&i]() { i = 3; });
        Event::Delegate<void()> delegateTwo([&y]() { y = 7; });

        delegateOne.Invoke();
        delegateTwo.Invoke();

        CHECK_EQ(i, 3);
        CHECK_EQ(y, 7);
    }

    SUBCASE("Lambda capture lifetime")
    {
        int currentValue = 0;
        int expectedValue = 0;
        int expectedCopies = 0;
        int expectedInstances = 0;

        Test::InstanceCounter<> counter;
        CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);

        Event::Delegate<void()> delegate;

        {
            SUBCASE("Bind lvalue lambda")
            {
                {
                    auto lambda = [&currentValue, counter]()
                    {
                        currentValue += 1;
                    };

                    CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);
                    CHECK_EQ(counter.GetStats().copies, expectedCopies += 1);

                    delegate = lambda;
                    expectedValue += 1;

                    CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);
                    CHECK_EQ(counter.GetStats().copies, expectedCopies += 1);
                }
                
                CHECK_EQ(counter.GetStats().instances, expectedInstances -= 1);
                CHECK_EQ(counter.GetStats().copies, expectedCopies);
            }
            
            SUBCASE("Bind rvalue lambda")
            {
                delegate.Bind([&currentValue, counter]()
                {
                    currentValue += 10;
                });

                expectedValue += 10;

                CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);
                CHECK_EQ(counter.GetStats().copies, expectedCopies += 1);
            }

            SUBCASE("Copy delegate")
            {
                Event::Delegate<void()> delegateCopy([&currentValue, counter]()
                {
                    currentValue += 100;
                });

                expectedValue += 100;

                CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);
                CHECK_EQ(counter.GetStats().copies, expectedCopies += 1);

                delegate = delegateCopy;

                CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);
                CHECK_EQ(counter.GetStats().copies, expectedCopies += 1);

                delegateCopy = nullptr;

                CHECK_EQ(counter.GetStats().instances, expectedInstances -= 1);
                CHECK_EQ(counter.GetStats().copies, expectedCopies);
            }

            SUBCASE("Move delegate")
            {
                Event::Delegate<void()> delegateMove([&currentValue, counter]()
                {
                    currentValue += 1000;
                });

                expectedValue += 1000;

                CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);
                CHECK_EQ(counter.GetStats().copies, expectedCopies += 1);

                delegate = std::move(delegateMove);

                CHECK_EQ(counter.GetStats().instances, expectedInstances);
                CHECK_EQ(counter.GetStats().copies, expectedCopies);

                delegateMove = nullptr;

                CHECK_EQ(counter.GetStats().instances, expectedInstances);
                CHECK_EQ(counter.GetStats().copies, expectedCopies);
            }

            CHECK_EQ(counter.GetStats().instances, expectedInstances);
            CHECK_EQ(counter.GetStats().copies, expectedCopies);

            delegate.Invoke();
            CHECK_EQ(currentValue, expectedValue);

            CHECK_EQ(counter.GetStats().instances, expectedInstances);
            CHECK_EQ(counter.GetStats().copies, expectedCopies);

            delegate = nullptr;

            CHECK_EQ(counter.GetStats().instances, expectedInstances -= 1);
            CHECK_EQ(counter.GetStats().copies, expectedCopies);
        }

        CHECK_EQ(counter.GetStats().instances, expectedInstances);
        CHECK_EQ(counter.GetStats().copies, expectedCopies);
    }
}

TEST_CASE("Event Collector")
{
    SUBCASE("Collect nothing")
    {
        Event::CollectNothing collectNothing;
        CHECK(collectNothing.ShouldContinue());
    }

    SUBCASE("Collect last result")
    {
        Event::CollectLast<int> collectLast(0);

        for(int i = 0; i < 10; ++i)
        {
            CHECK_EQ(collectLast.GetResult(), i);
            collectLast.ConsumeResult(i + 1);
            CHECK(collectLast.ShouldContinue());
            CHECK_EQ(collectLast.GetResult(), i + 1);
        }
    }

    SUBCASE("Collect while true")
    {
        Event::CollectWhileTrue collectWhileTrue(true);
        CHECK(collectWhileTrue.GetResult());

        collectWhileTrue.ConsumeResult(true);
        CHECK(collectWhileTrue.ShouldContinue());
        CHECK(collectWhileTrue.GetResult());

        collectWhileTrue.ConsumeResult(false);
        CHECK_FALSE(collectWhileTrue.ShouldContinue());
        CHECK_FALSE(collectWhileTrue.GetResult());

        SUBCASE("Collect after reset")
        {
            collectWhileTrue.Reset();

            collectWhileTrue.ConsumeResult(true);
            CHECK(collectWhileTrue.ShouldContinue());
            CHECK(collectWhileTrue.GetResult());

            collectWhileTrue.ConsumeResult(false);
            CHECK_FALSE(collectWhileTrue.ShouldContinue());
            CHECK_FALSE(collectWhileTrue.GetResult());
        }
    }

    SUBCASE("Collect while false")
    {
        Event::CollectWhileFalse collectWhileFalse(false);
        CHECK_FALSE(collectWhileFalse.GetResult());

        collectWhileFalse.ConsumeResult(false);
        CHECK(collectWhileFalse.ShouldContinue());
        CHECK_FALSE(collectWhileFalse.GetResult());

        collectWhileFalse.ConsumeResult(true);
        CHECK_FALSE(collectWhileFalse.ShouldContinue());
        CHECK(collectWhileFalse.GetResult());

        SUBCASE("Collect after reset")
        {
            collectWhileFalse.Reset();

            collectWhileFalse.ConsumeResult(false);
            CHECK(collectWhileFalse.ShouldContinue());
            CHECK_FALSE(collectWhileFalse.GetResult());

            collectWhileFalse.ConsumeResult(true);
            CHECK_FALSE(collectWhileFalse.ShouldContinue());
            CHECK(collectWhileFalse.GetResult());
        }
    }
}

TEST_CASE("Event Dispatcher")
{
    SUBCASE("Collecting last result")
    {
        int i = 0;

        Event::Receiver<int(int&)> receiverAddOne;
        receiverAddOne.Bind([](int& i) { i += 1; return i; });

        Event::Receiver<int(int&)> receiverAddTwo;
        receiverAddTwo.Bind([](int& i) { i += 2; return i; });

        Event::Dispatcher<int(int&)> dispatcher(0);
        CHECK_EQ(dispatcher.Dispatch(i), 0);

        CHECK(dispatcher.Subscribe(receiverAddOne));
        CHECK(dispatcher.Subscribe(receiverAddOne));
        CHECK_EQ(dispatcher.Dispatch(i), 1);

        CHECK(dispatcher.Subscribe(receiverAddTwo));
        CHECK(dispatcher.Subscribe(receiverAddTwo));
        CHECK_EQ(dispatcher.Dispatch(i), 4);

        CHECK(receiverAddOne.Unsubscribe());
        CHECK_FALSE(receiverAddOne.Unsubscribe());
        CHECK_EQ(dispatcher.Dispatch(i), 6);

        CHECK(dispatcher.Unsubscribe(receiverAddTwo));
        CHECK_FALSE(dispatcher.Unsubscribe(receiverAddTwo));
        CHECK_EQ(dispatcher.Dispatch(i), 0);
    }

    SUBCASE("Collecting boolean result")
    {
        Event::Receiver<bool(int&)> receiverTrue;
        receiverTrue.Bind([](int& i) { i += 1; return true; });

        Event::Receiver<bool(int&)> receiverFalse;
        receiverFalse.Bind([](int& i) { i += 2; return false; });

        Event::Receiver<bool(int&)> receiverDummy;
        receiverDummy.Bind([](int& i) { i += 9999; return true; });
        
        SUBCASE("While returning true")
        {
            int i = 0;

            auto collector = std::make_unique<Event::CollectWhileTrue>(true);
            Event::Dispatcher<bool(int&)> dispatcherWhileTrue(std::move(collector));
            CHECK(dispatcherWhileTrue.Dispatch(i));
            CHECK_EQ(i, 0);

            CHECK(dispatcherWhileTrue.Subscribe(receiverTrue));
            CHECK(dispatcherWhileTrue.Subscribe(receiverTrue));
            CHECK(dispatcherWhileTrue.Dispatch(i));
            CHECK_EQ(i, 1);

            CHECK(dispatcherWhileTrue.Subscribe(receiverFalse));
            CHECK(dispatcherWhileTrue.Subscribe(receiverFalse));
            CHECK_FALSE(dispatcherWhileTrue.Dispatch(i));
            CHECK_EQ(i, 4);

            CHECK(dispatcherWhileTrue.Subscribe(receiverDummy));
            CHECK(dispatcherWhileTrue.Subscribe(receiverDummy));
            CHECK_FALSE(dispatcherWhileTrue.Dispatch(i));
            CHECK_EQ(i, 7);
        }

        SUBCASE("While returning false")
        {
            int i = 0;

            auto collector = std::make_unique<Event::CollectWhileFalse>(false);
            Event::Dispatcher<bool(int&)> dispatcherWhileFalse(std::move(collector));
            CHECK_FALSE(dispatcherWhileFalse.Dispatch(i));
            CHECK_EQ(i, 0);

            CHECK(dispatcherWhileFalse.Subscribe(receiverFalse));
            CHECK(dispatcherWhileFalse.Subscribe(receiverFalse));
            CHECK_FALSE(dispatcherWhileFalse.Dispatch(i));
            CHECK_EQ(i, 2);

            CHECK(dispatcherWhileFalse.Subscribe(receiverTrue));
            CHECK(dispatcherWhileFalse.Subscribe(receiverTrue));
            CHECK(dispatcherWhileFalse.Dispatch(i));
            CHECK_EQ(i, 5);

            CHECK(dispatcherWhileFalse.Subscribe(receiverDummy));
            CHECK(dispatcherWhileFalse.Subscribe(receiverDummy));
            CHECK(dispatcherWhileFalse.Dispatch(i));
            CHECK_EQ(i, 8);
        }

        SUBCASE("Stopped by initial false")
        {
            int i = 0;

            auto collector = std::make_unique<Event::CollectWhileTrue>(false);
            Event::Dispatcher<bool(int&)> dispatcherWhileTrue(std::move(collector));
            CHECK_FALSE(dispatcherWhileTrue.Dispatch(i));
            CHECK_EQ(i, 0);

            dispatcherWhileTrue.Subscribe(receiverTrue);
            CHECK_FALSE(dispatcherWhileTrue.Dispatch(i));
            CHECK_EQ(i, 0);
        }

        SUBCASE("Stopped by initial true")
        {
            int i = 0;

            auto collector = std::make_unique<Event::CollectWhileFalse>(true);
            Event::Dispatcher<bool(int&)> dispatcherWhileFalse(std::move(collector));
            CHECK(dispatcherWhileFalse.Dispatch(i));
            CHECK_EQ(i, 0);

            dispatcherWhileFalse.Subscribe(receiverFalse);
            CHECK(dispatcherWhileFalse.Dispatch(i));
            CHECK_EQ(i, 0);
        }

        CHECK_FALSE(receiverTrue.IsSubscribed());
        CHECK_FALSE(receiverFalse.IsSubscribed());
        CHECK_FALSE(receiverDummy.IsSubscribed());
    }

    SUBCASE("Receiver subscription")
    {
        int currentA = 0, expectedA = 0, incrementA = 0;
        int currentB = 0, expectedB = 0, incrementB = 0;

        Event::Receiver<void(int&)> receiverAddOne;
        receiverAddOne.Bind([](int& i) { i += 1; });

        Event::Receiver<void(int&)> receiverAddTwo;
        receiverAddTwo.Bind([](int& i) { i += 2; });

        Event::Receiver<void(int&)> receiverAddThree;
        receiverAddThree.Bind([](int& i) { i += 3; });

        Event::Receiver<void(int&)> receiverAddFour;
        receiverAddFour.Bind([](int& i) { i += 4; });

        Event::Dispatcher<void(int&)> dispatcherA;
        Event::Dispatcher<void(int&)> dispatcherB;

        auto dispatchAndCheck = [&]()
        {
            dispatcherA.Dispatch(currentA);
            CHECK_EQ(currentA, expectedA += incrementA);

            dispatcherB.Dispatch(currentB);
            CHECK_EQ(currentB, expectedB += incrementB);
        };

        SUBCASE("Subcribe receivers")
        {
            CHECK(dispatcherA.Subscribe(receiverAddOne));
            CHECK(dispatcherA.Subscribe(receiverAddTwo));
            CHECK(dispatcherA.Subscribe(receiverAddThree));
            CHECK(dispatcherA.Subscribe(receiverAddFour));
            incrementA = 10;

            dispatchAndCheck();

            SUBCASE("Repeat subscription")
            {
                CHECK(dispatcherA.Subscribe(receiverAddOne));
                CHECK(dispatcherA.Subscribe(receiverAddTwo));
                CHECK(dispatcherA.Subscribe(receiverAddThree));
                CHECK(dispatcherA.Subscribe(receiverAddFour));

                dispatchAndCheck();
            }

            SUBCASE("Unsubcribe nonsubscribed")
            {
                CHECK_FALSE(dispatcherB.Subscribe(receiverAddOne));
                CHECK_FALSE(dispatcherB.Subscribe(receiverAddTwo));
                CHECK_FALSE(dispatcherB.Subscribe(receiverAddThree));
                CHECK_FALSE(dispatcherB.Subscribe(receiverAddFour));

                dispatchAndCheck();
            }

            SUBCASE("Replace subscription")
            {
                CHECK(dispatcherB.Subscribe(receiverAddOne, Event::SubscriptionPolicy::ReplaceSubscription));
                incrementA -= 1;
                incrementB += 1;

                dispatchAndCheck();

                CHECK(dispatcherB.Subscribe(receiverAddThree, Event::SubscriptionPolicy::ReplaceSubscription));
                incrementA -= 3;
                incrementB += 3;

                dispatchAndCheck();

                CHECK(dispatcherB.Subscribe(receiverAddFour, Event::SubscriptionPolicy::ReplaceSubscription));
                incrementA -= 4;
                incrementB += 4;

                dispatchAndCheck();

                CHECK(dispatcherB.Subscribe(receiverAddTwo, Event::SubscriptionPolicy::ReplaceSubscription));
                incrementA -= 2;
                incrementB += 2;

                dispatchAndCheck();

                CHECK_FALSE(dispatcherA.Subscribe(receiverAddOne, Event::SubscriptionPolicy::RetainSubscription));
                CHECK_FALSE(dispatcherA.Subscribe(receiverAddTwo, Event::SubscriptionPolicy::RetainSubscription));
                CHECK_FALSE(dispatcherA.Subscribe(receiverAddThree, Event::SubscriptionPolicy::RetainSubscription));
                CHECK_FALSE(dispatcherA.Subscribe(receiverAddFour, Event::SubscriptionPolicy::RetainSubscription));

                dispatchAndCheck();
            }
        }

        dispatchAndCheck();

        dispatcherA.UnsubscribeAll();
        incrementA = 0;

        dispatcherB.UnsubscribeAll();
        incrementB = 0;

        dispatchAndCheck();
    }

    SUBCASE("Subscription change during dispatch")
    {
        int value = 0, firstDispatch = 0, secondDispatch = 0;

        auto collector = std::make_unique<Event::CollectWhileTrue>();
        Event::Dispatcher<bool(void)> dispatcher(std::move(collector));

        Event::Receiver<bool(void)> receiverFireOnce;
        receiverFireOnce.Bind([&self = receiverFireOnce, &value]()
        {
            self.Unsubscribe();
            value += 1;
            return true;
        });

        Event::Receiver<bool(void)> receiverFireAlways;
        receiverFireAlways.Bind([&value]()
        {
            value += 10;
            return true;
        });

        Event::Receiver<bool(void)> receiverChainA;
        receiverChainA.Bind([&value]()
        {
            value += 100;
            return true;
        });

        Event::Receiver<bool(void)> receiverChainB;
        receiverChainB.Bind([&dispatcher, &receiverChainA, &value]()
        {
            receiverChainA.Subscribe(dispatcher);
            value += 1000;
            return true;
        });

        Event::Receiver<bool(void)> receiverChainC;
        receiverChainC.Bind([&dispatcher, &receiverChainB, &receiverChainC, &value]()
        {
            dispatcher.Subscribe(receiverChainB);
            dispatcher.Unsubscribe(receiverChainC);
            value += 10000;
            return true;
        });

        Event::Receiver<bool(void)> receiverChainD;
        receiverChainD.Bind([&dispatcher, &receiverChainC, &receiverChainD, &value]()
        {
            dispatcher.Unsubscribe(receiverChainD);
            dispatcher.Subscribe(receiverChainC);
            value += 100000;
            return true;
        });

        SUBCASE("Subscribe fire once receiver")
        {
            CHECK(dispatcher.Subscribe(receiverFireOnce));
            firstDispatch += 1;
            secondDispatch += 0;
        }

        SUBCASE("Subscribe fire always receiver")
        {
            CHECK(dispatcher.Subscribe(receiverFireAlways));
            firstDispatch += 10;
            secondDispatch += 10;
        }

        SUBCASE("Susbcribe receiver chain")
        {
            CHECK(dispatcher.Subscribe(receiverChainD));
            firstDispatch += 111100;
            secondDispatch += 1100;
        }

        CHECK(dispatcher.Dispatch());
        CHECK_EQ(value, firstDispatch);

        value = 0;
        CHECK(dispatcher.Dispatch());
        CHECK_EQ(value, secondDispatch);

        dispatcher.UnsubscribeAll();

        value = 0;
        CHECK(dispatcher.Dispatch());
        CHECK_EQ(value, 0);
    }

    SUBCASE("Invoking unbound receivers")
    {
        Event::Dispatcher<int(int&)> dispatcher(0);
        Event::Receiver<int(int&)> receiverUnboundFirst;
        Event::Receiver<int(int&)> receiverUnboundMiddle;
        Event::Receiver<int(int&)> receiverUnboundLast;

        Event::Receiver<int(int&)> receiverBoundFirst;
        receiverBoundFirst.Bind([](int& i) { i += 1; return 1; });

        Event::Receiver<int(int&)> receiverBoundSecond;
        receiverBoundSecond.Bind([](int& i) { i += 2; return 2; });

        CHECK(dispatcher.Subscribe(receiverUnboundFirst));
        CHECK(dispatcher.Subscribe(receiverBoundFirst));
        CHECK(dispatcher.Subscribe(receiverUnboundMiddle));
        CHECK(dispatcher.Subscribe(receiverBoundSecond));
        CHECK(dispatcher.Subscribe(receiverUnboundLast));

        int value = 0;
        CHECK_EQ(dispatcher.Dispatch(value), 2);
        CHECK_EQ(value, 3);
    }

    SUBCASE("Dispatch copy count")
    {
        Test::InstanceCounter<> counter;
        Event::Dispatcher<char(Test::InstanceCounter<>, int)> dispatcher('\0');
        Event::Receiver<char(Test::InstanceCounter<>, int)> receiver;
        CHECK(receiver.Subscribe(dispatcher));

        SUBCASE("Function dispatch")
        {
            receiver.Bind<&Function>();
            CHECK_EQ(dispatcher.Dispatch(counter, 0), '0');
            CHECK_EQ(counter.GetStats().copies, 1);
        }

        SUBCASE("Method dispatch")
        {
            BaseClass baseClass;
            receiver.Bind<BaseClass, &BaseClass::Method>(&baseClass);
            CHECK_EQ(dispatcher.Dispatch(counter, 3), '3');
            CHECK_EQ(counter.GetStats().copies, 1);
        }

        SUBCASE("Lambda dispatch")
        {
            receiver.Bind([](Test::InstanceCounter<>, int index) { return Text[index]; });
            CHECK_EQ(dispatcher.Dispatch(counter, 5), '5');
            CHECK_EQ(counter.GetStats().copies, 1);
        }
    }
}

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

TEST_CASE("Event Broker")
{
    int currentValue = 0;
    int expectedValue = 0;

    Event::Receiver<void(const EventBoolean&)> receiverBooleanVoid;
    receiverBooleanVoid.Bind([&currentValue](const EventBoolean& event)
    {
        currentValue += event.boolean ? 10 : 100;
    });

    Event::Receiver<bool(const EventInteger&)> receiverIntegerTrue;
    receiverIntegerTrue.Bind([&currentValue](const EventInteger& event)
    {
        currentValue += event.integer;
        return true;
    });

    Event::Receiver<bool(const EventInteger&)> receiverIntegerFalse;
    receiverIntegerFalse.Bind([&currentValue](const EventInteger& event)
    {
        currentValue += event.integer;
        return false;
    });

    Event::Receiver<bool(const EventString&)> receiverStringTrue;
    receiverStringTrue.Bind([&currentValue](const EventString& event)
    {
        currentValue += event.string.size();
        return true;
    });

    Event::Receiver<bool(const EventString&)> receiverStringFalse;
    receiverStringFalse.Bind([&currentValue](const EventString& event)
    {
        currentValue += event.string.size();
        return false;
    });

    Event::Broker broker;

    SUBCASE("Dispatch empty")
    {
        CHECK(broker.Dispatch<bool>(EventInteger{ 4 }).IsFailure());
        CHECK(broker.Dispatch<bool>(EventString{ "Null" }).IsFailure());
        CHECK_EQ(currentValue, expectedValue);
    }

    SUBCASE("Dispatch unregistered")
    {
        CHECK(broker.Subscribe(receiverIntegerTrue).IsFailure());
        CHECK(broker.Subscribe(receiverStringFalse).IsFailure());

        CHECK(broker.Dispatch<bool>(EventInteger{ 2 }).IsFailure());
        CHECK_EQ(currentValue, expectedValue);

        CHECK(broker.Dispatch<bool>(EventString{ "Jelly" }).IsFailure());
        CHECK_EQ(currentValue, expectedValue);
    }

    SUBCASE("Dispatch registered")
    {
        CHECK(broker.Register<void, EventBoolean>(std::make_unique<Event::CollectNothing>()));
        CHECK(broker.Register<bool, EventInteger>(std::make_unique<Event::CollectWhileTrue>()));
        CHECK(broker.Register<bool, EventString>(std::make_unique<Event::CollectWhileFalse>()));
        broker.Finalize();

        SUBCASE("Register when finalized")
        {
            CHECK_FALSE(broker.Register<bool, EventVector>());
        }

        CHECK(broker.Subscribe(receiverBooleanVoid).IsSuccess());
        CHECK(broker.Subscribe(receiverIntegerTrue).IsSuccess());
        CHECK(broker.Subscribe(receiverStringFalse).IsSuccess());

        CHECK(broker.Dispatch<void>(EventBoolean{ true }).IsSuccess());
        CHECK_EQ(currentValue, expectedValue += 10);

        CHECK_EQ(broker.Dispatch<bool>(EventInteger{ 2 }).Unwrap(), true);
        CHECK_EQ(currentValue, expectedValue += 2);

        CHECK_EQ(broker.Dispatch<bool>(EventString{ "Jelly" }).Unwrap(), false);
        CHECK_EQ(currentValue, expectedValue += 5);

        SUBCASE("Dispatch with mismatched result type")
        {
            CHECK(broker.Dispatch<bool>(EventBoolean{ true }).IsFailure());
            CHECK_EQ(currentValue, expectedValue);

            CHECK(broker.Dispatch<int>(EventInteger{ 2 }).IsFailure());
            CHECK_EQ(currentValue, expectedValue);

            CHECK(broker.Dispatch<float>(EventString{ "Jelly" }).IsFailure());
            CHECK_EQ(currentValue, expectedValue);
        }
    }
}
