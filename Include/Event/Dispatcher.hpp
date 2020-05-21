/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <functional>
#include <Common/LinkedList.hpp>
#include "Event/Collector.hpp"
#include "Event/Receiver.hpp"

// Forward declarations.
namespace Event
{
    template<typename Type>
    class Receiver;
}

/*
    Dispatcher

    Holds list of subscribed receivers that can be invoked all at once. 
    Safer than using raw delegates as unsubscribing is automated at receiver's
    destruction, so no dangerous dangling pointers are left.

    ASingle dispatcher instance can have multiple receivers subscribed,
    but single receiver can be only subscribed to one dispatcher.
*/

namespace Event
{
    template<typename Type>
    class ReceiverInvoker;

    template<typename ReturnType, typename... Arguments>
    class ReceiverInvoker<ReturnType(Arguments...)>
    {
    protected:
        ReturnType Dispatch(Receiver<ReturnType(Arguments...)>* receiver, Arguments... arguments)
        {
            ASSERT(receiver != nullptr, "Receiver is nullptr!");
            return receiver->Receive(std::forward<Arguments>(arguments)...);
        }
    };

    template<typename Collector, typename Type>
    class CollectorDispatcher;

    template<class Collector, typename ReturnType, typename... Arguments>
    class CollectorDispatcher<Collector, ReturnType(Arguments...)> : public ReceiverInvoker<ReturnType(Arguments...)>
    {
    public:
        void operator()(Collector& collector, Receiver<ReturnType(Arguments...)>* receiver, Arguments... arguments)
        {
            ASSERT(receiver != nullptr, "Receiver is nullptr!");
            collector.ConsumeResult(this->Dispatch(receiver, std::forward<Arguments>(arguments)...));
        }
    };

    template<class Collector, typename... Arguments>
    class CollectorDispatcher<Collector, void(Arguments...)> : public ReceiverInvoker<void(Arguments...)>
    {
    public:
        void operator()(Collector& collector, Receiver<void(Arguments...)>* receiver, Arguments... arguments)
        {
            ASSERT(receiver != nullptr, "Receiver is nullptr!");
            this->Dispatch(receiver, std::forward<Arguments>(arguments)...);
        }
    };

    // Does not allow dispatching/invoking receivers, allowing
    // dispatcher instance to be safely passed as reference.
    template<typename Type>
    class DispatcherBase;

    template<typename ReturnType, typename... Arguments>
    class DispatcherBase<ReturnType(Arguments...)> : private Common::NonCopyable
    {
    public:
        using ReceiverReturnType = ReturnType;
        using ReceiverListNode = Common::ListNode<Receiver<ReturnType(Arguments...)>>;

    protected:
        // Can only be constructed via Dispatcher template class.
        DispatcherBase() = default;

        // Virtual destructor for derived Dispatcher template class.
        virtual ~DispatcherBase()
        {
            this->UnsubscribeAll();
        }

        DispatcherBase(DispatcherBase&& other) :
            DispatcherBase()
        {
            // Invoke move operation.
            *this = std::move(other);
        }

        DispatcherBase& operator=(DispatcherBase&& other)
        {
            // Swap class members.
            std::swap(this->m_receiverList, other.m_receiverList);

            // Fix pointers of subscribed receivers.
            ReceiverListNode* iterator = this->m_receiverList.GetNext();

            while(iterator != &this->m_receiverList)
            {
                // Retrieve receiver instance.
                Receiver<ReturnType(Arguments...)>* receiver = iterator->GetReference();
                ASSERT(receiver != nullptr, "Retrieved receiver is nullptr!");

                // Assign new dispatcher reference
                receiver->m_dispatcher = this;

                // Move to next receiver.
                iterator = iterator->GetNext();
            }

            ReceiverListNode* otherIterator = other.m_receiverList.GetNext();

            while(otherIterator != &other.m_receiverList)
            {
                // Retrieve receiver instance.
                Receiver<ReturnType(Arguments...)>* receiver = otherIterator->GetReference();
                ASSERT(receiver != nullptr, "Retrieved receiver is nullptr!");

                // Assign new dispatcher reference
                receiver->m_dispatcher = &other;

                // Move to next receiver.
                otherIterator = otherIterator->GetNext();
            }

            return *this;
        }

    public:
        // By default we do not want to replace receiver's current dispatcher.
        bool Subscribe(Receiver<ReturnType(Arguments...)>& receiver, bool unsubscribeReceiver = false, bool insertFront = false)
        {
            // Check if receiver is already subscribed somewhere else.
            if(!receiver.m_listNode.IsFree())
            {
                // Check if receiver is already subscribed to this dispatcher.
                if(receiver.m_dispatcher == this)
                    return true;

                // Check if we want to replace receiver's dispatcher
                if(!unsubscribeReceiver)
                    return false;

                // Unsubscribe first and then continue.
                receiver.Unsubscribe();
            }

            // Add receiver to linked list.
            if(insertFront)
            {
                // Add receiver node to the beginning of receiver list.
                receiver.m_listNode.InsertBefore(m_receiverList.GetNext());
            }
            else
            {
                // Add receiver node to the end of receiver list.
                receiver.m_listNode.InsertAfter(m_receiverList.GetPrevious());
            }

            // Set dispatcher reference.
            receiver.m_dispatcher = this;

            return true;
        }

