/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Collector
    
    Utility template classes for storing return values of multiple receiver invocations.
    Returns boolean on call that indicates the final result of multiple invocations.
    Some collector rules may terminate further receiver invocations on specific returns.
*/

namespace Event
{
    // Default collector.
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

    // Default collector specialized for dealing with void return type.
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

    // Collector that returns the result of the last receiver invocation.
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

    // Collector that continues dispatcher propagation while receiver invocations return true.
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

    // Collector that continues dispatcher propagation while receiver invocations return false.
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
