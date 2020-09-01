/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Common/Debug.hpp"
#include "Event/Dispatcher.hpp"
#include "Event/Delegate.hpp"
#include "Event/Policies.hpp"

namespace Event
{
    template<typename Type>
    class DispatcherBase;

    template<typename Type>
    class ReceiverInvoker;
}

/*
    Receiver

    Invokes delegate after receiving signal from dispatcher.
    Single receiver instance can be subscribed to only one dispatcher.

    See Dispatcher template class for more information.
*/

namespace Event
{
    template<typename Type>
    class Receiver;

    template<typename ReturnType, typename... Arguments>
    class Receiver<ReturnType(Arguments...)> : public Delegate<ReturnType(Arguments...)>, private Common::NonCopyable
    {
    public:
        friend DispatcherBase<ReturnType(Arguments...)>;
        friend ReceiverInvoker<ReturnType(Arguments...)>;

        using DispatcherType = DispatcherBase<ReturnType(Arguments...)>;
        using ReceiverListNode = typename DispatcherType::ReceiverListNode;

        Receiver() :
            m_listNode(this)
        {
        }

        virtual ~Receiver()
        {
            Unsubscribe();
        }

        Receiver(Receiver&& other) :
            Receiver()
        {
            *this = std::move(other);
        }

        Receiver& operator=(Receiver&& other)
        {
            // Swap only subscription and leave bound function as is.
            // Swapping list nodes does not require references to be reset.

            std::swap(m_listNode, other.m_listNode);
            std::swap(m_dispatcher, other.m_dispatcher);

            return *this;
        }

        bool Subscribe(DispatcherBase<ReturnType(Arguments...)>& dispatcher, 
            SubscriptionPolicy subscriptionPolicy = SubscriptionPolicy::ReplaceSubscription,
            PriorityPolicy priorityPolicy = PriorityPolicy::InsertBack)
        {
            return dispatcher.Subscribe(*this, subscriptionPolicy, priorityPolicy);
        }

        void Unsubscribe()
        {
            if(m_dispatcher != nullptr)
            {
                m_dispatcher->Unsubscribe(*this);
            }

            ASSERT(m_dispatcher == nullptr, "Dispatcher did not unsubscribe this receiver properly!");
            ASSERT(m_listNode.IsFree(), "Dispatcher did not unsubscribe this receiver properly!");
        }

        bool IsSubscribed() const
        {
            return !m_listNode.IsFree();
        }

    private:
        ReturnType Receive(Arguments&&... arguments)
        {
            ASSERT(m_dispatcher, "Invoked receiver without it being subscribed!");
            return Invoke(std::forward<Arguments>(arguments)...);
        }

        ReturnType Invoke(Arguments&&... arguments)
        {
            return Delegate<ReturnType(Arguments...)>::Invoke(std::forward<Arguments>(arguments)...);
        }

        ReceiverListNode m_listNode;
        DispatcherType* m_dispatcher = nullptr;
    };
}
