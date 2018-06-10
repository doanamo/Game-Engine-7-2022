/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>

/*
    Logger Format

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
    // Forward declarations.
    class Message;
    struct SinkContext;

    // Default format class.
    class DefaultFormat
    {
    public:
        static std::string ComposeSessionStart();
        static std::string ComposeMessage(const Message& message, const SinkContext& context);
        static std::string ComposeSessionEnd();
    };
}
