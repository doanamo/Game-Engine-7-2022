/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <any>
#include <utility>
#include <typeindex>
#include <unordered_map>
#include "Event/Dispatcher.hpp"

/*
    Event Broker

    Shared point where multiple receiver and dispatcher can be stored and signaled for different event types.
*/

namespace Event
{
    template<typename Type>
    class Receiver;

    class Broker : private Common::NonCopyable
    {
    public:
        using EventHandle = std::any;
        using DispatcherHandle = std::any;
        using DispatcherInvoker = Delegate<bool(DispatcherHandle&, const EventHandle&)>;
        using DispatcherEntry = std::pair<DispatcherHandle, DispatcherInvoker>;
        using DispatcherMap = std::unordered_map<std::type_index, DispatcherEntry>;

        Broker() = default;
        ~Broker() = default;

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
            using DispatcherPtr = std::shared_ptr<Dispatcher<bool(const Type&), CollectWhileTrue>>;

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

            DispatcherHandle& handle = it->second.first;
            auto& dispatcher = std::any_cast<DispatcherPtr&>(handle);
            return dispatcher->Subscribe(receiver, unsubscribeReceiver, insertFront);
        }

        bool Dispatch(const EventHandle& eventHandle)
        {
            if(!eventHandle.has_value())
                return false;

            std::type_index type = eventHandle.type();
            auto it = m_dispatcherMap.find(type);

            if(it == m_dispatcherMap.end())
                return false;

            DispatcherEntry& dispatcherEntry = it->second;
            DispatcherHandle& dispatcherHandle = dispatcherEntry.first;
            DispatcherInvoker& dispatcherInvoker = dispatcherEntry.second;
            return dispatcherInvoker(dispatcherHandle, eventHandle);
        }

    private:
        DispatcherMap m_dispatcherMap;
    };
}
