/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Logger/Message.hpp"
#include "Logger/Sink.hpp"
using namespace Logger;

Message::Message() = default;
Message::~Message() = default;

Message::Message(Message&& other)
{
    m_text = std::move(other.m_text);

    m_severity = other.m_severity;
    other.m_severity = Severity::Info;

    m_source = other.m_source;
    other.m_source = nullptr;

    m_line = other.m_line;
    other.m_line = 0;
}

Message& Message::SetText(std::string text)
{
    m_text = text;
    return *this;
}

Message& Message::SetSeverity(Severity::Type severity)
{
    assert(Severity::Invalid < severity && severity < Severity::Count && "Severity argument is invalid!");

    m_severity = severity;
    return *this;
}

Message& Message::SetSource(const char* source)
{
    m_source = source;
    return *this;
}

Message& Message::SetLine(unsigned int line)
{
    assert(line > 0 && "Attempting to set an invalid source line!");

    m_line = line;
    return *this;
}

const std::string& Message::GetText() const
{
    return m_text;
}

Severity::Type Message::GetSeverity() const
{
    return m_severity;
}

const char* Message::GetSource() const
{
    return m_source;
}

unsigned int Message::GetLine() const
{
    return m_line;
}

bool Message::IsEmpty() const
{
    return m_text.empty();
}

ScopedMessage::ScopedMessage(Sink& sink) :
    m_sink(sink)
{
}

ScopedMessage::~ScopedMessage()
{
    m_sink.Write(*this);
}
