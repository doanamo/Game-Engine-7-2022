/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Collector
    
    Utility template classes for storing return values of multiple receiver invocations.
    Returns boolean on a call that indicates the final result of multiple invocations.
    Some collector rules may terminate further receiver invocations on specific returns.

    void ExampleCollector()
    {
        // Definition of a dispatcher that will keep invoking receivers
        // as long as they keep returning true as the result.
        Dispatcher<bool(void), CollectWhileTrue<bool>> dispatcher(defaultResult);

        // Returns true if all receivers returned true.
        // Returns false when the first receiver returns false.
        bool result = dispatcher.Dispatch();
    }
*/

namespace Common
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

        bool ShouldContinue()
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

        bool ShouldContinue()
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
    template<typename ReturnType = bool>
    class CollectWhileTrue
    {
    public:
        CollectWhileTrue(ReturnType initialResult) :
            m_result(initialResult)
        {
        }

        void ConsumeResult(ReturnType result)
        {
            m_result = result;
        }

        bool ShouldContinue()
        {
            return m_result;
        }

        ReturnType GetResult() const
        {
            return m_result;
        }

    private:
        ReturnType m_result;
    };

    // Collector that continues dispatcher propagation while receiver invocations return false.
    template<typename ReturnType = bool>
    class CollectWhileFalse
    {
    public:
    public:
        CollectWhileFalse(ReturnType initialResult) :
            m_result(initialResult)
        {
        }

        void ConsumeResult(ReturnType result)
        {
            m_result = result;
        }

        bool ShouldContinue()
        {
            return !m_result;
        }

        ReturnType GetResult() const
        {
            return m_result;
        }

    private:
        ReturnType m_result;
    };
}
