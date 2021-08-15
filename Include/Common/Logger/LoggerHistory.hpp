/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <deque>
#include "Common/Logger/LoggerOutput.hpp"
#include "Common/Logger/LoggerMessage.hpp"

/*
    Logger History
*/

namespace Logger
{
    class History final : public Output
    {
    public:
        struct MessageEntry
        {
            Severity::Type severity;
            std::string text;
        };

        struct MessageStats
        {
            MessageStats();

            uint32_t severityCount[Severity::Count];
        };

        using MessageList = std::deque<MessageEntry>;

    public:
        History();
        ~History();

        bool Initialize() const override;
        void Write(const Message& message, const SinkContext& context) override;
        void Clear();

        MessageList GetMessages() const
        {
            std::scoped_lock guard(m_lock);
            return m_messages;
        }

        MessageStats GetStats() const
        {
            std::scoped_lock guard(m_lock);
            return m_stats;
        }

    private:
        mutable std::mutex m_lock;
        MessageList m_messages;
        MessageStats m_stats;
    };
}
