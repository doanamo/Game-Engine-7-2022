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
    static const int MessageHistorySize = 1000;
}

History::History() = default;
History::~History() = default;

bool History::Initialize() const
{
    return true;
}

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
