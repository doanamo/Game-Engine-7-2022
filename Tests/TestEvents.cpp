/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include <Common/Event/Delegate.hpp>
#include <Common/Event/Collector.hpp>
#include <Common/Event/Dispatcher.hpp>
#include <Common/Event/Receiver.hpp>

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
    Common::Delegate<char(const char* c, int i)> delegate;

    if(delegate.IsBound())
        return false;

    // Static function binding.
    delegate.Bind<&Function>();

    if(!delegate.IsBound())
        return false;

    if(delegate.Invoke("Hello world!", 6) != 'w')
        return false;

    // Class method binding.
    BaseClass baseClass;

    delegate.Bind<BaseClass, &BaseClass::Method>(&baseClass);

    if(!delegate.IsBound())
        return false;

    if(delegate.Invoke("Hello world!", 6) != ' ')
        return false;

    // Virtual method binding.
    DerivedClass derivedClass;

    delegate.Bind<BaseClass, &BaseClass::Method>(&derivedClass);

    if(!delegate.IsBound())
        return false;

    if(delegate.Invoke("Hello world!", 6) != 'o')
        return false;

    // Lambda functor binding.
    auto functor = [](const char* c, int i) -> char
    {
        return c[i + 2];
    };

    delegate.Bind(&functor);

    if(!delegate.IsBound())
        return false;

    if(delegate.Invoke("Hello world!", 6) != 'r')
        return false;

    // Lambda binding via constructor.
    {
        int counter = 0;

        Common::Delegate<void()> delegate1([&counter, add = 2]()
        {
            counter += add;
        });

        Common::Delegate<void()> delegate2([&counter, add = 4]()
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

    if(delegate.IsBound())
        return false;

    return true;
}

bool TestCollector()
{
    Common::CollectDefault<void> collectDefault;

    collectDefault.ConsumeResult();

    if(!collectDefault.ShouldContinue())
        return false;

    collectDefault.GetResult();

    Common::CollectLast<int> collectLast(0);

    if(collectLast.GetResult() != 0)
        return false;

    collectLast.ConsumeResult(1);
    if(!collectLast.ShouldContinue())
        return false;

    if(collectLast.GetResult() != 1)
        return false;

    collectLast.ConsumeResult(2);
    if(!collectLast.ShouldContinue())
        return false;

    if(collectLast.GetResult() != 2)
        return false;

    collectLast.ConsumeResult(3);
    if(!collectLast.ShouldContinue())
        return false;

    if(collectLast.GetResult() != 3)
        return false;

    collectLast.ConsumeResult(4);
    if(!collectLast.ShouldContinue())
        return false;

    if(collectLast.GetResult() != 4)
        return false;

    Common::CollectWhileTrue collectWhileTrue(true);

    if(collectWhileTrue.GetResult() != true)
        return false;

    collectWhileTrue.ConsumeResult(true);
    if(!collectWhileTrue.ShouldContinue())
        return false;

    if(collectWhileTrue.GetResult() != true)
        return false;

    collectWhileTrue.ConsumeResult(false);
    if(collectWhileTrue.ShouldContinue())
        return false;

    if(collectWhileTrue.GetResult() != false)
        return false;

    Common::CollectWhileFalse collectWhileFalse(false);

    if(collectWhileFalse.GetResult() != false)
        return false;

    collectWhileFalse.ConsumeResult(false);
    if(!collectWhileFalse.ShouldContinue())
        return false;

    if(collectWhileFalse.GetResult() != false)
        return false;

    collectWhileFalse.ConsumeResult(true);
    if(collectWhileFalse.ShouldContinue())
        return false;

    if(collectWhileFalse.GetResult() != true)
        return false;

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

        Common::Receiver<int(int*)> receiverA;
        receiverA.Bind<DispatcherClass, &DispatcherClass::FunctionA>(&dispatcherClass);

        Common::Receiver<int(int*)> receiverB;
        receiverB.Bind<DispatcherClass, &DispatcherClass::FunctionB>(&dispatcherClass);

        Common::Dispatcher<int(int*)> dispatcher(42);

        if(dispatcher.Dispatch(&i) != 42)
            return false;

        if(i != 0)
            return false;

        dispatcher.Subscribe(receiverA);

        if(dispatcher.Dispatch(&i) != 32)
            return false;

        if(i != 2)
            return false;

        dispatcher.Subscribe(receiverB);

        if(dispatcher.Dispatch(&i) != 33)
            return false;

        if(i != 8)
            return false;

        receiverB.Unsubscribe();

        if(dispatcher.Dispatch(&i) != 32)
            return false;

        if(i != 10)
            return false;

        dispatcher.Unsubscribe(receiverA);

        if(dispatcher.Dispatch(&i) != 42)
            return false;

        if(i != 10)
            return false;
    }

    // Dispatcher with collect while true/false collector.
    {
        int i = 0;

        Common::Receiver<bool(int*)> receiverTrue;
        receiverTrue.Bind<DispatcherClass, &DispatcherClass::FunctionTrue>(&dispatcherClass);
        
        Common::Receiver<bool(int*)> receiverFalse;
        receiverFalse.Bind<DispatcherClass, &DispatcherClass::FunctionFalse>(&dispatcherClass);

        Common::Receiver<bool(int*)> receiverDummy;
        receiverDummy.Bind<DispatcherClass, &DispatcherClass::FunctionDummy>(&dispatcherClass);

        Common::Dispatcher<bool(int*), Common::CollectWhileTrue> dispatcherWhileTrue(true);

        if(dispatcherWhileTrue.Dispatch(&i) != true)
            return false;

        if(i != 0)
            return false;

        dispatcherWhileTrue.Subscribe(receiverTrue);

        if(dispatcherWhileTrue.Dispatch(&i) != true)
            return false;

        if(i != 3)
            return false;

        dispatcherWhileTrue.Subscribe(receiverFalse);

        if(dispatcherWhileTrue.Dispatch(&i) != false)
            return false;

        if(i != 15)
            return false;

        dispatcherWhileTrue.Subscribe(receiverDummy);

        if(dispatcherWhileTrue.Dispatch(&i) != false)
            return false;

        if(i != 27)
            return false;

        int y = 0;

        Common::Dispatcher<bool(int*), Common::CollectWhileFalse> dispatcherWhileFalse(false);

        if(dispatcherWhileFalse.Dispatch(&y) != false)
            return false;

        if(y != 0)
            return false;

        dispatcherWhileFalse.Subscribe(receiverFalse, true);

        if(dispatcherWhileFalse.Dispatch(&y) != false)
            return false;

        if(y != 9)
            return false;

        dispatcherWhileFalse.Subscribe(receiverTrue, true);

        if(dispatcherWhileFalse.Dispatch(&y) != true)
            return false;

        if(y != 21)
            return false;

        dispatcherWhileFalse.Subscribe(receiverDummy);

        if(dispatcherWhileFalse.Dispatch(&y) != true)
            return false;

        if(y != 33)
            return false;
    }

    // Caling dispatcher with failing initial state.
    {
        int i = 0;

        Common::Receiver<bool(int*)> receiverTrue;
        receiverTrue.Bind<DispatcherClass, &DispatcherClass::FunctionTrue>(&dispatcherClass);

        Common::Receiver<bool(int*)> receiverFalse;
        receiverFalse.Bind<DispatcherClass, &DispatcherClass::FunctionFalse>(&dispatcherClass);

        Common::Dispatcher<bool(int*), Common::CollectWhileTrue> dispatcherWhileTrue(false);

        if(dispatcherWhileTrue.Dispatch(&i) != false)
            return false;

        if(i != 0)
            return false;

        dispatcherWhileTrue.Subscribe(receiverTrue);

        if(dispatcherWhileTrue.Dispatch(&i) != false)
            return false;

        if(i != 0)
            return false;

        Common::Dispatcher<bool(int*), Common::CollectWhileFalse> dispatcherWhileFalse(true);

        if(dispatcherWhileFalse.Dispatch(&i) != true)
            return false;

        if(i != 0)
            return false;

        dispatcherWhileFalse.Subscribe(receiverFalse);

        if(dispatcherWhileFalse.Dispatch(&i) != true)
            return false;

        if(i != 0)
            return false;
    }

    // Removing and adding receivers to dispatcher with void return type.
    {
        int i = 0;

        DispatcherClass dispatcherClass;

        Common::Receiver<void(int*)> receiverAddOne;
        receiverAddOne.Bind<DispatcherClass, &DispatcherClass::FunctionAddOne>(&dispatcherClass);

        Common::Receiver<void(int*)> receiverAddTwo;
        receiverAddTwo.Bind<DispatcherClass, &DispatcherClass::FunctionAddTwo>(&dispatcherClass);

        Common::Receiver<void(int*)> receiverAddThree;
        receiverAddThree.Bind<DispatcherClass, &DispatcherClass::FunctionAddThree>(&dispatcherClass);

        Common::Receiver<void(int*)> receiverAddFour;
        receiverAddFour.Bind<DispatcherClass, &DispatcherClass::FunctionAddFour>(&dispatcherClass);

        Common::Dispatcher<void(int*)> dispatcherA;
        Common::Dispatcher<void(int*)> dispatcherB;

        dispatcherA.Subscribe(receiverAddOne);
        dispatcherA.Subscribe(receiverAddTwo);
        dispatcherA.Subscribe(receiverAddThree);
        dispatcherA.Subscribe(receiverAddFour);

        dispatcherA.Dispatch(&i);

        if(i != 10)
            return false;

        if(!dispatcherA.Subscribe(receiverAddOne))
            return false;

        if(!dispatcherA.Subscribe(receiverAddTwo))
            return false;

        if(!dispatcherA.Subscribe(receiverAddThree))
            return false;

        if(!dispatcherA.Subscribe(receiverAddFour))
            return false;

        dispatcherA.Dispatch(&i);

        if(i != 20)
            return false;

        if(dispatcherB.Subscribe(receiverAddOne))
            return false;

        dispatcherA.Dispatch(&i);

        if(i != 30)
            return false;

        dispatcherB.Dispatch(&i);

        if(i != 30)
            return false;

        if(!dispatcherB.Subscribe(receiverAddOne, true))
            return false;

        dispatcherA.Dispatch(&i);

        if(i != 39)
            return false;

        dispatcherB.Dispatch(&i);

        if(i != 40)
            return false;

        if(!dispatcherB.Subscribe(receiverAddThree, true))
            return false;

        dispatcherA.Dispatch(&i);

        if(i != 46)
            return false;

        dispatcherB.Dispatch(&i);

        if(i != 50)
            return false;

        if(!dispatcherB.Subscribe(receiverAddFour, true))
            return false;

        dispatcherA.Dispatch(&i);

        if(i != 52)
            return false;

        dispatcherB.Dispatch(&i);

        if(i != 60)
            return false;

        if(!dispatcherB.Subscribe(receiverAddTwo, true))
            return false;

        dispatcherA.Dispatch(&i);

        if(i != 60)
            return false;

        dispatcherB.Dispatch(&i);

        if(i != 70)
            return false;

        receiverAddTwo.Unsubscribe();

        dispatcherB.Dispatch(&i);

        if(i != 78)
            return false;

        dispatcherB.Unsubscribe(receiverAddFour);

        dispatcherB.Dispatch(&i);

        if(i != 82)
            return false;

        dispatcherB.UnsubscribeAll();

        dispatcherA.Dispatch(&i);
        dispatcherB.Dispatch(&i);

        if(i != 82)
            return false;
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
