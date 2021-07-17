/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <functional>
#include "Common/LinkedList.hpp"
#include "Common/Event/EventCollector.hpp"
#include "Common/Event/EventReceiver.hpp"
#include "Common/Event/EventPolicies.hpp"

namespace Event
{
    template<typename Type>
    class Receiver;
}

/*
    Dispatcher

    Holds list of subscribed receivers that can be invoked in sequence. 
    Safer than using raw delegates as unsubscribing is automatically
    invoked on destruction, so no dangerous dangling pointers are left.

    Single dispatcher instance can have multiple receivers subscribed,
    but single receiver can be only subscribed to one dispatcher.

    DispatcherBase type does not allow dispatching/invoking receivers,
    enabling dispatcher instance to be safely passed as a reference
    for subscription only purposes.
*/

namespace Event
{
    template<typename Type>
    class ReceiverInvoker;

    template<typename ReturnType, typename... Arguments>
    class ReceiverInvoker<ReturnType(Arguments...)>
    {
    protected:
        ReturnType Dispatch(Receiver<ReturnType(Arguments...)>& receiver, Arguments&&... arguments)
        {
            return receiver.Receive(std::forward<Arguments>(arguments)...);
        }
    };

    template<typename Type>
    class CollectorDispatcher;

    template<typename ReturnType, typename... Arguments>
    class CollectorDispatcher<ReturnType(Arguments...)>
        : public ReceiverInvoker<ReturnType(Arguments...)>
    {
    public:
        void operator()(Collector<ReturnType>& collector,
            Receiver<ReturnType(Arguments...)>& receiver, Arguments&&... arguments)
        {
            collector.ConsumeResult(this->Dispatch(
                receiver, std::forward<Arguments>(arguments)...));
        }
    };

    template<typename... Arguments>
    class CollectorDispatcher<void(Arguments...)>
        : public ReceiverInvoker<void(Arguments...)>
    {
    public:
        void operator()(Collector<void>& collector,
            Receiver<void(Arguments...)>& receiver, Arguments&&... arguments)
        {
            this->Dispatch(receiver, std::forward<Arguments>(arguments)...);
        }
    };

    template<typename Type>
    class DispatcherBase;

    template<typename ReturnType, typename... Arguments>
    class DispatcherBase<ReturnType(Arguments...)> : private Common::NonCopyable
    {
    public:
        using ReceiverType = Receiver<ReturnType(Arguments...)>;
        using ReceiverListNode = Common::ListNode<ReceiverType>;
        using ReceiverReturnType = ReturnType;

        bool Subscribe(ReceiverType& receiver,
            SubscriptionPolicy subscriptionPolicy = SubscriptionPolicy::RetainSubscription,
            PriorityPolicy priorityPolicy = PriorityPolicy::InsertBack)
        {
            if(receiver.IsSubscribed())
            {
                if(receiver.m_dispatcher == this)
                    return true;

                if(subscriptionPolicy == SubscriptionPolicy::RetainSubscription)
                    return false;

                receiver.Unsubscribe();
            }

            if(priorityPolicy == PriorityPolicy::InsertFront)
            {
                receiver.m_listNode.InsertBefore(m_receiverList.GetNext());
            }
            else
            {
                receiver.m_listNode.InsertAfter(m_receiverList.GetPrevious());
            }

            receiver.m_dispatcher = this;
            return true;
        }

        bool Unsubscribe(ReceiverType& receiver)
        {
            if(receiver.m_dispatcher != this)
                return false;

            receiver.m_listNode.Remove();
            receiver.m_dispatcher = nullptr;

            return true;
        }

        void UnsubscribeAll()
        {
            ReceiverListNode* iterator = m_receiverList.GetNext();
            while(iterator != &m_receiverList)
            {
                ReceiverType* receiver = iterator->GetReference();
                ASSERT(receiver != nullptr);

                iterator = iterator->GetNext();
                receiver->Unsubscribe();
            }
        }

        bool HasSubscribers() const
        {
            return !m_receiverList.IsFree();
        }

