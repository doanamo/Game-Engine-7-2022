/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Format

    Default logging format used for all outputs.
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