        void Unsubscribe(Receiver<ReturnType(Arguments...)>& receiver)
        {
            // Check if receiver is subscribed to this dispatcher.
            VERIFY(receiver.m_dispatcher == this, "Attempting to unsubscribe a receiver that is not subscribed to this dispatcher!");

            // Remove receiver node.
            receiver.m_listNode.Remove();

            // Clear dispatcher reference.
            receiver.m_dispatcher = nullptr;
        }

        void UnsubscribeAll()
        {
            // Iterate through linked list to unsubscribe all receivers.
            ReceiverListNode* iterator = m_receiverList.GetNext();

            while(iterator != &m_receiverList)
            {
                // Get the receiver instance.
                Receiver<ReturnType(Arguments...)>* receiver = iterator->GetReference();
                ASSERT(receiver != nullptr, "Retrieved receiver is nullptr!");

                // Advance to next receiver.
                // Do this here before receiver node gets removed.
                iterator = iterator->GetNext();

                // Unsubscribe receiver.
                receiver->Unsubscribe();
            }
        }

        bool HasSubscribers() const
        {
            return !m_receiverList.IsFree();
        }

    protected:
        // Invokes receivers with following arguments.
        template<typename Collector>
        void Dispatch(Collector& collector, Arguments... arguments)
        {
            // Send dispatch event to all receivers.
            auto argumentTuple = std::make_tuple(std::forward<Arguments>(arguments)...);

            m_receiverList.ForEach(
                [&collector = collector, argumentTuple = std::move(argumentTuple)](ReceiverListNode& node)
                {
                    // Check is we should continue processing receivers.
                    if(!collector.ShouldContinue())
                        return false;

                    // Retrieve receiver instance.
                    Receiver<ReturnType(Arguments...)>* receiver = node.GetReference();
                    ASSERT(receiver != nullptr, "Retrieved receiver is nullptr!");

                    // Invoke receiver and collect result.
                    auto invokeReceiver = [&collector = collector, receiver](auto... arguments)
                    {
                        CollectorDispatcher<Collector, ReturnType(Arguments...)> invocation;
                        invocation(collector, receiver, std::forward<Arguments>(arguments)...);
                    };

                    std::apply(invokeReceiver, argumentTuple);

                    // Continue iterating.
                    return true;
                }
            );
        }

    protected:
        // Circular linked list of receivers.
        ReceiverListNode m_receiverList;
    };

    template<typename Type, class Collector = CollectDefault<typename std::function<Type>::result_type>>
    class Dispatcher;

    template<typename Collector, typename ReturnType, typename... Arguments>
    class Dispatcher<ReturnType(Arguments...), Collector> : public DispatcherBase<ReturnType(Arguments...)>
    {
    public:
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
            // Invoke move operation.
            *this = std::move(other);
        }

        Dispatcher& operator=(Dispatcher&& other)
        {
            // Swap class members.
            DispatcherBase<ReturnType(Arguments...)>::operator=(std::move(other));
            std::swap(this->m_defaultCollector, other.m_defaultCollector);
            return *this;
        }

        // Invokes receivers with following arguments.
        ReturnType Dispatch(Arguments... arguments)
        {
            // Create result collector.
            Collector collector(m_defaultCollector);

            // Dispatch to receivers.
            DispatcherBase<ReturnType(Arguments...)>::template
                Dispatch<Collector>(collector, std::forward<Arguments>(arguments)...);

            // Return collected result.
            return collector.GetResult();
        }

        // Overloaded call operator that is used as dispatch.
        ReturnType operator()(Arguments... arguments)
        {
            return this->Dispatch(std::forward<Arguments>(arguments)...);
        }

    private:
        // Default collector value for returns.
        Collector m_defaultCollector;
    };

    template<typename Collector, typename... Arguments>
    class Dispatcher<void(Arguments...), Collector> : public DispatcherBase<void(Arguments...)>
    {
    public:
        Dispatcher() = default;

        Dispatcher(Dispatcher&& other) = default;
        Dispatcher& operator=(Dispatcher&& other) = default;

        // Invokes receivers with following arguments.
        void Dispatch(Arguments... arguments)
        {
            // Create default collector for void type.
            CollectDefault<void> collector;

            // Dispatch to receivers.
            DispatcherBase<void(Arguments...)>::template
                Dispatch<Collector>(collector, std::forward<Arguments>(arguments)...);
        }

        // Overloaded call operator that is used as a dispatch.
        void operator()(Arguments... arguments)
        {
            return this->Dispatch(std::forward<Arguments>(arguments)...);
        }
    };
}
