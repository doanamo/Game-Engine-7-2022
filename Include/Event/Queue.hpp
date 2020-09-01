/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <any>
#include <queue>

/*
    Event Queue
*/

namespace Event
{
    class Queue : private Common::NonCopyable
    {
    public:
        using EventList = std::queue<std::any>;

        Queue() = default;
        ~Queue() = default;

        Queue(Queue&& other)
        {
            *this = std::move(other);
        }

        Queue& operator=(Queue&& other)
        {
            std::swap(m_eventList, other.m_eventList);
            return *this;
        }

        template<typename Type>
        void Push(const Type& event)
        {
            m_eventList.push(event);
        }

        std::any Pop()
        {
            if(m_eventList.empty())
                return std::any();

            std::any event = m_eventList.front();
            m_eventList.pop();
            return event;
        }

        bool IsEmpty() const
        {
            return m_eventList.empty();
        }

    private:
        EventList m_eventList;
    };
}
