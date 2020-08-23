/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Logger/History.hpp"
#include "Logger/Format.hpp"
using namespace Logger;

namespace
{
    static const int MessageHistorySize = 1000;
}

History::History() = default;
History::~History() = default;

void History::Write(const Logger::Message& message, const Logger::SinkContext& context)
{
    // Truncate message history.
    if(m_messages.size() == MessageHistorySize)
    {
        m_messages.pop_front();
    }

    // Add new message entry.
    MessageEntry messageEntry;
    messageEntry.severity = message.GetSeverity();
    messageEntry.text = DefaultFormat::ComposeMessage(message, context);
    m_messages.emplace_back(std::move(messageEntry));
}

const History::MessageList& History::GetMessages() const
{
    return m_messages;
}
