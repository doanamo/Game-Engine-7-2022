/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Common/NonCopyable.hpp"

/*
    Scope Guard

    Executes bound lambda function at the end of current scope.
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

    template<typename Type>
    ScopeGuard<Type> MakeScopeGuard(Type function)
    {
        return ScopeGuard<Type>(function);
    }
}

/*
    Macro Helpers
*/

#define SCOPE_GUARD_STRING(line) scopeGuardLine ## line
#define SCOPE_GUARD_NAME(line) SCOPE_GUARD_STRING(line)

#define SCOPE_GUARD_VARIABLE auto SCOPE_GUARD_NAME(__LINE__)
#define SCOPE_GUARD SCOPE_GUARD_VARIABLE = Common::MakeScopeGuard
