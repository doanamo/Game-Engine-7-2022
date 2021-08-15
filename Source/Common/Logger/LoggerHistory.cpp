/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Common/Precompiled.hpp"
#include "Common/Logger/LoggerHistory.hpp"
#include "Common/Logger/LoggerFormat.hpp"
using namespace Logger;

namespace
{
    static const int MessageHistorySize = 10000;
}

History::MessageStats::MessageStats()
{
    for(auto& severity : severityCount)
    {
        severity = 0;
    }
}

History::History() = default;
History::~History() = default;

bool History::Initialize() const
{
    return true;
}

void History::Write(const Message& message, const SinkContext& context)
{
    std::scoped_lock guard(m_lock);

    // Truncate message history.
    if(m_messages.size() == MessageHistorySize)
    {
        assert(m_messages.front().severity < Severity::Count);
        int severityID = static_cast<int>(m_messages.front().severity);
        m_stats.severityCount[severityID] -= 1;

        m_messages.pop_front();
    }

    // Add new message entry.
    assert(message.GetSeverity() < Severity::Count);
    int severityID = static_cast<int>(message.GetSeverity());
    m_stats.severityCount[severityID] += 1;

    MessageEntry messageEntry;
    messageEntry.severity = message.GetSeverity();
    messageEntry.text = DefaultFormat::ComposeMessage(message, context);
    m_messages.emplace_back(std::move(messageEntry));
}

void History::Clear()
{
    std::scoped_lock guard(m_lock);

    m_messages = MessageList();
    m_stats = MessageStats();
}
