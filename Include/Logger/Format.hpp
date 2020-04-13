/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>

/*
    Format

    Default logging format used for all outputs.

    void ExampleLoggerFormat(std::ostream& stream, const Logger::Message& message, const SinkContext& context)
    {
        stream << ComposeSessionStart();
        stream << ComposeMessage(message, context);
        stream << ComposeSessionEnd();
    }
*/

namespace Logger
{
    class Message;
    struct SinkContext;

    class DefaultFormat
    {
    public:
        static std::string ComposeSessionStart();
        static std::string ComposeMessage(const Message& message, const SinkContext& context);
        static std::string ComposeSessionEnd();
    };
}
