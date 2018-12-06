/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <functional>
#include "Common/LinkedList.hpp"
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

    Holds a list of subscribed receivers that can be invoked all at once. 
    Safer than using raw delegates as unsubscribing is automated at receiver's
    destruction, so no dangerous dangling pointers are left.

    A single dispatcher instance can have multiple receivers subscribed,
    but a single receiver can be only subscribed to one dispatcher.

    void ExampleDispatcher()
    {
        // Create a class instance that defines following methods:
        // void Class::FunctionA(const EventData& event) { ... }
        // void Class::FunctionB(const EventData& event) { ... }
        Class instance;
    
        // Create event receivers.
        Receiver<void(const EventData&)> receiverA;
        receiverA.Bind<Class, &Class::FunctionA>(&instance);
    
        Receiver<void(const EventData&)> receiverB;
        receiverB.Bind<Class, &Class::FunctionB>(&instance);
    
        // Subscribe event receivers.
        Dispatcher<void(const EventData&)> dispatcher(defaultResult);
        dispatcher.Subscribe(receiverA);
        dispatcher.Subscribe(receiverB);

        // Dispatch an event to receivers.
        dispatcher.Dispatch(EventData(...));
    }
*/

namespace Event
{
    // Receiver invoker template class helper.
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

    // Collector dispatcher template class helper.
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

    // Dispatcher base template class.
    // Does not allow dispatching/invoking receivers, allowing
    // a dispatcher instance to be safely passed as a reference.
    template<typename Type>
    class DispatcherBase;

    template<typename ReturnType, typename... Arguments>
    class DispatcherBase<ReturnType(Arguments...)>
    {
    public:
        // Type declarations.
        using ReceiverListNode = Common::ListNode<Receiver<ReturnType(Arguments...)>>;

    protected:
        // Can only be constructed via Dispatcher template class.
        DispatcherBase() = default;

        // Virtual destructor for derived Dispatcher template class.
        virtual ~DispatcherBase()
        {
            this->UnsubscribeAll();
        }

        // Disallow copying operations.
        DispatcherBase(const DispatcherBase& other) = delete;
        DispatcherBase& operator=(const DispatcherBase& other) = delete;

        // Move constructor.
        DispatcherBase(DispatcherBase&& other) :
            DispatcherBase()
        {
            // Invoke a move operation.
            *this = std::move(other);
        }

        // Move assignment operator
        DispatcherBase& operator=(DispatcherBase&& other)
        {
            // Swap class members.
            std::swap(this->m_receiverList, other.m_receiverList);

            // Fix pointers of subscribed receivers.
            ReceiverListNode* iterator = this->m_receiverList.GetNext();

            while(iterator != &this->m_receiverList)
            {
                // Retrieve a receiver instance.
                Receiver<ReturnType(Arguments...)>* receiver = iterator->GetReference();
                ASSERT(receiver != nullptr, "Retrieved receiver is nullptr!");

                // Assign a new dispatcher reference
                receiver->m_dispatcher = this;

                // Move to the next receiver.
                iterator = iterator->GetNext();
            }

            ReceiverListNode* otherIterator = other.m_receiverList.GetNext();

            while(otherIterator != &other.m_receiverList)
            {
                // Retrieve a receiver instance.
                Receiver<ReturnType(Arguments...)>* receiver = otherIterator->GetReference();
                ASSERT(receiver != nullptr, "Retrieved receiver is nullptr!");

                // Assign a new dispatcher reference
                receiver->m_dispatcher = &other;

                // Move to the next receiver.
                otherIterator = otherIterator->GetNext();
            }

            return *this;
        }

    public:
        // Subscribes a receiver.
        // By default we do not want to replace receiver's current dispatcher.
        bool Subscribe(Receiver<ReturnType(Arguments...)>& receiver,
            bool unsubscribeReceiver = false, bool insertFront = false)
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

            // Add receiver to the linked list.
            if(insertFront)
            {
                // Add receiver node to the beginning of the receiver list.
                receiver.m_listNode.InsertBefore(m_receiverList.GetNext());
            }
            else
            {
                // Add receiver node to the end of the receiver list.
                receiver.m_listNode.InsertAfter(m_receiverList.GetPrevious());
            }

            // Set dispatcher reference.
            receiver.m_dispatcher = this;

            return true;
        }

