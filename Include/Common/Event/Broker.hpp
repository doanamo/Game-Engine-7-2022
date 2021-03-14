/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <any>
#include <utility>
#include <typeindex>
#include <unordered_map>
#include "Common/Event/Dispatcher.hpp"

/*
    Event Broker

    Shared point where multiple receiver and dispatcher can be stored and
    signaled for different event types. Note that dispatchers are stored
    in std::any which can trigger allocations (to be replaced in future).
*/

namespace Event
{
    template<typename Type>
    class Receiver;

    class Broker : private Common::NonCopyable
    {
    public:
        template<typename EventType>
        using DispatcherType = Dispatcher<bool(const EventType&)>;
        using DispatcherMap = std::unordered_map<std::type_index, std::any>;

        template<typename EventType>
        struct DispatcherStorage
        {
            DispatcherStorage(std::unique_ptr<Collector<bool>>&& collector) :
                dispatcher(std::make_shared<DispatcherType<EventType>>(
                    std::forward<std::unique_ptr<Collector<bool>>>(collector)))
            {
            }

            // Must be copy constructible to satisfy std::any requirements.
            std::shared_ptr<DispatcherType<EventType>> dispatcher;
        };

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

        template<typename EventType>
        bool Subscribe(Receiver<bool(const EventType&)>& receiver,
            SubscriptionPolicy subscriptionPolicy = SubscriptionPolicy::RetainSubscription,
            PriorityPolicy priorityPolicy = PriorityPolicy::InsertBack)
        {
            std::type_index eventType = typeid(EventType);
            auto it = m_dispatcherMap.find(eventType);
            if(it == m_dispatcherMap.end())
            {
                auto collector = std::make_unique<CollectWhileTrue>(true);
                auto dispatcher = std::make_any<DispatcherStorage<EventType>>(std::move(collector));
                auto result = m_dispatcherMap.emplace(eventType, std::move(dispatcher));
                ASSERT(result.second, "Dispatcher storage failed to be emplaced!");
                it = result.first;
            }

            DispatcherStorage<EventType>* storage = nullptr;
            storage = std::any_cast<DispatcherStorage<EventType>>(&it->second);
            return storage->dispatcher->Subscribe(receiver, subscriptionPolicy, priorityPolicy);
        }

        template<typename EventType>
        bool Dispatch(const EventType& event)
        {
            std::type_index eventType = typeid(EventType);
            auto it = m_dispatcherMap.find(eventType);
            if(it == m_dispatcherMap.end())
                return false;

            DispatcherStorage<EventType>* storage = nullptr;
            storage = std::any_cast<DispatcherStorage<EventType>>(&it->second);
            return storage->dispatcher->Dispatch(event);
        }

    private:
        DispatcherMap m_dispatcherMap;
    };
}
