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
        Message();
        Message(Message&& other);
        virtual ~Message();

        Message(const Message&) = delete;
        Message& operator=(const Message&) = delete;

        template<typename... Args>
        Message& Format(const char* format, Args&&... arguments)
        {
            m_text = fmt::format(format, arguments...);
            return *this;
        }

        Message& SetText(std::string text);
        Message& SetSeverity(Severity::Type severity);
        Message& SetSource(const char* source);
        Message& SetLine(unsigned int line);

        const std::string& GetText() const;
        Severity::Type GetSeverity() const;
        const char* GetSource() const;
        unsigned int GetLine() const;
        bool IsEmpty() const;

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

    class ScopedMessage : public Message
    {
    public:
        ScopedMessage(Sink& sink);
        ~ScopedMessage();

    private:
        Logger::Sink& m_sink;
    };
}
