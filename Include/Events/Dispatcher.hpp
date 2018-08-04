/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <functional>
#include "Common/Debug.hpp"
#include "Events/Collector.hpp"
#include "Events/Receiver.hpp"

// Forward declarations.
namespace Common
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

namespace Common
{
    // Dispatcher base template class.
    // Does not allow dispatching/invoking receivers, allowing
    // a dispatcher instance to be safely passed as a reference.
    template<typename Type>
    class DispatcherBase;

    template<typename ReturnType, typename... Arguments>
    class DispatcherBase<ReturnType(Arguments...)>
    {
    protected:
        // Can only be constructed via Dispatcher template class.
        DispatcherBase();

        // Virtual destructor for derived Dispatcher template class.
        virtual ~DispatcherBase();

    public:
        // Subscribes a receiver.
        // By default we do not want to replace receiver's current dispatcher.
        bool Subscribe(Receiver<ReturnType(Arguments...)>& receiver, bool unsubscribeReceiver = false);

        // Unsubscribes a receiver.
        void Unsubscribe(Receiver<ReturnType(Arguments...)>& receiver);

        // Unsubscribes all receivers.
        void UnsubscribeAll();

        // Checks if the dispatcher has any subscribers.
        bool HasSubscribers() const;

    protected:
        // Invokes receivers with following arguments.
        template<typename Collector>
        void Dispatch(Collector& collector, Arguments... arguments);

    private:
        // Double linked list of receivers.
        Receiver<ReturnType(Arguments...)>* m_begin;
        Receiver<ReturnType(Arguments...)>* m_end;
    };

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

    // Dispatcher template class.
    template<typename Type, class Collector = CollectDefault<typename std::function<Type>::result_type>>
    class Dispatcher;

    template<typename Collector, typename ReturnType, typename... Arguments>
    class Dispatcher<ReturnType(Arguments...), Collector> : public DispatcherBase<ReturnType(Arguments...)>
    {
    public:
        // Default constructor.
        Dispatcher(ReturnType defaultResult);

        // Disallow copying operations.
        Dispatcher(const Dispatcher& other) = delete;
        Dispatcher& operator=(const Dispatcher& other) = delete;

        // Move operations.
        Dispatcher(Dispatcher&&);
        Dispatcher& operator=(Dispatcher&&);

        // Invokes receivers with following arguments.
        ReturnType Dispatch(Arguments... arguments);

        // Overloaded call operator that is used as a dispatch.
        ReturnType operator()(Arguments... arguments);

    private:
        // Default collector value for returns.
        ReturnType m_defaultResult;
    };

    template<typename Collector, typename... Arguments>
    class Dispatcher<void(Arguments...), Collector> : public DispatcherBase<void(Arguments...)>
    {
    public:
        // Default constructor.
        Dispatcher();

        // Invokes receivers with following arguments.
        void Dispatch(Arguments... arguments);

        // Overloaded call operator that is used as a dispatch.
        void operator()(Arguments... arguments);
    };

    // Template definitions.
    template<typename ReturnType, typename... Arguments>
    DispatcherBase<ReturnType(Arguments...)>::DispatcherBase() :
        m_begin(nullptr),
        m_end(nullptr)
    {
    }

    template<typename ReturnType, typename... Arguments>
    DispatcherBase<ReturnType(Arguments...)>::~DispatcherBase()
    {
        this->UnsubscribeAll();
    }

    template<typename ReturnType, typename... Arguments>
    void DispatcherBase<ReturnType(Arguments...)>::UnsubscribeAll()
    {
        // Iterate through the double linked list to unsubscribe all receivers.
        Receiver<ReturnType(Arguments...)>* iterator = m_begin;

        while(iterator != nullptr)
        {
            // Get the receiver instance.
            Receiver<ReturnType(Arguments...)>* receiver = iterator;
            ASSERT(receiver->m_dispatcher != nullptr, "Receiver's dispatcher is nullptr!");

            // Advance to the next receiver.
            iterator = iterator->m_next;

            // Unsubscribe a receiver.
            receiver->m_dispatcher = nullptr;
            receiver->m_previous = nullptr;
            receiver->m_next = nullptr;
        }

        m_begin = nullptr;
        m_end = nullptr;
    }

    template<typename ReturnType, typename... Arguments>
    bool DispatcherBase<ReturnType(Arguments...)>::Subscribe(Receiver<ReturnType(Arguments...)>& receiver, bool unsubscribeReceiver)
    {
        // Check if receiver is already subscribed somewhere else.
        if(receiver.m_dispatcher != nullptr)
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

        ASSERT(receiver.m_previous == nullptr, "Receiver's previous list element is not nullptr!");
        ASSERT(receiver.m_next == nullptr, "Receiver's next list element is not nullptr!");

        // Add receiver to the linked list.
        if(m_begin == nullptr)
        {
            ASSERT(m_end == nullptr, "Linked list's beginning element is nullptr but the ending is not!");

            // Add as the only element on the list.
            m_begin = &receiver;
            m_end = &receiver;
        }
        else
        {
            ASSERT(m_end != nullptr, "Linked list's ending element is nullptr but the beginning is not!");

            // Add to the end of the list.
            m_end->m_next = &receiver;
            receiver.m_previous = m_end;
            m_end = &receiver;
        }

        // Set dispatcher's reference.
        receiver.m_dispatcher = this;

        return true;
    }

