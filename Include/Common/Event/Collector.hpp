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
    template<typename ResultType>
    class Collector
    {
    public:
        Collector(ResultType defaultResult) :
            m_defaultResult(defaultResult)
        {
        }

        virtual ~Collector() = default;

        virtual void Reset()
        {
            ConsumeResult(m_defaultResult);
        }

        virtual void ConsumeResult(ResultType result) = 0;
        virtual bool ShouldContinue() const = 0;
        virtual ResultType GetResult() const = 0;

    private:
        ResultType m_defaultResult;
    };

    template<>
    class Collector<void>
    {
    public:
        Collector() = default;
        virtual ~Collector() = default;

        bool ShouldContinue() const
        {
            return true;
        }
    };

    using CollectNothing = Collector<void>;

    template<typename ResultType>
    class CollectLast : public Collector<ResultType>
    {
    public:
        CollectLast(ResultType defaultResult) :
            Collector(defaultResult),
            m_result(defaultResult)
        {
        }

        void ConsumeResult(ResultType result) override
        {
            m_result = result;
        }

        bool ShouldContinue() const override
        {
            return true;
        }

        ResultType GetResult() const override
        {
            return std::move(m_result);
        }

    private:
        ResultType m_result;
    };

    class CollectWhileTrue : public Collector<bool>
    {
    public:
        CollectWhileTrue(bool defaultResult = true) :
            Collector(defaultResult),
            m_result(defaultResult)
        {
        }

        void ConsumeResult(bool result) override
        {
            m_result = result;
        }

        bool ShouldContinue() const override
        {
            return m_result;
        }

        bool GetResult() const override
        {
            return m_result;
        }

    private:
        bool m_result;
    };

    class CollectWhileFalse : public Collector<bool>
    {
    public:
        CollectWhileFalse(bool defaultResult = false) :
            Collector(defaultResult),
            m_result(defaultResult)
        {
        }

        void ConsumeResult(bool result) override
        {
            m_result = result;
        }

        bool ShouldContinue() const override
        {
            return !m_result;
        }

        bool GetResult() const override
        {
            return m_result;
        }

    private:
        bool m_result;
    };
}
