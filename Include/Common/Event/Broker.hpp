/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <any>
#include <utility>
#include <typeindex>
#include <unordered_map>
#include "Common/Result.hpp"
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
        enum class DispatchErrors
        {
            UnregisteredEventType,
            IncorrectResultType,
        };

        template<typename ResultType>
        using DispatchResult = Common::Result<ResultType, DispatchErrors>;

        template<typename ResultType, typename EventType>
        using DispatcherType = Dispatcher<ResultType(const EventType&)>;
        using DispatcherMap = std::unordered_map<std::type_index, std::any>;

        template<typename ResultType, typename EventType>
        struct DispatcherStorage
        {
            DispatcherStorage(std::unique_ptr<Collector<ResultType>>&& collector) :
                dispatcher(std::make_shared<DispatcherType<
                    ResultType, EventType>>(std::move(collector)))
            {
            }

            // Must be copy constructible to satisfy std::any requirements.
            std::shared_ptr<DispatcherType<ResultType, EventType>> dispatcher;
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

        template<typename ResultType, typename EventType>
        bool Register(std::unique_ptr<Collector<ResultType>>&& collector = nullptr)
        {
            if(m_finalized)
            {
                LOG_WARNING("Event broker is finalized and cannot register more event types!");
                return false;
            }

            std::type_index eventType = typeid(EventType);
            auto it = m_dispatcherMap.find(eventType);
            if(it == m_dispatcherMap.end())
            {
                auto dispatcher = std::make_any<DispatcherStorage<
                    ResultType, EventType>>(std::move(collector));
                auto result = m_dispatcherMap.emplace(eventType, std::move(dispatcher));;
            }

            return true;
        }

        void Finalize()
        {
            m_finalized = true;
        }

        template<typename ResultType, typename EventType>
        bool Subscribe(Receiver<ResultType(const EventType&)>& receiver,
            SubscriptionPolicy subscriptionPolicy = SubscriptionPolicy::RetainSubscription,
            PriorityPolicy priorityPolicy = PriorityPolicy::InsertBack)
        {
            std::type_index eventType = typeid(EventType);
            auto it = m_dispatcherMap.find(eventType);
            if(it == m_dispatcherMap.end())
                return false;

            DispatcherStorage<ResultType, EventType>* storage = nullptr;
            storage = std::any_cast<DispatcherStorage<ResultType, EventType>>(&it->second);
            if(storage == nullptr)
                return false;

            return storage->dispatcher->Subscribe(receiver, subscriptionPolicy, priorityPolicy);
        }

        template<typename ResultType, typename EventType>
        DispatchResult<ResultType> Dispatch(const EventType& event)
        {
            std::type_index eventType = typeid(EventType);
            auto it = m_dispatcherMap.find(eventType);
            if(it == m_dispatcherMap.end())
                return Common::Failure(DispatchErrors::UnregisteredEventType);

            DispatcherStorage<ResultType, EventType>* storage = nullptr;
            storage = std::any_cast<DispatcherStorage<ResultType, EventType>>(&it->second);
            if(storage == nullptr)
                return Common::Failure(DispatchErrors::IncorrectResultType);

            if constexpr(std::is_same<ResultType, void>::value)
            {
                storage->dispatcher->Dispatch(event);
                return Common::Success();
            }
            else
            {
                return Common::Success(storage->dispatcher->Dispatch(event));
            }
        }

    private:
        DispatcherMap m_dispatcherMap;
        bool m_finalized = false;
    };
}
