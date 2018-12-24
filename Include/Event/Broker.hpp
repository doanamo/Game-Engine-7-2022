/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Event/Dispatcher.hpp"

/*
    Event Broker
*/

namespace Event
{
    // Forward declarations.
    template<typename Type>
    class Receiver;

    // Event broker class.
    class Broker
    {
    public:
        // Type declarations.
        using EventHandle = std::any;
        using DispatcherHandle = std::any;
        using DispatcherInvoker = Delegate<bool(DispatcherHandle&, const EventHandle&)>;
        using DispatcherEntry = std::pair<DispatcherHandle, DispatcherInvoker>;
        using DispatcherMap = std::unordered_map<std::type_index, DispatcherEntry>;

    public:
        Broker() = default;
        ~Broker() = default;

        Broker(const Broker& other) = delete;
        Broker& operator=(const Broker& other) = delete;

        Broker(Broker&& other)
        {
            *this = std::move(other);
        }

        Broker& operator=(Broker&& other)
        {
            std::swap(m_dispatcherMap, other.m_dispatcherMap);
            return *this;
        }

        template<typename Type>
        bool Subscribe(Receiver<bool(const Type&)>& receiver, bool unsubscribeReceiver = false, bool insertFront = false)
        {
            // Declare the shared pointer type.
            using DispatcherPtr = std::shared_ptr<Dispatcher<bool(const Type&), CollectWhileTrue>>;

            // Find or create dispatcher if needed.
            std::type_index type = typeid(Type);
            auto it = m_dispatcherMap.find(type);

            if(it == m_dispatcherMap.end())
            {
                DispatcherPtr dispatcher = std::make_shared<Dispatcher<bool(const Type&), CollectWhileTrue>>(true);
                DispatcherHandle handle = std::make_any<DispatcherPtr>(dispatcher);
                DispatcherInvoker invoker = [](DispatcherHandle& dispatcherHandle, const EventHandle& eventHandle) -> bool
                {
                    auto& dispatcher = std::any_cast<DispatcherPtr&>(dispatcherHandle);
                    auto& event = std::any_cast<const Type&>(eventHandle);
                    return dispatcher->Dispatch(event);
                };

                auto result = m_dispatcherMap.insert({ type, std::make_pair(handle, invoker) });
                ASSERT(result.second, "Dispatcher entry was not inserted!");
                it = result.first;
            }

            // Subscribe receiver to the dispatcher.
            DispatcherHandle& handle = it->second.first;
            auto& dispatcher = std::any_cast<DispatcherPtr&>(handle);
            return dispatcher->Subscribe(receiver, unsubscribeReceiver, insertFront);
        }

        bool Dispatch(const EventHandle& eventHandle)
        {
            // Check if event is empty.
            if(!eventHandle.has_value())
                return false;

            // Find registered dispatcher by type.
            std::type_index type = eventHandle.type();
            auto it = m_dispatcherMap.find(type);

            if(it == m_dispatcherMap.end())
                return false;

            // Dispatch event through the dispatcher.
            DispatcherEntry& dispatcherEntry = it->second;
            DispatcherHandle& dispatcherHandle = dispatcherEntry.first;
            DispatcherInvoker& dispatcherInvoker = dispatcherEntry.second;
            return dispatcherInvoker(dispatcherHandle, eventHandle);
        }

    private:
        // List of dispatchers.
        DispatcherMap m_dispatcherMap;
    };
}