    protected:
        DispatcherBase() = default;

        virtual ~DispatcherBase()
        {
            UnsubscribeAll();
        }

        DispatcherBase(DispatcherBase&& other) :
            DispatcherBase()
        {
            *this = std::move(other);
        }

        DispatcherBase& operator=(DispatcherBase&& other)
        {
            std::swap(m_receiverList, other.m_receiverList);

            ReceiverListNode* iterator = m_receiverList.GetNext();
            while(iterator != &m_receiverList)
            {
                ReceiverType* receiver = iterator->GetReference();
                ASSERT(receiver != nullptr);

                receiver->m_dispatcher = this;
                iterator = iterator->GetNext();
            }

            ReceiverListNode* otherIterator = other.m_receiverList.GetNext();
            while(otherIterator != &other.m_receiverList)
            {
                ReceiverType* receiver = otherIterator->GetReference();
                ASSERT(receiver != nullptr);

                receiver->m_dispatcher = &other;
                otherIterator = otherIterator->GetNext();
            }

            return *this;
        }

        void Dispatch(Collector<ReturnType>& collector, Arguments&&... arguments)
        {
            m_receiverList.ForEach(
                [&collector](ReceiverListNode& node, Arguments&&... arguments)
                {
                    if(collector.ShouldContinue())
                    {
                        ReceiverType* receiver = node.GetReference();
                        ASSERT(receiver != nullptr);

                        if(receiver->IsBound())
                        {
                            CollectorDispatcher<ReturnType(Arguments...)> invocation;
                            invocation(collector, *receiver, std::forward<Arguments>(arguments)...);
                        }

                        return true;
                    }

                    return false;
                },
                std::forward<Arguments>(arguments)...
            );
        }

        ReceiverListNode m_receiverList;
    };

    template<typename Type>
    class Dispatcher;

    template<typename ReturnType, typename... Arguments>
    class Dispatcher<ReturnType(Arguments...)> final :
        public DispatcherBase<ReturnType(Arguments...)>
    {
    public:
        using Super = DispatcherBase<ReturnType(Arguments...)>;

        Dispatcher(ReturnType defaultResult = ReturnType()) :
            m_collector(std::make_unique<CollectDefault<ReturnType>>(defaultResult))
        {
        }

        Dispatcher(std::unique_ptr<Collector<ReturnType>>&& collector = nullptr) :
            m_collector(std::move(collector))
        {
            if(m_collector == nullptr)
            {
                m_collector = std::make_unique<CollectDefault<ReturnType>>();
            }
        }

        Dispatcher(Dispatcher&& other) :
            Dispatcher(other.m_collector)
        {
            *this = std::move(other);
        }

        Dispatcher& operator=(Dispatcher&& other)
        {
            Super::operator=(std::move(other));
            std::swap(m_collector, other.m_collector);
            return *this;
        }

        ReturnType Dispatch(Arguments... arguments)
        {
            ASSERT(m_collector != nullptr);

            m_collector->Reset();
            Super::Dispatch(*m_collector, std::forward<Arguments>(arguments)...);
            return m_collector->GetResult();
        }

        ReturnType operator()(Arguments... arguments)
        {
            return Dispatch(std::forward<Arguments>(arguments)...);
        }

    private:
        std::unique_ptr<Collector<ReturnType>> m_collector;
    };

    template<typename... Arguments>
    class Dispatcher<void(Arguments...)> final : public DispatcherBase<void(Arguments...)>
    {
    public:
        using Super = DispatcherBase<void(Arguments...)>;

        Dispatcher() = default;
        Dispatcher(Dispatcher&& other) = default;
        Dispatcher& operator=(Dispatcher&& other) = default;

        Dispatcher(std::unique_ptr<Collector<void>>&& collector)
        {
        }

        void Dispatch(Arguments... arguments)
        {
            CollectNothing collector;
            Super::Dispatch(collector, std::forward<Arguments>(arguments)...);
        }

        void operator()(Arguments... arguments)
        {
            return Dispatch(std::forward<Arguments>(arguments)...);
        }
    };
}
