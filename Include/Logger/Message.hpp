/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>
#include "Sink.hpp"

/*
    Logger Message

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
    // Message severity.
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

    // Message class.
    class Message
    {
    public:
        Message();
        Message(Message&& other);
        virtual ~Message();

        // Disable copying.
        Message(const Message&) = delete;
        Message& operator=(const Message&) = delete;

        // Formats message text.
        template<typename... Args>
        Message& Format(const char* format, Args&&... arguments)
        {
            m_text = fmt::format(format, arguments...);
            return *this;
        }

        // Sets message text.
        Message& SetText(std::string text);

        // Sets message severity.
        Message& SetSeverity(Severity::Type severity);

        // Sets message source.
        Message& SetSource(const char* source);

        // Sets message line.
        Message& SetLine(unsigned int line);

        // Gets message text.
        const std::string& GetText() const;

        // Gets message severity.
        Severity::Type GetSeverity() const;

        // Gets message source.
        const char* GetSource() const;

        // Gets message line.
        unsigned int GetLine() const;

        // Checks if message is empty.
        bool IsEmpty() const;

    private:
        // Message state.
        std::string    m_text;
        Severity::Type m_severity;
        const char*    m_source;
        unsigned int   m_line;
    };
}

/*
    Logger Scoped Message

    Log message object that writes to a sink at the end of its lifetime.
    Extensively used by LOG() macro to write to the sink at the end of scope.
*/

namespace Logger
{
    // Forward declarations.
    class Sink;

    // Scoped message class.
    class ScopedMessage : public Message
    {
    public:
        ScopedMessage(Sink& sink);
        ~ScopedMessage();

    private:
        // Message sink output.
        Logger::Sink& m_sink;
    };
}
