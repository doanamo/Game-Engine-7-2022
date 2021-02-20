/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Collector
    
    Utility template classes for storing return values of multiple receiver
    invocations. Returns boolean on call that indicates the final result of
    multiple invocations. Some collectors may terminate further receiver
    invocations on specific return values.
*/

namespace Event
{
    template<typename ReturnType>
    class CollectLast;

    template<typename Type>
    class CollectDefault : public CollectLast<Type>
    {
    public:
        CollectDefault(Type defaultResult) :
            CollectLast<Type>(defaultResult)
        {
        }
    };

    template<>
    class CollectDefault<void>
    {
    public:
        CollectDefault(void)
        {
        }

        void ConsumeResult(void)
        {
        }

        bool ShouldContinue() const
        {
            return true;
        }

        void GetResult() const
        {
        }
    };

    template<typename ReturnType>
    class CollectLast
    {
    public:
        CollectLast(ReturnType initialResult) :
            m_result(initialResult)
        {
        }

        void ConsumeResult(ReturnType result)
        {
            m_result = result;
        }

        bool ShouldContinue() const
        {
            return true;
        }

        ReturnType GetResult() const
        {
            return m_result;
        }

    private:
        ReturnType m_result;
    };

    class CollectWhileTrue
    {
    public:
        CollectWhileTrue(bool initialResult = true) :
            m_result(initialResult)
        {
        }

        void ConsumeResult(bool result)
        {
            m_result = result;
        }

        bool ShouldContinue() const
        {
            return m_result;
        }

        bool GetResult() const
        {
            return m_result;
        }

    private:
        bool m_result;
    };

    class CollectWhileFalse
    {
    public:
        CollectWhileFalse(bool initialResult = false) :
            m_result(initialResult)
        {
        }

        void ConsumeResult(bool result)
        {
            m_result = result;
        }

        bool ShouldContinue() const
        {
            return !m_result;
        }

        bool GetResult() const
        {
            return m_result;
        }

    private:
        bool m_result;
    };
}
