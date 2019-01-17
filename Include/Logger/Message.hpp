/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>
#include <sstream>
#include "Logger/Sink.hpp"

/*
    Logger Message

    Object that holds a log message and its properties.

    void ExampleLoggerMessage(Logger::Sink& sink)
    {
        // Simply use a log macro with a global sink.
        LOG() << "Hello world!";

        // Above macro equals to the following:
        Logger::Message message;
        message.SetSource(__FILE__);
        message.SetLine(__LINE__);
        message.SetText("Hello world!");
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

            Debug,
            Info,
            Warning,
            Error,

            Count,
        };
    };

    // Message class.
    class Message : public std::ostream, private NonCopyable
    {
    public:
        Message();
        Message(Message&& other);
        virtual ~Message();

        // Sets the message severity.
        Message& SetSeverity(Severity::Type severity);

        // Sets the message text.
        Message& SetText(std::string text);

        // Sets the message source.
        Message& SetSource(std::string source);

        // Sets the message line.
        Message& SetLine(int line);

        // Gets the message severity.
        Severity::Type GetSeverity() const;

        // Gets the message text.
        std::string GetText() const;

        // Gets the message source.
        std::string GetSource() const;

        // Gets the message line.
        int GetLine() const;

        // Checks if the message is empty.
        bool IsEmpty() const;

    private:
        // Message state.
        Severity::Type m_severity;
        std::stringbuf m_text;
        std::string    m_source;
        int            m_line;
    };
}

/*
    Logger Scoped Message

    Log message object that writes to a sink at the end of its lifetime.
    Extensively used by Log() macro to write to the sink at the end of scope.
    Using anynomous instantiation and its scope ends at the next semicolon (';').
*/

namespace Logger
{
    // Forward declarations.
    class Sink;

    // Scoped message class.
    class ScopedMessage : public Message
    {
    public:
        ScopedMessage(Sink* sink);
        ScopedMessage(ScopedMessage&& other);
        ~ScopedMessage();

    private:
        // Message sink output.
        Logger::Sink* m_sink;
    };
}
