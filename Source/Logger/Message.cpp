/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Logger/Message.hpp"
#include "Logger/Sink.hpp"
using namespace Logger;

namespace
{
    // Case insensitive character comparison function for std::search().
    bool CaseInsensitiveCharacterComparison(char a, char b)
    {
        return std::tolower(a) == std::tolower(b);
    };
}

Message::Message() :
    std::ostream(&m_text),
    m_severity(Severity::Info),
    m_source(),
    m_line(0)
{
}

Message::Message(Message&& other) :
    std::ostream(&m_text)
{
    m_severity = other.m_severity;
    other.m_severity = Severity::Info;

    m_text = std::move(other.m_text);
    m_source = std::move(other.m_source);

    m_line = other.m_line;
    other.m_line = 0;
}

Message::~Message()
{
}

Message& Message::SetSeverity(Severity::Type severity)
{
    ASSERT(Severity::Invalid < severity && severity < Severity::Count, "Severity argument is invalid!");

    m_severity = severity;
    return *this;
}

Message& Message::SetText(std::string text)
{
    m_text.str(text);
    return *this;
}

Message& Message::SetSource(std::string source)
{
    // Assign source string.
    m_source = source;

    // Truncate the source file path.
    if(!m_source.empty())
    {
        // Normalize source path delimeters.
        std::replace(m_source.begin(), m_source.end(), '\\', '/');

        // Find and remove base path to source directory.
        std::string sourceDir = "Source/";

        auto reverseIt = std::search(
            m_source.rbegin(), m_source.rend(),
            sourceDir.rbegin(), sourceDir.rend(),
            CaseInsensitiveCharacterComparison
        );

        if(reverseIt != m_source.rend())
        {
            reverseIt += sourceDir.length();
        }

        // Find and remove base path to include directory.
        std::string includeDir = "Include/";

        if(reverseIt == m_source.rend())
        {
            reverseIt = std::search(
                m_source.rbegin(), m_source.rend(),
                includeDir.rbegin(), includeDir.rend(),
                CaseInsensitiveCharacterComparison
            );

            if(reverseIt != m_source.rend())
            {
                reverseIt += includeDir.length();
            }
        }

        // Remove the base path to a file.
        if(reverseIt != m_source.rend())
        {
            m_source.erase(m_source.begin(), reverseIt.base());
        }
    }

    return *this;
}

Message& Message::SetLine(int line)
{
    ASSERT(line > 0, "Attempting to set an invalid source line!");

    m_line = line;
    return *this;
}

Severity::Type Message::GetSeverity() const
{
    return m_severity;
}

std::string Message::GetText() const
{
    return m_text.str();
}

std::string Message::GetSource() const
{
    return m_source;
}

int Message::GetLine() const
{
    return m_line;
}

bool Message::IsEmpty() const
{
    return m_text.str().empty();
}

ScopedMessage::ScopedMessage(Sink* sink) :
    m_sink(sink)
{
    ASSERT(sink != nullptr, "Scoped message needs a valid sink reference!");
}

ScopedMessage::ScopedMessage(ScopedMessage&& other)
{
    m_sink = other.m_sink;
    other.m_sink = nullptr;
}

ScopedMessage::~ScopedMessage()
{
    if(m_sink != nullptr)
    {
        m_sink->Write(*this);
    }
}
