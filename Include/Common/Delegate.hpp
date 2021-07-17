/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Common/Debug.hpp"

/*
    Delegate

    Binds function, class method or functor/closure which can be invoked
    at later time. Be careful not to invoke a delegate to method of an 
    object that no longer exists. Check Receiver and Dispatcher class
    templates for subscription based solution that wraps delegates.
    
    Implementation partially based on:
    - http://molecularmusings.wordpress.com/2011/09/19/generic-type-safe-delegates-and-events-in-c/
*/

namespace Event
{
    template<typename Type>
    class Delegate;

    template<typename ReturnType, typename... Arguments>
    class Delegate<ReturnType(Arguments...)>
    {
    private:
        using ErasedPtr = void*;
        using InvokerPtr = ReturnType(*)(ErasedPtr, Arguments&&...);
        using CopierPtr = void*(*)(void*);
        using DeleterPtr = void(*)(void*);

        template<ReturnType(*Function)(Arguments...)>
        static ReturnType FunctionStub(ErasedPtr erased, Arguments&&... arguments)
        {
            return Function(std::forward<Arguments>(arguments)...);
        }

        template<class FunctionType, ReturnType(FunctionType::*Function)(Arguments...)>
        static ReturnType MethodStub(ErasedPtr erased, Arguments&&... arguments)
        {
            return (static_cast<FunctionType*>(erased)->*Function)
                (std::forward<Arguments>(arguments)...);
        }

        template<class FunctionType>
        static ReturnType FunctorStub(ErasedPtr erased, Arguments&&... arguments)
        {
            return (*static_cast<FunctionType*>(erased))
                (std::forward<Arguments>(arguments)...);
        }

    public:
        Delegate() = default;

        virtual ~Delegate()
        {
            ClearBinding();
        }

        Delegate(const Delegate& other)
        {
            *this = other;
        }

        Delegate& operator=(const Delegate& other)
        {
            ASSERT(&other != this);

            if(other.m_copier)
            {
                m_erased = other.m_copier(other.m_erased);
            }
            else
            {
                m_erased = other.m_erased;
            }

            m_invoker = other.m_invoker;
            m_copier = other.m_copier;
            m_deleter = other.m_deleter;
            return *this;
        }

        Delegate(Delegate&& other) :
            Delegate()
        {
            *this = std::move(other);
        }

        Delegate& operator=(Delegate&& other)
        {
            ASSERT(&other != this);

            std::swap(m_erased, other.m_erased);
            std::swap(m_invoker, other.m_invoker);
            std::swap(m_copier, other.m_copier);
            std::swap(m_deleter, other.m_deleter);
            return *this;
        }

        Delegate& operator=(std::nullptr_t)
        {
            Bind(nullptr);
            return *this;
        }

        void Bind(std::nullptr_t)
        {
            ClearBinding();
        }

        template<ReturnType(*Function)(Arguments...)>
        void Bind()
        {
            ClearBinding();

            m_erased = nullptr;
            m_invoker = &FunctionStub<Function>;
        }

        template<class FunctionType>
        void Bind(FunctionType* function)
        {
            static_assert(std::is_invocable<FunctionType, Arguments...>::value,
                "Arguments of provided callable must match arguments of delegate!");

            ClearBinding();

            if(function)
            {
                m_erased = function;
                m_invoker = &FunctorStub<FunctionType>;
            }
        }

        template<class InstanceType, ReturnType(InstanceType::*Method)(Arguments...)>
        void Bind(InstanceType* instance)
        {
            ClearBinding();

            if(instance)
            {
                m_erased = instance;
                m_invoker = &MethodStub<InstanceType, Method>;
            }
        }

        template<typename FunctionType>
        Delegate(FunctionType closure)
        {
            Bind(std::forward<FunctionType>(closure));
        }

        template<typename FunctionType>
        Delegate& operator=(FunctionType closure)
        {
            Bind(std::forward<FunctionType>(closure));
            return *this;
        }

        template<typename FunctionType>
        void Bind(FunctionType closure)
        {
            static_assert(std::is_invocable<FunctionType, Arguments...>::value,
                "Arguments of provided callable must match arguments of delegate!");

            ClearBinding();

            if constexpr(std::is_convertible<FunctionType, ReturnType(*)(Arguments...)>::value)
            {
                m_erased = static_cast<void*>(&closure);
                m_invoker = &FunctorStub<FunctionType>;
            }
            else
            {
                m_erased = new FunctionType(std::forward<FunctionType>(closure));
                m_invoker = &FunctorStub<FunctionType>;

                m_copier = [](void* closure) -> void*
                {
                    ASSERT(closure);
                    return new FunctionType(*static_cast<FunctionType*>(closure));
                };

                m_deleter = [](void* closure) -> void
                {
                    ASSERT(closure);
                    delete static_cast<FunctionType*>(closure);
                };
            }
        }

        auto Invoke(Arguments... arguments)
        {
            return Invoke(std::is_void<ReturnType>{}, std::forward<Arguments>(arguments)...);
        }

        auto operator()(Arguments... arguments)
        {
            return Invoke(std::forward<Arguments>(arguments)...);
        }

        bool IsBound()
        {
            return m_invoker != nullptr;
        }

    private:
        void ClearBinding()
        {
            if(m_deleter)
            {
                m_deleter(m_erased);
                m_copier = nullptr;
                m_deleter = nullptr;
            }

            m_erased = nullptr;
            m_invoker = nullptr;
        }

        ReturnType Invoke(std::false_type, Arguments&&... arguments)
        {
            if(m_invoker)
            {
                return m_invoker(m_erased, std::forward<Arguments>(arguments)...);
            }
            else
            {
                return {};
            }
        }

        void Invoke(std::true_type, Arguments&&... arguments)
        {
            if(m_invoker)
            {
                m_invoker(m_erased, std::forward<Arguments>(arguments)...);
            }
        }

        ErasedPtr m_erased = nullptr;
        InvokerPtr m_invoker = nullptr;
        CopierPtr m_copier = nullptr;
        DeleterPtr m_deleter = nullptr;
    };
}
