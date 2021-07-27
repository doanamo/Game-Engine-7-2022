/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#define DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
#include <doctest/doctest.h>

#include <memory>
#include <functional>
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

DOCTEST_TEST_CASE("Event Delegate")
{
    DOCTEST_SUBCASE("Binding")
    {
        Test::InstanceCounter<> counter;
        Event::Delegate<char(Test::InstanceCounter<>, int)> delegate;
        DOCTEST_REQUIRE(!delegate.IsBound());

        DOCTEST_SUBCASE("Static function binding")
        {
            delegate.Bind<&Function>();
            DOCTEST_CHECK(delegate.IsBound());
            DOCTEST_CHECK_EQ(delegate.Invoke(counter, 4), '4');
        }

        DOCTEST_SUBCASE("Class method binding")
        {
            BaseClass baseClass;
            delegate.Bind<BaseClass, &BaseClass::Method>(&baseClass);
            DOCTEST_CHECK(delegate.IsBound());
            DOCTEST_CHECK_EQ(delegate.Invoke(counter, 6), '6');
        }

        DOCTEST_SUBCASE("Virtual method binding")
        {
            DerivedClass derivedClass;
            delegate.Bind<BaseClass, &BaseClass::Method>(&derivedClass);
            DOCTEST_CHECK(delegate.IsBound());
            DOCTEST_CHECK_EQ(delegate.Invoke(counter, 1), '1');
        }

        DOCTEST_SUBCASE("Lambda function binding")
        {
            auto functor = [](Test::InstanceCounter<> counter, int index) -> char
            {
                return Text[index];
            };

            delegate.Bind(&functor);
            DOCTEST_CHECK(delegate.IsBound());
            DOCTEST_CHECK_EQ(delegate.Invoke(counter, 9), '9');
        }

        DOCTEST_SUBCASE("Lambda DOCTEST_CAPTURE binding via constructor")
        {
            int modifier = 4;
            delegate = Event::Delegate<char(Test::InstanceCounter<>, int)>(
                [&modifier](Test::InstanceCounter<> counter, int index) -> char
                {
                    return Text[index + modifier];
                });

            DOCTEST_CHECK(delegate.IsBound());
            DOCTEST_CHECK_EQ(delegate.Invoke(counter, 3), '7');
        }

        DOCTEST_SUBCASE("Lambda with parameter binding")
        {
            auto functor = [](Test::InstanceCounter<> counter, int index, int modifier) -> char
            {
                return Text[index + modifier];
            };

            delegate = std::bind(functor, std::placeholders::_1, std::placeholders::_2, 4 );

            DOCTEST_CHECK(delegate.IsBound());
            DOCTEST_CHECK_EQ(delegate.Invoke(counter, 3), '7');
        }

        delegate.Bind(nullptr);
        DOCTEST_CHECK(!delegate.IsBound());
        DOCTEST_CHECK_EQ(counter.GetStats().copies, 1);
    }

    DOCTEST_SUBCASE("Similar lambda signatures")
    {
        int i = 0, y = 0;

        Event::Delegate<void()> delegateOne([&i]() { i = 3; });
        Event::Delegate<void()> delegateTwo([&y]() { y = 7; });

        delegateOne.Invoke();
        delegateTwo.Invoke();

        DOCTEST_CHECK_EQ(i, 3);
        DOCTEST_CHECK_EQ(y, 7);
    }

    DOCTEST_SUBCASE("Lambda capture lifetime")
    {
        int currentValue = 0;
        int expectedValue = 0;
        int expectedCopies = 0;
        int expectedInstances = 0;

        Test::InstanceCounter<> counter;
        DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);

        Event::Delegate<void()> delegate;

        {
            DOCTEST_SUBCASE("Bind lvalue lambda")
            {
                {
                    auto lambda = [&currentValue, counter]()
                    {
                        currentValue += 1;
                    };

                    DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);
                    DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies += 1);

                    delegate = lambda;
                    expectedValue += 1;

                    DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);
                    DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies += 1);
                }
                
                DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances -= 1);
                DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies);
            }
            
            DOCTEST_SUBCASE("Bind rvalue lambda")
            {
                delegate.Bind([&currentValue, counter]()
                {
                    currentValue += 10;
                });

                expectedValue += 10;

                DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);
                DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies += 1);
            }

            DOCTEST_SUBCASE("Copy delegate")
            {
                Event::Delegate<void()> delegateCopy([&currentValue, counter]()
                {
                    currentValue += 100;
                });

                expectedValue += 100;

                DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);
                DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies += 1);

                delegate = delegateCopy;

                DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);
                DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies += 1);

                delegateCopy = nullptr;

                DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances -= 1);
                DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies);
            }

            DOCTEST_SUBCASE("Move delegate")
            {
                Event::Delegate<void()> delegateMove([&currentValue, counter]()
                {
                    currentValue += 1000;
                });

                expectedValue += 1000;

                DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances += 1);
                DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies += 1);

                delegate = std::move(delegateMove);

                DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances);
                DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies);

                delegateMove = nullptr;

                DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances);
                DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies);
            }

            DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances);
            DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies);

            delegate.Invoke();
            DOCTEST_CHECK_EQ(currentValue, expectedValue);

            DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances);
            DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies);

            delegate = nullptr;

            DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances -= 1);
            DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies);
        }

        DOCTEST_CHECK_EQ(counter.GetStats().instances, expectedInstances);
        DOCTEST_CHECK_EQ(counter.GetStats().copies, expectedCopies);
    }
}

