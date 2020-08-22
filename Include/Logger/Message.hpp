/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>
#include "Sink.hpp"

/*
    Message

    Object that holds a log message and its properties.

    void ExampleLoggerMessage(Logger::Sink& sink)
    {
        // Simply usage of log macro with global sink.
        // Utilizes fmt library for modern printf formatting.
        LOG("Hello {}!", "world");

        // Above macro equals to the following:
        Logger::Message message;
        message.SetText("Hello world!");
        message.SetSource(__FILE__);
        message.SetLine(__LINE__);
        sink.Write(message);
    }
*/

namespace Logger
{
    struct Severity
    {
        enum Type
        {
            Invalid,

            Trace,
            Debug,
            Info,
            Warning,
            Error,
            Fatal,

            Count,
        };
    };

    class Message
    {
    public:
        Message() = default;
        Message(Message&& other);

        // Destructor left intentionally without virtual dispatch
        // to save on vtable lookup when ScopeMessage gets destroyed.
        ~Message() = default;

        Message(const Message&) = delete;
        Message& operator=(const Message&) = delete;

        template<typename... Args>
        Message& Format(const char* format, Args&&... arguments)
        {
            m_text = fmt::format(format, arguments...);
            return *this;
        }

        Message& SetText(std::string text)
        {
            m_text = text;
            return *this;
        }

        Message& SetSeverity(Severity::Type severity)
        {
            m_severity = severity;
            return *this;
        }

        Message& SetSource(const char* source)
        {
            m_source = source;
            return *this;
        }

        Message& SetLine(unsigned int line)
        {
            m_line = line;
            return *this;
        }

        const std::string& GetText() const
        {
            return m_text;
        }

        Severity::Type GetSeverity() const
        {
            return m_severity;
        }

        const char* GetSource() const
        {
            return m_source;
        }

        unsigned int GetLine() const
        {
            return m_line;
        }

        bool IsEmpty() const
        {
            return m_text.empty();
        }

    private:
        std::string m_text;
        Severity::Type m_severity = Severity::Info;
        const char* m_source = nullptr;
        unsigned int m_line = 0;
    };
}

/*
    Scoped Message

    Log message object that writes to a sink at the end of its lifetime.
    Extensively used by LOG() macro to write to the sink at the end of scope.
*/

namespace Logger
{
    class Sink;

    class ScopedMessage final : public Message
    {
    public:
        ScopedMessage(Sink& sink) :
            m_sink(sink)
        {
        }

        ~ScopedMessage()
        {
            m_sink.Write(*this);
        }

    private:
        Logger::Sink& m_sink;
    };
}