    template<typename ReturnType, typename... Arguments>
    void DispatcherBase<ReturnType(Arguments...)>::Unsubscribe(Receiver<ReturnType(Arguments...)>& receiver)
    {
        // Check if receiver is subscribed to this dispatcher.
        VERIFY(receiver.m_dispatcher == this, "Attempting to unsubscribe a receiver that is not subscribed to this dispatcher!");

        // Remove receiver from the linked list.
        if(m_begin == &receiver)
        {
            // Receiver is first on the list.
            if(m_end == &receiver)
            {
                // Remove as the only element on the list.
                m_begin = nullptr;
                m_end = nullptr;
            }
            else
            {
                ASSERT(receiver.m_next != nullptr, "Receiver's next list element is nullptr!");

                // Remove from the beginning of the list.
                m_begin = receiver.m_next;
                m_begin->m_previous = nullptr;
            }
        }
        else
        {
            // Receiver is not first on the list.
            if(m_end == &receiver)
            {
                ASSERT(receiver.m_previous != nullptr, "Receiver's previous list element is nullptr!");

                // Removing from the end of the list.
                m_end = receiver.m_previous;
                m_end->m_next = nullptr;

            }
            else
            {
                ASSERT(receiver.m_previous != nullptr, "Receiver's previous list element is nullptr!");
                ASSERT(receiver.m_next != nullptr, "Receiver's next list element is nullptr!");

                // Removing in the middle of the list.
                receiver.m_previous->m_next = receiver.m_next;
                receiver.m_next->m_previous = receiver.m_previous;
            }
        }

        // Clear receiver's members.
        receiver.m_dispatcher = nullptr;
        receiver.m_previous = nullptr;
        receiver.m_next = nullptr;
    }

    template<typename ReturnType, typename... Arguments>
    template<typename Collector>
    void DispatcherBase<ReturnType(Arguments...)>::Dispatch(Collector& collector, Arguments... arguments)
    {
        // Send a dispatch event to all receivers.
        Receiver<ReturnType(Arguments...)>* receiver = m_begin;

        while(receiver != nullptr)
        {
            // Check is we should continue processing receivers.
            if(!collector.ShouldContinue())
                break;

            // Invoke a receiver and collect the result.
            CollectorDispatcher<Collector, ReturnType(Arguments...)> invocation;
            invocation(collector, receiver, std::forward<Arguments>(arguments)...);

            // Advance to the next receiver.
            receiver = receiver->m_next;
        }
    }

    template<typename ReturnType, typename... Arguments>
    bool DispatcherBase<ReturnType(Arguments...)>::HasSubscribers() const
    {
        return m_begin != nullptr;
    }

    template<typename Collector, typename ReturnType, typename... Arguments>
    Dispatcher<ReturnType(Arguments...), Collector>::Dispatcher(ReturnType defaultResult) :
        m_defaultResult(defaultResult)
    {
    }

    template<typename Collector, typename ReturnType, typename... Arguments>
    Dispatcher<ReturnType(Arguments...), Collector>::Dispatcher(Dispatcher&& other) :
        Dispatcher(other.m_defaultResult)
    {
        // Invoke a move operation.
        *this = std::move(other);
    }

    template<typename Collector, typename ReturnType, typename... Arguments>
    Dispatcher<ReturnType(Arguments...), Collector>&
        Dispatcher<ReturnType(Arguments...), Collector>::operator=(Dispatcher&& other)
    {
        // Swap primitive types.
        std::swap(m_defaultResult, other.m_defaultResult);
        std::swap(m_begin, other.m_begin);
        std::swap(m_end, other.m_end);

        // Fix pointers of subscribed receivers.
        if(m_begin != nullptr)
        {
            ASSERT(m_end != nullptr, "Broken linked list pointers!");
            Receiver<ReturnType(Arguments...)>* receiver = m_begin;

            while(receiver != nullptr)
            {
                // Assign a new pointer and move to the next receiver.
                receiver->m_dispatcher = this;
                receiver = receiver->m_next
            }
        }

        if(other.m_begin != nullptr)
        {
            ASSERT(other.m_end != nullptr, "Broken linked list pointers!");
            Receiver<ReturnType(Arguments...)>* receiver = other.m_begin;

            while(receiver != nullptr)
            {
                // Assign a new pointer and move to the next receiver.
                receiver->m_dispatcher = &other;
                receiver = receiver->m_next
            }
        }

        return *this;
    }

    template<typename Collector, typename ReturnType, typename... Arguments>
    ReturnType Dispatcher<ReturnType(Arguments...), Collector>::Dispatch(Arguments... arguments)
    {
        // Create a result collector.
        Collector collector(m_defaultResult);

        // Dispatch to receivers.
        DispatcherBase<ReturnType(Arguments...)>::template 
            Dispatch<Collector>(collector, std::forward<Arguments>(arguments)...);

        // Return collected result.
        return collector.GetResult();
    }

    template<typename Collector, typename ReturnType, typename... Arguments>
    ReturnType Dispatcher<ReturnType(Arguments...), Collector>::operator()(Arguments... arguments)
    {
        return this->Dispatch(std::forward<Arguments>(arguments)...);
    }

    template<typename Collector, typename... Arguments>
    Dispatcher<void(Arguments...), Collector>::Dispatcher()
    {
    }

    template<typename Collector, typename... Arguments>
    void Dispatcher<void(Arguments...), Collector>::Dispatch(Arguments... arguments)
    {
        // Create default collector for void type.
        CollectDefault<void> collector;

        // Dispatch to receivers.
        DispatcherBase<void(Arguments...)>::template Dispatch<Collector>(collector, std::forward<Arguments>(arguments)...);
    }

    template<typename Collector, typename... Arguments>
    void Dispatcher<void(Arguments...), Collector>::operator()(Arguments... arguments)
    {
        return this->Dispatch(std::forward<Arguments>(arguments)...);
    }
}
