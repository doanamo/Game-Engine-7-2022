/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Common/Debug.hpp"
#include "Event/Dispatcher.hpp"
#include "Event/Delegate.hpp"

namespace Event
{
    template<typename Type>
    class DispatcherBase;

    template<typename Type>
    class ReceiverInvoker;
}

/*
    Receiver

    Invokes a delegate after receiving a signal from a dispatcher.
    Single receiver instance can be subscribed to only one dispatcher.

    See Dispatcher template class for more information.
*/

namespace Event
{
    template<typename Type>
    class Receiver;

    template<typename ReturnType, typename... Arguments>
    class Receiver<ReturnType(Arguments...)> : public Delegate<ReturnType(Arguments...)>, private NonCopyable
    {
    public:
        friend DispatcherBase<ReturnType(Arguments...)>;
        friend ReceiverInvoker<ReturnType(Arguments...)>;
        using ReceiverListNode = typename DispatcherBase<ReturnType(Arguments...)>::ReceiverListNode;

    public:
        Receiver()
        {
            // Set a strong reference to this node.
            m_listNode.SetReference(this);
        }

        virtual ~Receiver()
        {
            // Unsubscribe from the dispatcher.
            this->Unsubscribe();
        }

        Receiver(Receiver&& other) :
            Receiver()
        {
            // Call the assignment operator.
            *this = std::move(other);
        }

        Receiver& operator=(Receiver&& other)
        {
            // Swap class members.
            // We do not move base delegate class on purpose,
            // as we want bound function to persists after move.
            std::swap(m_listNode, other.m_listNode);
            std::swap(m_dispatcher, other.m_dispatcher);
 
            return *this;
        }

        bool Subscribe(DispatcherBase<ReturnType(Arguments...)>& dispatcher, 
            bool unsubscribeReceiver = true, bool insertFront = false)
        {
            return dispatcher.Subscribe(*this, unsubscribeReceiver, insertFront);
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

    private:
        // Receives an event and invokes bound function.
        ReturnType Receive(Arguments... arguments)
        {
            ASSERT(m_dispatcher, "Invoked a receiver without it being subscribed!");
            return this->Invoke(std::forward<Arguments>(arguments)...);
        }

        // Make derived invoke method private.
        // We do not want other classes calling this.
        ReturnType Invoke(Arguments... arguments)
        {
            return Delegate<ReturnType(Arguments...)>::Invoke(std::forward<Arguments>(arguments)...);
        }

    private:
        ReceiverListNode m_listNode;
        DispatcherBase<ReturnType(Arguments...)>* m_dispatcher = nullptr;
    };
}
