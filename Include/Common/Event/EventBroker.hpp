/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <any>
#include <unordered_map>
#include <Reflection/Reflection.hpp>
#include "Common/Result.hpp"
#include "Common/Event/EventBase.hpp"
#include "Common/Event/EventDispatcher.hpp"

/*
    Event Broker

    Maps receivers to event types to allow dispatching arbitrary events through a single point.
*/

namespace Event
{
    template<typename Type>
    class Receiver;

    class Broker : private Common::NonCopyable
    {
    public:
        enum class RegisterErrors
        {
            AlreadyFinalized,
        };

        using RegisterResult = Common::Result<void, RegisterErrors>;

        enum class SubscriptionErrors
        {
            UnregisteredEventType,
            IncorrectResultType,
            FailedSubscription,
        };

        using SubscriptionResult = Common::Result<void, SubscriptionErrors>;

        enum class DispatchErrors
        {
            UnregisteredEventType,
            IncorrectResultType,
        };

        template<typename ResultType>
        using DispatchResult = Common::Result<ResultType, DispatchErrors>;

        template<typename ResultType, typename EventType>
        using DispatcherType = Dispatcher<ResultType(const EventType&)>;
        using DispatcherMap = std::unordered_map<Reflection::TypeIdentifier, std::any>;

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
            std::swap(m_finalized, other.m_finalized);
            return *this;
        }

        template<typename ResultType, typename EventType>
        RegisterResult Register(std::unique_ptr<Collector<ResultType>>&& collector = nullptr)
        {
            // Only allow new event types if broker has not been finalized.
            if(m_finalized)
            {
                return Common::Failure(RegisterErrors::AlreadyFinalized);
            }

            // Register new event type.
            Reflection::TypeIdentifier eventType = Reflection::GetIdentifier<EventType>();
            auto it = m_dispatcherMap.find(eventType);
            if(it == m_dispatcherMap.end())
            {
                auto dispatcher = std::make_any<
                    DispatcherStorage<ResultType, EventType>>(std::move(collector));
                auto result = m_dispatcherMap.emplace(eventType, std::move(dispatcher));;
            }

            return Common::Success();
        }

        void Finalize()
        {
            m_finalized = true;
        }

        template<typename ResultType, typename EventType>
        SubscriptionResult Subscribe(Receiver<ResultType(const EventType&)>& receiver,
            SubscriptionPolicy subscriptionPolicy = SubscriptionPolicy::RetainSubscription,
            PriorityPolicy priorityPolicy = PriorityPolicy::InsertBack)
        {
            // Ensure that event type has been registered.
            Reflection::TypeIdentifier eventType = Reflection::GetIdentifier<EventType>();
            auto it = m_dispatcherMap.find(eventType);
            if(it == m_dispatcherMap.end())
            {
                return Common::Failure(SubscriptionErrors::UnregisteredEventType);
            }

            // Subscribe receiver to registered event type.
            DispatcherStorage<ResultType, EventType>* storage = nullptr;
            storage = std::any_cast<DispatcherStorage<ResultType, EventType>>(&it->second);
            if(storage == nullptr)
            {
                return Common::Failure(SubscriptionErrors::IncorrectResultType);
            }

            if(!storage->dispatcher->Subscribe(receiver, subscriptionPolicy, priorityPolicy))
            {
                return Common::Failure(SubscriptionErrors::FailedSubscription);
            }

            return Common::Success();
        }

        template<typename ResultType, typename EventType>
        DispatchResult<ResultType> Dispatch(const EventType& event)
        {
            // Ensure that event type has been registered.
            Reflection::TypeIdentifier eventType = Reflection::GetIdentifier<EventType>();
            auto it = m_dispatcherMap.find(eventType);
            if(it == m_dispatcherMap.end())
            {
                return Common::Failure(DispatchErrors::UnregisteredEventType);
            }

            // Dispatch event to receivers.
            DispatcherStorage<ResultType, EventType>* storage = nullptr;
            storage = std::any_cast<DispatcherStorage<ResultType, EventType>>(&it->second);
            if(storage == nullptr)
            {
                return Common::Failure(DispatchErrors::IncorrectResultType);
            }

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