        // Unsubscribes a receiver.
        void Unsubscribe(Receiver<ReturnType(Arguments...)>& receiver)
        {
            // Check if receiver is subscribed to this dispatcher.
            VERIFY(receiver.m_dispatcher == this, "Attempting to unsubscribe a receiver that is not subscribed to this dispatcher!");

            // Remove receiver node.
            receiver.m_listNode.Remove();

            // Clear dispatcher reference.
            receiver.m_dispatcher = nullptr;
        }

        // Unsubscribes all receivers.
        void UnsubscribeAll()
        {
            // Iterate through the linked list to unsubscribe all receivers.
            ReceiverListNode* iterator = m_receiverList.GetNext();

            while(iterator != &m_receiverList)
            {
                // Get the receiver instance.
                Receiver<ReturnType(Arguments...)>* receiver = iterator->GetReference();
                ASSERT(receiver != nullptr, "Retrieved receiver is nullptr!");

                // Advance to the next receiver.
                // Do this here before receiver node gets removed.
                iterator = iterator->GetNext();

                // Unsubscribe a receiver.
                receiver->Unsubscribe();
            }
        }

        // Checks if the dispatcher has any subscribers.
        bool HasSubscribers() const
        {
            return !m_receiverList.IsFree();
        }

    protected:
        // Invokes receivers with following arguments.
        template<typename Collector>
        void Dispatch(Collector& collector, Arguments... arguments)
        {
            // Send a dispatch event to all receivers.
            ReceiverListNode* iterator = m_receiverList.GetNext();

            while(iterator != &m_receiverList)
            {
                // Retrieve receiver instance.
                Receiver<ReturnType(Arguments...)>* receiver = iterator->GetReference();
                ASSERT(receiver != nullptr, "Retrieved receiver is nullptr!");

                // Check is we should continue processing receivers.
                if(!collector.ShouldContinue())
                    break;

                // Cache the next receiver, as we may lose the current iterator if it suddenly gets removed.
                // This may happen when we unsubscribe the current receiver in an invoked function.
                ReceiverListNode* nextIterator = iterator->GetNext();

                // Invoke a receiver and collect the result.
                CollectorDispatcher<Collector, ReturnType(Arguments...)> invocation;
                invocation(collector, receiver, std::forward<Arguments>(arguments)...);

                // Advance to the next receiver.
                iterator = nextIterator;
            }
        }

    protected:
        // Circular linked list of receivers.
        ReceiverListNode m_receiverList;
    };

    // Dispatcher template class.
    template<typename Type, class Collector = CollectDefault<typename std::function<Type>::result_type>>
    class Dispatcher;

    template<typename Collector, typename ReturnType, typename... Arguments>
    class Dispatcher<ReturnType(Arguments...), Collector> : public DispatcherBase<ReturnType(Arguments...)>
    {
    public:
        // Default constructor.
        Dispatcher() :
            m_defaultCollector()
        {
        }

        Dispatcher(Collector defaultCollector) :
            m_defaultCollector(defaultCollector)
        {
        }

        // Move constructor.
        Dispatcher(Dispatcher&& other) :
            Dispatcher(other.m_defaultCollector)
        {
            // Invoke a move operation.
            *this = std::move(other);
        }

        // Move assignment operator.
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
            // Create a result collector.
            Collector collector(m_defaultCollector);

            // Dispatch to receivers.
            DispatcherBase<ReturnType(Arguments...)>::template
                Dispatch<Collector>(collector, std::forward<Arguments>(arguments)...);

            // Return collected result.
            return collector.GetResult();
        }

        // Overloaded call operator that is used as a dispatch.
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
        // Default constructor.
        Dispatcher() = default;

        // Allow move operations.
        Dispatcher(Dispatcher&& other) = default;
        Dispatcher& operator=(Dispatcher&& other) = default;

        // Invokes receivers with following arguments.
        void Dispatch(Arguments... arguments)
        {
            // Create default collector for void type.
            CollectDefault<void> collector;

            // Dispatch to receivers.
            DispatcherBase<void(Arguments...)>::template Dispatch<Collector>(collector, std::forward<Arguments>(arguments)...);
        }

        // Overloaded call operator that is used as a dispatch.
        void operator()(Arguments... arguments)
        {
            return this->Dispatch(std::forward<Arguments>(arguments)...);
        }
    };
}
