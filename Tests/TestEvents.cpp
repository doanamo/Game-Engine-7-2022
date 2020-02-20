/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "TestHelpers.hpp"
#include <Event/Delegate.hpp>
#include <Event/Collector.hpp>
#include <Event/Dispatcher.hpp>
#include <Event/Receiver.hpp>

char Function(const char* c, int i)
{
    return c[i];
}

class BaseClass
{
public:
    virtual char Method(const char* c, int i)
    {
        return c[i - 1];
    }
};

class DerivedClass : public BaseClass
{
public:
    char Method(const char* c, int i) override
    {
        return c[i + 1];
    }
};

bool TestDelegate()
{
    Event::Delegate<char(const char* c, int i)> delegate;

    TEST_FALSE(delegate.IsBound());

    // Static function binding.
    delegate.Bind<&Function>();

    TEST_TRUE(delegate.IsBound());
    TEST_EQ(delegate.Invoke("Hello world!", 6), 'w');

    delegate.Bind(nullptr);

    TEST_FALSE(delegate.IsBound());

    // Class method binding.
    BaseClass baseClass;

    delegate.Bind<BaseClass, &BaseClass::Method>(&baseClass);

    TEST_TRUE(delegate.IsBound());
    TEST_EQ(delegate.Invoke("Hello world!", 6), ' ');

    delegate.Bind(nullptr);

    TEST_FALSE(delegate.IsBound());

    // Virtual method binding.
    DerivedClass derivedClass;

    delegate.Bind<BaseClass, &BaseClass::Method>(&derivedClass);

    TEST_TRUE(delegate.IsBound());
    TEST_EQ(delegate.Invoke("Hello world!", 6), 'o');

    delegate.Bind(nullptr);

    TEST_FALSE(delegate.IsBound());

    // Lambda functor binding.
    auto functor = [](const char* c, int i) -> char
    {
        return c[i + 2];
    };

    delegate.Bind(&functor);

    TEST_TRUE(delegate.IsBound());
    TEST_EQ(delegate.Invoke("Hello world!", 6), 'r');

    delegate.Bind(nullptr);

    TEST_FALSE(delegate.IsBound());

    // Lambda binding via constructor.
    {
        int counter = 0;

        Event::Delegate<void()> delegate1([&counter, add = 2]()
        {
            counter += add;
        });

        Event::Delegate<void()> delegate2([&counter, add = 4]()
        {
            counter += add;
        });

        delegate1();
        delegate2();

        if(counter != 6)
            return false;
    }

    // Delegate unbinding.
    delegate.Bind(nullptr);

    TEST_FALSE(delegate.IsBound());

    return true;
}

bool TestCollector()
{
    {
        Event::CollectDefault<void> collectDefault;

        collectDefault.ConsumeResult();
        TEST_TRUE(collectDefault.ShouldContinue());
        TEST_VOID(collectDefault.GetResult());
    }

    {
        Event::CollectLast<int> collectLast(0);
        TEST_EQ(collectLast.GetResult(), 0);

        collectLast.ConsumeResult(1);
        TEST_TRUE(collectLast.ShouldContinue());
        TEST_EQ(collectLast.GetResult(), 1);

        collectLast.ConsumeResult(2);
        TEST_TRUE(collectLast.ShouldContinue());
        TEST_EQ(collectLast.GetResult(), 2);

        collectLast.ConsumeResult(3);
        TEST_TRUE(collectLast.ShouldContinue());
        TEST_EQ(collectLast.GetResult(), 3);

        collectLast.ConsumeResult(4);
        TEST_TRUE(collectLast.ShouldContinue());
        TEST_EQ(collectLast.GetResult(), 4);
    }
    
    {
        Event::CollectWhileTrue collectWhileTrue(true);
        TEST_EQ(collectWhileTrue.GetResult(), true);

        collectWhileTrue.ConsumeResult(true);
        TEST_TRUE(collectWhileTrue.ShouldContinue());
        TEST_EQ(collectWhileTrue.GetResult(), true);

        collectWhileTrue.ConsumeResult(false);
        TEST_FALSE(collectWhileTrue.ShouldContinue());
        TEST_EQ(collectWhileTrue.GetResult(), false);
    }
    
    {
        Event::CollectWhileFalse collectWhileFalse(false);
        TEST_EQ(collectWhileFalse.GetResult(), false);

        collectWhileFalse.ConsumeResult(false);
        TEST_TRUE(collectWhileFalse.ShouldContinue());
        TEST_EQ(collectWhileFalse.GetResult(), false);

        collectWhileFalse.ConsumeResult(true);
        TEST_FALSE(collectWhileFalse.ShouldContinue());
        TEST_EQ(collectWhileFalse.GetResult(), true);
    }


    return true;
}

