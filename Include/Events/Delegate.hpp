/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Common/Debug.hpp"

/*
    Delegate

    Binds a function, a class method or a functor object which can be invoked at a later time.
    Be careful not to invoke a delagate to a method of an instance that no longer exists.
    Check Receiver and Dispatcher class templates for a subscription based solution that wraps delegates.
    
    void ExampleDelegateFunction()
    {
        // Create a delegate that can bind to functions such as:
        // bool Function(const char* c, int i) { ... }
        Delegate<bool(const char*, int)> delegate;

        // Bind the delegate to a function.
        delegate.Bind<&Function>();

        // Call the function by invoking the delegate.
        delegate.Invoke("hello", 5);
    }
    
    void ExampleDelegateMethod(Class& instance)
    {
        // Create a delegate that can bind to class methods such as:
        // bool Class::Function(const char* c, int i) { ... }
        Delegate<bool(const char*, int)> delegate;

        // Bind the delegate to a method of a class instance.
        delegate.Bind<Class, &Class::Function>(&instance);

        // Call the method of the instance by invoking the delegate.
        // Be careful to not call the delegate on an instance that no longer exists.
        delegate.Invoke("hello", 5);
    }

    void ExampleDelegateFunctor()
    {
        // Create a functor (object that overloads the call operator).
        auto functor = [](const char* c, int i) -> bool
        {
            return true;
        };

        // Create a delegate that can bind to functors with above arguments.
        Delegate<bool(const char*, int)> delegate;

        // Bind the delegate to a functor instance.
        delegate.Bind(&Object);

        // Call the functor by invoking the delegate.
        delegate.Invoke("hello", 5);
    }
    
    Implementation based on: http://molecularmusings.wordpress.com/2011/09/19/generic-type-safe-delegates-and-events-in-c/
*/

namespace Common
{
    template<typename Type>
    class Delegate;

    template<typename ReturnType, typename... Arguments>
    class Delegate<ReturnType(Arguments...)>
    {
    private:
        // Type declarations.
        typedef void* InstancePtr;
        typedef ReturnType(*FunctionPtr)(InstancePtr, Arguments...);

        // Compile time invocation stubs.
        template<ReturnType(*Function)(Arguments...)>
        static ReturnType FunctionStub(InstancePtr instance, Arguments... arguments)
        {
            return (Function)(std::forward<Arguments>(arguments)...);
        }

        template<class InstanceType, ReturnType(InstanceType::*Function)(Arguments...)>
        static ReturnType MethodStub(InstancePtr instance, Arguments... arguments)
        {
            return (static_cast<InstanceType*>(instance)->*Function)(std::forward<Arguments>(arguments)...);
        }

        template<class InstanceType>
        static ReturnType FunctorStub(InstancePtr instance, Arguments... arguments)
        {
            return (*static_cast<InstanceType*>(instance))(std::forward<Arguments>(arguments)...);
        }

    public:
        Delegate() :
            m_instance(nullptr),
            m_function(nullptr)
        {
        }

        virtual ~Delegate()
        {
        }

        // Unbinds the delegate.
        void Bind(std::nullptr_t)
        {
            m_instance = nullptr;
            m_function = nullptr;
        }

        // Binds a function.
        template<ReturnType(*Function)(Arguments...)>
        void Bind()
        {
            m_instance = nullptr;
            m_function = &FunctionStub<Function>;
        }

        // Binds a functor object.
        template<class InstanceType>
        void Bind(InstanceType* instance)
        {
            ASSERT(instance != nullptr, "Received nullptr as functor instance!");

            m_instance = instance;
            m_function = &FunctorStub<InstanceType>;
        }

        // Binds an instance method.
        template<class InstanceType, ReturnType(InstanceType::*Function)(Arguments...)>
        void Bind(InstanceType* instance)
        {
            ASSERT(instance != nullptr, "Received nullptr as method instance!");

            m_instance = instance;
            m_function = &MethodStub<InstanceType, Function>;
        }

        // Invokes the delegate.
        ReturnType Invoke(Arguments... arguments)
        {
            VERIFY(m_function != nullptr, "Attempting to invoke a delegate without a bound function!");
            return m_function(m_instance, std::forward<Arguments>(arguments)...);
        }

        // Checks if the delegate is bound.
        bool IsBound()
        {
            return m_function != nullptr;
        }

    private:
        // Pointer to an instance of the delegate.
        // Will be nullptr for standalone functions.
        InstancePtr m_instance;

        // Pointer to a function generated from one of the three specialized
        // templates (either for raw functions, methods and functors).
        FunctionPtr m_function;
    };
}
