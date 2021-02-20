/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <functional>
#include <Common/LinkedList.hpp>
#include "Event/Collector.hpp"
#include "Event/Receiver.hpp"
#include "Event/Policies.hpp"

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
        ReturnType Dispatch(Receiver<ReturnType(Arguments...)>* receiver, Arguments&&... arguments)
        {
            ASSERT(receiver != nullptr, "Receiver is nullptr!");
            return receiver->Receive(std::forward<Arguments>(arguments)...);
        }
    };

    template<typename Collector, typename Type>
    class CollectorDispatcher;

    template<class Collector, typename ReturnType, typename... Arguments>
    class CollectorDispatcher<Collector, ReturnType(Arguments...)>
        : public ReceiverInvoker<ReturnType(Arguments...)>
    {
    public:
        void operator()(Collector& collector,
            Receiver<ReturnType(Arguments...)>* receiver, Arguments&&... arguments)
        {
            ASSERT(receiver != nullptr);
            collector.ConsumeResult(this->Dispatch(
                receiver, std::forward<Arguments>(arguments)...));
        }
    };

    template<class Collector, typename... Arguments>
    class CollectorDispatcher<Collector, void(Arguments...)>
        : public ReceiverInvoker<void(Arguments...)>
    {
    public:
        void operator()(Collector& collector,
            Receiver<void(Arguments...)>* receiver, Arguments&&... arguments)
        {
            ASSERT(receiver != nullptr);
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

        void Unsubscribe(ReceiverType& receiver)
        {
            VERIFY(receiver.m_dispatcher == this,
                "Attempting to unsubscribe a receiver that is not subscribed to this dispatcher!");

            receiver.m_listNode.Remove();
            receiver.m_dispatcher = nullptr;
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

        template<typename Collector>
        void Dispatch(Collector& collector, Arguments&&... arguments)
        {
            m_receiverList.ForEach(
                [&collector = collector](ReceiverListNode& node, Arguments&&... arguments)
                {
                    if(collector.ShouldContinue())
                    {
                        ReceiverType* receiver = node.GetReference();
                        ASSERT(receiver != nullptr);

                        CollectorDispatcher<Collector, ReturnType(Arguments...)> invocation;
                        invocation(collector, receiver, std::forward<Arguments>(arguments)...);
                        
                        return true;
                    }

                    return false;
                },
                std::forward<Arguments>(arguments)...
            );
        }

        ReceiverListNode m_receiverList;
    };

    template<typename Type, class Collector = CollectDefault<typename std::function<Type>::result_type>>
    class Dispatcher;

    template<typename Collector, typename ReturnType, typename... Arguments>
    class Dispatcher<ReturnType(Arguments...), Collector> : public DispatcherBase<ReturnType(Arguments...)>
    {
    public:
        using Super = DispatcherBase<ReturnType(Arguments...)>;

        Dispatcher() :
            m_defaultCollector()
        {
        }

        Dispatcher(Collector defaultCollector) :
            m_defaultCollector(defaultCollector)
        {
        }

        Dispatcher(Dispatcher&& other) :
            Dispatcher(other.m_defaultCollector)
        {
            *this = std::move(other);
        }

        Dispatcher& operator=(Dispatcher&& other)
        {
            Super::operator=(std::move(other));
            std::swap(m_defaultCollector, other.m_defaultCollector);
            return *this;
        }

        ReturnType Dispatch(Arguments... arguments)
        {
            Collector collector(m_defaultCollector);
            Super::template Dispatch<Collector>(collector, std::forward<Arguments>(arguments)...);
            return collector.GetResult();
        }

        ReturnType operator()(Arguments... arguments)
        {
            return Dispatch(std::forward<Arguments>(arguments)...);
        }

    private:
        Collector m_defaultCollector;
    };

    template<typename Collector, typename... Arguments>
    class Dispatcher<void(Arguments...), Collector> : public DispatcherBase<void(Arguments...)>
    {
    public:
        using Super = DispatcherBase<void(Arguments...)>;

        Dispatcher() = default;
        Dispatcher(Dispatcher&& other) = default;
        Dispatcher& operator=(Dispatcher&& other) = default;

        void Dispatch(Arguments... arguments)
        {
            CollectDefault<void> collector;
            Super::template Dispatch<Collector>(collector, std::forward<Arguments>(arguments)...);
        }

        void operator()(Arguments... arguments)
        {
            return Dispatch(std::forward<Arguments>(arguments)...);
        }
    };
}