class DispatcherClass
{
public:
    int FunctionA(int* i)
    {
        *i += 2;
        return 32;
    }

    int FunctionB(int* i)
    {
        *i += 4;
        return 33;
    }

    bool FunctionTrue(int* i)
    {
        *i += 3;
        return true;
    }

    bool FunctionFalse(int* i)
    {
        *i += 9;
        return false;
    }

    bool FunctionDummy(int* i)
    {
        *i += 99999;
        return true;
    }

    void FunctionAddOne(int* i)
    {
        *i += 1;
    }

    void FunctionAddTwo(int* i)
    {
        *i += 2;
    }

    void FunctionAddThree(int* i)
    {
        *i += 3;
    }

    void FunctionAddFour(int* i)
    {
        *i += 4;
    }
};

bool TestDispatcher()
{
    DispatcherClass dispatcherClass;

    // Basic dispatcher usage with collect last collector.
    {
        int i = 0;

        Event::Receiver<int(int*)> receiverA;
        receiverA.Bind<DispatcherClass, &DispatcherClass::FunctionA>(&dispatcherClass);

        Event::Receiver<int(int*)> receiverB;
        receiverB.Bind<DispatcherClass, &DispatcherClass::FunctionB>(&dispatcherClass);

        Event::Dispatcher<int(int*)> dispatcher(42);

        TEST_EQ(dispatcher.Dispatch(&i), 42);
        TEST_EQ(i, 0);

        dispatcher.Subscribe(receiverA);

        TEST_EQ(dispatcher.Dispatch(&i), 32);
        TEST_EQ(i, 2);

        dispatcher.Subscribe(receiverB);

        TEST_EQ(dispatcher.Dispatch(&i), 33);
        TEST_EQ(i, 8);

        receiverB.Unsubscribe();

        TEST_EQ(dispatcher.Dispatch(&i), 32);
        TEST_EQ(i, 10);

        dispatcher.Unsubscribe(receiverA);

        TEST_EQ(dispatcher.Dispatch(&i), 42);
        TEST_EQ(i, 10);
    }

    // Dispatcher with collect while true/false collector.
    {
        int i = 0;

        Event::Receiver<bool(int*)> receiverTrue;
        receiverTrue.Bind<DispatcherClass, &DispatcherClass::FunctionTrue>(&dispatcherClass);
        
        Event::Receiver<bool(int*)> receiverFalse;
        receiverFalse.Bind<DispatcherClass, &DispatcherClass::FunctionFalse>(&dispatcherClass);

        Event::Receiver<bool(int*)> receiverDummy;
        receiverDummy.Bind<DispatcherClass, &DispatcherClass::FunctionDummy>(&dispatcherClass);

        Event::Dispatcher<bool(int*), Event::CollectWhileTrue> dispatcherWhileTrue(true);

        TEST_TRUE(dispatcherWhileTrue.Dispatch(&i));
        TEST_EQ(i, 0);

        dispatcherWhileTrue.Subscribe(receiverTrue);

        TEST_TRUE(dispatcherWhileTrue.Dispatch(&i));
        TEST_EQ(i, 3);

        dispatcherWhileTrue.Subscribe(receiverFalse);

        TEST_FALSE(dispatcherWhileTrue.Dispatch(&i))
        TEST_EQ(i, 15);

        dispatcherWhileTrue.Subscribe(receiverDummy);

        TEST_FALSE(dispatcherWhileTrue.Dispatch(&i));
        TEST_EQ(i, 27);

        int y = 0;

        Event::Dispatcher<bool(int*), Event::CollectWhileFalse> dispatcherWhileFalse(false);

        TEST_FALSE(dispatcherWhileFalse.Dispatch(&y));
        TEST_EQ(y, 0);

        dispatcherWhileFalse.Subscribe(receiverFalse, true);

        TEST_FALSE(dispatcherWhileFalse.Dispatch(&y));
        TEST_EQ(y, 9);

        dispatcherWhileFalse.Subscribe(receiverTrue, true);

        TEST_TRUE(dispatcherWhileFalse.Dispatch(&y));
        TEST_EQ(y, 21);

        dispatcherWhileFalse.Subscribe(receiverDummy);

        TEST_TRUE(dispatcherWhileFalse.Dispatch(&y));
        TEST_EQ(y, 33)
    }

    // Calling dispatcher with failing initial state.
    {
        int i = 0;

        Event::Receiver<bool(int*)> receiverTrue;
        receiverTrue.Bind<DispatcherClass, &DispatcherClass::FunctionTrue>(&dispatcherClass);

        Event::Receiver<bool(int*)> receiverFalse;
        receiverFalse.Bind<DispatcherClass, &DispatcherClass::FunctionFalse>(&dispatcherClass);

        Event::Dispatcher<bool(int*), Event::CollectWhileTrue> dispatcherWhileTrue(false);

        TEST_FALSE(dispatcherWhileTrue.Dispatch(&i));
        TEST_EQ(i, 0);

        dispatcherWhileTrue.Subscribe(receiverTrue);

        TEST_FALSE(dispatcherWhileTrue.Dispatch(&i));
        TEST_EQ(i, 0);

        Event::Dispatcher<bool(int*), Event::CollectWhileFalse> dispatcherWhileFalse(true);

        TEST_TRUE(dispatcherWhileFalse.Dispatch(&i));
        TEST_EQ(i, 0);

        dispatcherWhileFalse.Subscribe(receiverFalse);

        TEST_TRUE(dispatcherWhileFalse.Dispatch(&i));
        TEST_EQ(i, 0);
    }

    // Removing and adding receivers to dispatcher with void return type.
    {
        int i = 0;

        DispatcherClass dispatcherClass;

        Event::Receiver<void(int*)> receiverAddOne;
        receiverAddOne.Bind<DispatcherClass, &DispatcherClass::FunctionAddOne>(&dispatcherClass);

        Event::Receiver<void(int*)> receiverAddTwo;
        receiverAddTwo.Bind<DispatcherClass, &DispatcherClass::FunctionAddTwo>(&dispatcherClass);

        Event::Receiver<void(int*)> receiverAddThree;
        receiverAddThree.Bind<DispatcherClass, &DispatcherClass::FunctionAddThree>(&dispatcherClass);

        Event::Receiver<void(int*)> receiverAddFour;
        receiverAddFour.Bind<DispatcherClass, &DispatcherClass::FunctionAddFour>(&dispatcherClass);

        Event::Dispatcher<void(int*)> dispatcherA;
        Event::Dispatcher<void(int*)> dispatcherB;

        dispatcherA.Subscribe(receiverAddOne);
        dispatcherA.Subscribe(receiverAddTwo);
        dispatcherA.Subscribe(receiverAddThree);
        dispatcherA.Subscribe(receiverAddFour);

        dispatcherA.Dispatch(&i);

        TEST_EQ(i, 10);
        TEST_TRUE(dispatcherA.Subscribe(receiverAddOne));
        TEST_TRUE(dispatcherA.Subscribe(receiverAddTwo));
        TEST_TRUE(dispatcherA.Subscribe(receiverAddThree));
        TEST_TRUE(dispatcherA.Subscribe(receiverAddFour));

        dispatcherA.Dispatch(&i);

        TEST_EQ(i, 20);
        TEST_FALSE(dispatcherB.Subscribe(receiverAddOne));

        dispatcherA.Dispatch(&i);

        TEST_EQ(i, 30);

        dispatcherB.Dispatch(&i);

        TEST_EQ(i, 30);
        TEST_TRUE(dispatcherB.Subscribe(receiverAddOne, true));

        dispatcherA.Dispatch(&i);

        TEST_EQ(i, 39);

        dispatcherB.Dispatch(&i);

        TEST_EQ(i, 40);
        TEST_TRUE(dispatcherB.Subscribe(receiverAddThree, true));

        dispatcherA.Dispatch(&i);

        TEST_EQ(i, 46);

        dispatcherB.Dispatch(&i);

        TEST_EQ(i, 50);
        TEST_TRUE(dispatcherB.Subscribe(receiverAddFour, true));

        dispatcherA.Dispatch(&i);

        TEST_EQ(i, 52);

        dispatcherB.Dispatch(&i);

        TEST_EQ(i, 60);
        TEST_TRUE(dispatcherB.Subscribe(receiverAddTwo, true));

        dispatcherA.Dispatch(&i);

        TEST_EQ(i, 60);

        dispatcherB.Dispatch(&i);

        TEST_EQ(i, 70);

        receiverAddTwo.Unsubscribe();

        dispatcherB.Dispatch(&i);

        TEST_EQ(i, 78);

        dispatcherB.Unsubscribe(receiverAddFour);

        dispatcherB.Dispatch(&i);

        TEST_EQ(i, 82);

        dispatcherB.UnsubscribeAll();

        dispatcherA.Dispatch(&i);
        dispatcherB.Dispatch(&i);

        TEST_EQ(i, 82);
    }

    return true;
}

int main()
{
    if(!TestDelegate())
        return 1;

    if(!TestCollector())
        return 1;

    if(!TestDispatcher())
        return 1;

    return 0;
}