DOCTEST_TEST_CASE("Event Collector")
{
    DOCTEST_SUBCASE("Collect nothing")
    {
        Event::CollectNothing collectNothing;
        DOCTEST_CHECK(collectNothing.ShouldContinue());
    }

    DOCTEST_SUBCASE("Collect last result")
    {
        Event::CollectLast<int> collectLast(0);

        for(int i = 0; i < 10; ++i)
        {
            DOCTEST_CHECK_EQ(collectLast.GetResult(), i);
            collectLast.ConsumeResult(i + 1);
            DOCTEST_CHECK(collectLast.ShouldContinue());
            DOCTEST_CHECK_EQ(collectLast.GetResult(), i + 1);
        }
    }

    DOCTEST_SUBCASE("Collect while true")
    {
        Event::CollectWhileTrue collectWhileTrue(true);
        DOCTEST_CHECK(collectWhileTrue.GetResult());

        collectWhileTrue.ConsumeResult(true);
        DOCTEST_CHECK(collectWhileTrue.ShouldContinue());
        DOCTEST_CHECK(collectWhileTrue.GetResult());

        collectWhileTrue.ConsumeResult(false);
        DOCTEST_CHECK_FALSE(collectWhileTrue.ShouldContinue());
        DOCTEST_CHECK_FALSE(collectWhileTrue.GetResult());

        DOCTEST_SUBCASE("Collect after reset")
        {
            collectWhileTrue.Reset();

            collectWhileTrue.ConsumeResult(true);
            DOCTEST_CHECK(collectWhileTrue.ShouldContinue());
            DOCTEST_CHECK(collectWhileTrue.GetResult());

            collectWhileTrue.ConsumeResult(false);
            DOCTEST_CHECK_FALSE(collectWhileTrue.ShouldContinue());
            DOCTEST_CHECK_FALSE(collectWhileTrue.GetResult());
        }
    }

    DOCTEST_SUBCASE("Collect while false")
    {
        Event::CollectWhileFalse collectWhileFalse(false);
        DOCTEST_CHECK_FALSE(collectWhileFalse.GetResult());

        collectWhileFalse.ConsumeResult(false);
        DOCTEST_CHECK(collectWhileFalse.ShouldContinue());
        DOCTEST_CHECK_FALSE(collectWhileFalse.GetResult());

        collectWhileFalse.ConsumeResult(true);
        DOCTEST_CHECK_FALSE(collectWhileFalse.ShouldContinue());
        DOCTEST_CHECK(collectWhileFalse.GetResult());

        DOCTEST_SUBCASE("Collect after reset")
        {
            collectWhileFalse.Reset();

            collectWhileFalse.ConsumeResult(false);
            DOCTEST_CHECK(collectWhileFalse.ShouldContinue());
            DOCTEST_CHECK_FALSE(collectWhileFalse.GetResult());

            collectWhileFalse.ConsumeResult(true);
            DOCTEST_CHECK_FALSE(collectWhileFalse.ShouldContinue());
            DOCTEST_CHECK(collectWhileFalse.GetResult());
        }
    }
}

