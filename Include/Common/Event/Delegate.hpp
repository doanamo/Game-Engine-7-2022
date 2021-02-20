/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Common/Debug.hpp"

/*
    Delegate

    Binds function, class method or functor/lambda which can be invoked
    at later time. Be careful not to invoke a delegate to method of an 
    instance that no longer exists. Check Receiver and Dispatcher class
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
        using InstancePtr = void*;
        using InvokerPtr = ReturnType(*)(InstancePtr, Arguments&&...);
        using CopierPtr = void*(*)(void*);
        using DeleterPtr = void(*)(void*);

        template<ReturnType(*Function)(Arguments...)>
        static ReturnType FunctionStub(InstancePtr function, Arguments&&... arguments)
        {
            return (Function)(std::forward<Arguments>(arguments)...);
        }

        template<class FunctionType, ReturnType(FunctionType::*Function)(Arguments...)>
        static ReturnType MethodStub(InstancePtr function, Arguments&&... arguments)
        {
            return (static_cast<FunctionType*>(function)->*Function)
                (std::forward<Arguments>(arguments)...);
        }

        template<class FunctionType>
        static ReturnType FunctorStub(InstancePtr function, Arguments&&... arguments)
        {
            return (*static_cast<FunctionType*>(function))
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
            if(other.m_copier)
            {
                m_instance = other.m_copier(other.m_instance);
            }
            else
            {
                m_instance = other.m_instance;
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
            std::swap(m_instance, other.m_instance);
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

            m_instance = nullptr;
            m_invoker = &FunctionStub<Function>;
        }

        template<class FunctionType>
        void Bind(FunctionType* instance)
        {
            ClearBinding();

            if(instance)
            {
                m_instance = instance;
                m_invoker = &FunctorStub<FunctionType>;
            }
        }

        template<class FunctionType, ReturnType(FunctionType::*Function)(Arguments...)>
        void Bind(FunctionType* instance)
        {
            ClearBinding();

            if(instance)
            {
                m_instance = instance;
                m_invoker = &MethodStub<FunctionType, Function>;
            }
        }

        template<typename Lambda>
        Delegate(Lambda lambda)
        {
            Bind(std::forward<Lambda>(lambda));
        }

        template<typename Lambda>
        Delegate& operator=(Lambda lambda)
        {
            Bind(std::forward<Lambda>(lambda));
            return *this;
        }

        template<typename Lambda>
        void Bind(Lambda lambda)
        {
            ClearBinding();

            if constexpr(std::is_convertible<Lambda, ReturnType(*)(Arguments...)>::value)
            {
                m_instance = static_cast<void*>(&lambda);
                m_invoker = &FunctorStub<Lambda>;
            }
            else
            {
                m_instance = new Lambda(std::forward<Lambda>(lambda));
                m_invoker = &FunctorStub<Lambda>;

                m_copier = [](void* lambda) -> void*
                {
                    ASSERT(lambda, "Lambda instance should be present if there is copier!");
                    return new Lambda(*static_cast<Lambda*>(lambda));
                };

                m_deleter = [](void* lambda)
                {
                    delete static_cast<Lambda*>(lambda);
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
                m_deleter(m_instance);
                m_copier = nullptr;
                m_deleter = nullptr;
            }

            m_instance = nullptr;
            m_invoker = nullptr;
        }

        ReturnType Invoke(std::false_type, Arguments&&... arguments)
        {
            if(m_invoker)
            {
                return m_invoker(m_instance, std::forward<Arguments>(arguments)...);
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
                m_invoker(m_instance, std::forward<Arguments>(arguments)...);
            }
        }

        InstancePtr m_instance = nullptr;
        InvokerPtr m_invoker = nullptr;
        CopierPtr m_copier = nullptr;
        DeleterPtr m_deleter = nullptr;
    };
}
