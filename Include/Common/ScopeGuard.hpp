/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <utility>
#include <memory>
#include "NonCopyable.hpp"

/*
    Scope Guard

    Executes bound function at the end of scope.

    See unit tests for example usage.
*/

namespace Common
{
    template<typename Type>
    class ScopeGuard : private NonCopyable
    {
    public:
        ScopeGuard(Type function) :
            m_function(function)
        {
        }

        ScopeGuard(ScopeGuard<Type>&& other) :
            m_function(std::move(other.m_function))
        {
        }

        ScopeGuard<Type>& operator=(ScopeGuard<Type>&& other)
        {
            if(this != &other)
            {
                m_function = std::move(other.m_function);
            }

            return *this;
        }

        ~ScopeGuard()
        {
            m_function();
        }

    private:
        Type m_function;
    };

    template<>
    class ScopeGuard<void> : private NonCopyable
    {
    public:
        class Condition
        {
        public:
            Condition() :
                m_value(true)
            {
            }

            Condition(bool value) :
                m_value(value)
            {
            }

            operator bool()
            {
                return m_value;
            }

        private:
            bool m_value;
        };
    };

    template<typename Type>
    ScopeGuard<Type> MakeScopeGuard(Type function)
    {
        return ScopeGuard<Type>(function);
    }
}

#define SCOPE_GUARD_STRING(line) scopeGuardLine ## line
#define SCOPE_GUARD_NAME(line) SCOPE_GUARD_STRING(line)

#define SCOPE_GUARD_BEGIN(...) auto SCOPE_GUARD_NAME(__LINE__) = Common::MakeScopeGuard([&]() { if(Common::ScopeGuard<void>::Condition(__VA_ARGS__)) { 
#define SCOPE_GUARD_END() } });

#define SCOPE_GUARD_MAKE(code) Common::MakeScopeGuard([&]() { code; })
#define SCOPE_GUARD(code) auto SCOPE_GUARD_NAME(__LINE__) = SCOPE_GUARD_MAKE(code)

#define SCOPE_GUARD_IF_MAKE(condition, code) Common::MakeScopeGuard([&]() { if(condition) { code; } })
#define SCOPE_GUARD_IF(condition, code) auto SCOPE_GUARD_NAME(__LINE__) = SCOPE_GUARD_IF_MAKE(condition, code)