DOCTEST_TEST_CASE("Event Dispatcher")
{
    DOCTEST_SUBCASE("Collecting last result")
    {
        int i = 0;

        Event::Receiver<int(int&)> receiverAddOne;
        receiverAddOne.Bind([](int& i) { i += 1; return i; });

        Event::Receiver<int(int&)> receiverAddTwo;
        receiverAddTwo.Bind([](int& i) { i += 2; return i; });

        Event::Dispatcher<int(int&)> dispatcher(0);
        DOCTEST_CHECK_EQ(dispatcher.Dispatch(i), 0);

        DOCTEST_CHECK(dispatcher.Subscribe(receiverAddOne));
        DOCTEST_CHECK(dispatcher.Subscribe(receiverAddOne));
        DOCTEST_CHECK_EQ(dispatcher.Dispatch(i), 1);

        DOCTEST_CHECK(dispatcher.Subscribe(receiverAddTwo));
        DOCTEST_CHECK(dispatcher.Subscribe(receiverAddTwo));
        DOCTEST_CHECK_EQ(dispatcher.Dispatch(i), 4);

        DOCTEST_CHECK(receiverAddOne.Unsubscribe());
        DOCTEST_CHECK_FALSE(receiverAddOne.Unsubscribe());
        DOCTEST_CHECK_EQ(dispatcher.Dispatch(i), 6);

        DOCTEST_CHECK(dispatcher.Unsubscribe(receiverAddTwo));
        DOCTEST_CHECK_FALSE(dispatcher.Unsubscribe(receiverAddTwo));
        DOCTEST_CHECK_EQ(dispatcher.Dispatch(i), 0);
    }

    DOCTEST_SUBCASE("Collecting boolean result")
    {
        Event::Receiver<bool(int&)> receiverTrue;
        receiverTrue.Bind([](int& i) { i += 1; return true; });

        Event::Receiver<bool(int&)> receiverFalse;
        receiverFalse.Bind([](int& i) { i += 2; return false; });

        Event::Receiver<bool(int&)> receiverDummy;
        receiverDummy.Bind([](int& i) { i += 9999; return true; });
        
        DOCTEST_SUBCASE("While returning true")
        {
            int i = 0;

            auto collector = std::make_unique<Event::CollectWhileTrue>(true);
            Event::Dispatcher<bool(int&)> dispatcherWhileTrue(std::move(collector));
            DOCTEST_CHECK(dispatcherWhileTrue.Dispatch(i));
            DOCTEST_CHECK_EQ(i, 0);

            DOCTEST_CHECK(dispatcherWhileTrue.Subscribe(receiverTrue));
            DOCTEST_CHECK(dispatcherWhileTrue.Subscribe(receiverTrue));
            DOCTEST_CHECK(dispatcherWhileTrue.Dispatch(i));
            DOCTEST_CHECK_EQ(i, 1);

            DOCTEST_CHECK(dispatcherWhileTrue.Subscribe(receiverFalse));
            DOCTEST_CHECK(dispatcherWhileTrue.Subscribe(receiverFalse));
            DOCTEST_CHECK_FALSE(dispatcherWhileTrue.Dispatch(i));
            DOCTEST_CHECK_EQ(i, 4);

            DOCTEST_CHECK(dispatcherWhileTrue.Subscribe(receiverDummy));
            DOCTEST_CHECK(dispatcherWhileTrue.Subscribe(receiverDummy));
            DOCTEST_CHECK_FALSE(dispatcherWhileTrue.Dispatch(i));
            DOCTEST_CHECK_EQ(i, 7);
        }

        DOCTEST_SUBCASE("While returning false")
        {
            int i = 0;

            auto collector = std::make_unique<Event::CollectWhileFalse>(false);
            Event::Dispatcher<bool(int&)> dispatcherWhileFalse(std::move(collector));
            DOCTEST_CHECK_FALSE(dispatcherWhileFalse.Dispatch(i));
            DOCTEST_CHECK_EQ(i, 0);

            DOCTEST_CHECK(dispatcherWhileFalse.Subscribe(receiverFalse));
            DOCTEST_CHECK(dispatcherWhileFalse.Subscribe(receiverFalse));
            DOCTEST_CHECK_FALSE(dispatcherWhileFalse.Dispatch(i));
            DOCTEST_CHECK_EQ(i, 2);

            DOCTEST_CHECK(dispatcherWhileFalse.Subscribe(receiverTrue));
            DOCTEST_CHECK(dispatcherWhileFalse.Subscribe(receiverTrue));
            DOCTEST_CHECK(dispatcherWhileFalse.Dispatch(i));
            DOCTEST_CHECK_EQ(i, 5);

            DOCTEST_CHECK(dispatcherWhileFalse.Subscribe(receiverDummy));
            DOCTEST_CHECK(dispatcherWhileFalse.Subscribe(receiverDummy));
            DOCTEST_CHECK(dispatcherWhileFalse.Dispatch(i));
            DOCTEST_CHECK_EQ(i, 8);
        }

        DOCTEST_SUBCASE("Stopped by initial false")
        {
            int i = 0;

            auto collector = std::make_unique<Event::CollectWhileTrue>(false);
            Event::Dispatcher<bool(int&)> dispatcherWhileTrue(std::move(collector));
            DOCTEST_CHECK_FALSE(dispatcherWhileTrue.Dispatch(i));
            DOCTEST_CHECK_EQ(i, 0);

            dispatcherWhileTrue.Subscribe(receiverTrue);
            DOCTEST_CHECK_FALSE(dispatcherWhileTrue.Dispatch(i));
            DOCTEST_CHECK_EQ(i, 0);
        }

        DOCTEST_SUBCASE("Stopped by initial true")
        {
            int i = 0;

            auto collector = std::make_unique<Event::CollectWhileFalse>(true);
            Event::Dispatcher<bool(int&)> dispatcherWhileFalse(std::move(collector));
            DOCTEST_CHECK(dispatcherWhileFalse.Dispatch(i));
            DOCTEST_CHECK_EQ(i, 0);

            dispatcherWhileFalse.Subscribe(receiverFalse);
            DOCTEST_CHECK(dispatcherWhileFalse.Dispatch(i));
            DOCTEST_CHECK_EQ(i, 0);
        }

        DOCTEST_CHECK_FALSE(receiverTrue.IsSubscribed());
        DOCTEST_CHECK_FALSE(receiverFalse.IsSubscribed());
        DOCTEST_CHECK_FALSE(receiverDummy.IsSubscribed());
    }

    DOCTEST_SUBCASE("Receiver subscription")
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

        auto dispatchAndDOCTEST_CHECK = [&]()
        {
            dispatcherA.Dispatch(currentA);
            DOCTEST_CHECK_EQ(currentA, expectedA += incrementA);

            dispatcherB.Dispatch(currentB);
            DOCTEST_CHECK_EQ(currentB, expectedB += incrementB);
        };

        DOCTEST_SUBCASE("Subcribe receivers")
        {
            DOCTEST_CHECK(dispatcherA.Subscribe(receiverAddOne));
            DOCTEST_CHECK(dispatcherA.Subscribe(receiverAddTwo));
            DOCTEST_CHECK(dispatcherA.Subscribe(receiverAddThree));
            DOCTEST_CHECK(dispatcherA.Subscribe(receiverAddFour));
            incrementA = 10;

            dispatchAndDOCTEST_CHECK();

            DOCTEST_SUBCASE("Repeat subscription")
            {
                DOCTEST_CHECK(dispatcherA.Subscribe(receiverAddOne));
                DOCTEST_CHECK(dispatcherA.Subscribe(receiverAddTwo));
                DOCTEST_CHECK(dispatcherA.Subscribe(receiverAddThree));
                DOCTEST_CHECK(dispatcherA.Subscribe(receiverAddFour));

                dispatchAndDOCTEST_CHECK();
            }

            DOCTEST_SUBCASE("Unsubcribe nonsubscribed")
            {
                DOCTEST_CHECK_FALSE(dispatcherB.Subscribe(receiverAddOne));
                DOCTEST_CHECK_FALSE(dispatcherB.Subscribe(receiverAddTwo));
                DOCTEST_CHECK_FALSE(dispatcherB.Subscribe(receiverAddThree));
                DOCTEST_CHECK_FALSE(dispatcherB.Subscribe(receiverAddFour));

                dispatchAndDOCTEST_CHECK();
            }

            DOCTEST_SUBCASE("Replace subscription")
            {
                DOCTEST_CHECK(dispatcherB.Subscribe(receiverAddOne,
                    Event::SubscriptionPolicy::ReplaceSubscription));
                incrementA -= 1;
                incrementB += 1;

                dispatchAndDOCTEST_CHECK();

                DOCTEST_CHECK(dispatcherB.Subscribe(receiverAddThree,
                    Event::SubscriptionPolicy::ReplaceSubscription));
                incrementA -= 3;
                incrementB += 3;

                dispatchAndDOCTEST_CHECK();

                DOCTEST_CHECK(dispatcherB.Subscribe(receiverAddFour,
                    Event::SubscriptionPolicy::ReplaceSubscription));
                incrementA -= 4;
                incrementB += 4;

                dispatchAndDOCTEST_CHECK();

                DOCTEST_CHECK(dispatcherB.Subscribe(receiverAddTwo,
                    Event::SubscriptionPolicy::ReplaceSubscription));
                incrementA -= 2;
                incrementB += 2;

                dispatchAndDOCTEST_CHECK();

                DOCTEST_CHECK_FALSE(dispatcherA.Subscribe(receiverAddOne,
                    Event::SubscriptionPolicy::RetainSubscription));
                DOCTEST_CHECK_FALSE(dispatcherA.Subscribe(receiverAddTwo,
                    Event::SubscriptionPolicy::RetainSubscription));
                DOCTEST_CHECK_FALSE(dispatcherA.Subscribe(receiverAddThree,
                    Event::SubscriptionPolicy::RetainSubscription));
                DOCTEST_CHECK_FALSE(dispatcherA.Subscribe(receiverAddFour,
                    Event::SubscriptionPolicy::RetainSubscription));

                dispatchAndDOCTEST_CHECK();
            }
        }

        dispatchAndDOCTEST_CHECK();

        dispatcherA.UnsubscribeAll();
        incrementA = 0;

        dispatcherB.UnsubscribeAll();
        incrementB = 0;

        dispatchAndDOCTEST_CHECK();
    }

    DOCTEST_SUBCASE("Subscription change during dispatch")
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

        DOCTEST_SUBCASE("Subscribe fire once receiver")
        {
            DOCTEST_CHECK(dispatcher.Subscribe(receiverFireOnce));
            firstDispatch += 1;
            secondDispatch += 0;
        }

        DOCTEST_SUBCASE("Subscribe fire always receiver")
        {
            DOCTEST_CHECK(dispatcher.Subscribe(receiverFireAlways));
            firstDispatch += 10;
            secondDispatch += 10;
        }

        DOCTEST_SUBCASE("Susbcribe receiver chain")
        {
            DOCTEST_CHECK(dispatcher.Subscribe(receiverChainD));
            firstDispatch += 111100;
            secondDispatch += 1100;
        }

        DOCTEST_CHECK(dispatcher.Dispatch());
        DOCTEST_CHECK_EQ(value, firstDispatch);

        value = 0;
        DOCTEST_CHECK(dispatcher.Dispatch());
        DOCTEST_CHECK_EQ(value, secondDispatch);

        dispatcher.UnsubscribeAll();

        value = 0;
        DOCTEST_CHECK(dispatcher.Dispatch());
        DOCTEST_CHECK_EQ(value, 0);
    }

    DOCTEST_SUBCASE("Invoking unbound receivers")
    {
        Event::Dispatcher<int(int&)> dispatcher(0);
        Event::Receiver<int(int&)> receiverUnboundFirst;
        Event::Receiver<int(int&)> receiverUnboundMiddle;
        Event::Receiver<int(int&)> receiverUnboundLast;

        Event::Receiver<int(int&)> receiverBoundFirst;
        receiverBoundFirst.Bind([](int& i) { i += 1; return 1; });

        Event::Receiver<int(int&)> receiverBoundSecond;
        receiverBoundSecond.Bind([](int& i) { i += 2; return 2; });

        DOCTEST_CHECK(dispatcher.Subscribe(receiverUnboundFirst));
        DOCTEST_CHECK(dispatcher.Subscribe(receiverBoundFirst));
        DOCTEST_CHECK(dispatcher.Subscribe(receiverUnboundMiddle));
        DOCTEST_CHECK(dispatcher.Subscribe(receiverBoundSecond));
        DOCTEST_CHECK(dispatcher.Subscribe(receiverUnboundLast));

        int value = 0;
        DOCTEST_CHECK_EQ(dispatcher.Dispatch(value), 2);
        DOCTEST_CHECK_EQ(value, 3);
    }

    DOCTEST_SUBCASE("Dispatch copy count")
    {
        Test::InstanceCounter<> counter;
        Event::Dispatcher<char(Test::InstanceCounter<>, int)> dispatcher('\0');
        Event::Receiver<char(Test::InstanceCounter<>, int)> receiver;
        DOCTEST_CHECK(receiver.Subscribe(dispatcher));

        DOCTEST_SUBCASE("Function dispatch")
        {
            receiver.Bind<&Function>();
            DOCTEST_CHECK_EQ(dispatcher.Dispatch(counter, 0), '0');
            DOCTEST_CHECK_EQ(counter.GetStats().copies, 1);
        }

        DOCTEST_SUBCASE("Method dispatch")
        {
            BaseClass baseClass;
            receiver.Bind<BaseClass, &BaseClass::Method>(&baseClass);
            DOCTEST_CHECK_EQ(dispatcher.Dispatch(counter, 3), '3');
            DOCTEST_CHECK_EQ(counter.GetStats().copies, 1);
        }

        DOCTEST_SUBCASE("Lambda dispatch")
        {
            receiver.Bind([](Test::InstanceCounter<>, int index) { return Text[index]; });
            DOCTEST_CHECK_EQ(dispatcher.Dispatch(counter, 5), '5');
            DOCTEST_CHECK_EQ(counter.GetStats().copies, 1);
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

DOCTEST_TEST_CASE("Event Broker")
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

    DOCTEST_SUBCASE("Dispatch empty")
    {
        DOCTEST_CHECK(broker.Dispatch<bool>(EventInteger{ 4 }).IsFailure());
        DOCTEST_CHECK(broker.Dispatch<bool>(EventString{ "Null" }).IsFailure());
        DOCTEST_CHECK_EQ(currentValue, expectedValue);
    }

    DOCTEST_SUBCASE("Dispatch unregistered")
    {
        DOCTEST_CHECK(broker.Subscribe(receiverIntegerTrue).IsFailure());
        DOCTEST_CHECK(broker.Subscribe(receiverStringFalse).IsFailure());

        DOCTEST_CHECK(broker.Dispatch<bool>(EventInteger{ 2 }).IsFailure());
        DOCTEST_CHECK_EQ(currentValue, expectedValue);

        DOCTEST_CHECK(broker.Dispatch<bool>(EventString{ "Jelly" }).IsFailure());
        DOCTEST_CHECK_EQ(currentValue, expectedValue);
    }

    DOCTEST_SUBCASE("Dispatch registered")
    {
        DOCTEST_CHECK(broker.Register<void, EventBoolean>(
            std::make_unique<Event::CollectNothing>()));
        DOCTEST_CHECK(broker.Register<bool, EventInteger>(
            std::make_unique<Event::CollectWhileTrue>()));
        DOCTEST_CHECK(broker.Register<bool, EventString>(
            std::make_unique<Event::CollectWhileFalse>()));
        broker.Finalize();

        DOCTEST_SUBCASE("Register when finalized")
        {
            DOCTEST_CHECK_FALSE(broker.Register<bool, EventVector>());
        }

        DOCTEST_CHECK(broker.Subscribe(receiverBooleanVoid).IsSuccess());
        DOCTEST_CHECK(broker.Subscribe(receiverIntegerTrue).IsSuccess());
        DOCTEST_CHECK(broker.Subscribe(receiverStringFalse).IsSuccess());

        DOCTEST_CHECK(broker.Dispatch<void>(EventBoolean{ true }).IsSuccess());
        DOCTEST_CHECK_EQ(currentValue, expectedValue += 10);

        DOCTEST_CHECK_EQ(broker.Dispatch<bool>(EventInteger{ 2 }).Unwrap(), true);
        DOCTEST_CHECK_EQ(currentValue, expectedValue += 2);

        DOCTEST_CHECK_EQ(broker.Dispatch<bool>(EventString{ "Jelly" }).Unwrap(), false);
        DOCTEST_CHECK_EQ(currentValue, expectedValue += 5);

        DOCTEST_SUBCASE("Dispatch with mismatched result type")
        {
            DOCTEST_CHECK(broker.Dispatch<bool>(EventBoolean{ true }).IsFailure());
            DOCTEST_CHECK_EQ(currentValue, expectedValue);

            DOCTEST_CHECK(broker.Dispatch<int>(EventInteger{ 2 }).IsFailure());
            DOCTEST_CHECK_EQ(currentValue, expectedValue);

            DOCTEST_CHECK(broker.Dispatch<float>(EventString{ "Jelly" }).IsFailure());
            DOCTEST_CHECK_EQ(currentValue, expectedValue);
        }
    }
}
