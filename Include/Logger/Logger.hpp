/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Logger/Message.hpp"

/*
    Logger

    Writes log messages to multiple outputs for debugging purposes.
    - Logger:  Interface for global logger sink.
    - Message: Single log message that will be written.
    - Output:  Message output streams (e.g. file writer, console output).
    - Format:  Defines how log messages are formatted before being written.
    - Sink:    Collects messages and sends them to registered outputs.
    
    void ExampleLogger()
    {
        // Initialize default logger.
        Logger::Initialize();

        // Write log message.
        LOG() << "Hello world!";

        // Write log messages of different severity.
        // This will allow us to filter them if needed.
        LOG_TRACE() << "Writing trace message.";
        LOG_DEBUG() << "Writing debug message.";
        LOG_INFO() << "Writing diagnostic message.";
        LOG_WARNING() << "Writing warning message.";
        LOG_ERROR() << "Writing error message.";
        LOG_FATAL() << "Writing fatal message.";

        // Create an indent until the end of the current scope.
        {
            LOG_SCOPED_INDENT();
            LOG() << "Indented message!";
        }

        LOG() << "Non indented message.";
    }
*/

namespace Logger
{
    // Forward declarations.
    class Sink;

    // Initializes global logger sink.
    void Initialize();

    // Writes message to global logger sink.
    void Write(const Message& message);

    // Advances frame of reference.
    // Helps distinguish messages from different frames.
    int AdvanceFrameReference();

    // Gets global logger sink.
    Sink& GetGlobalSink();

    // Checks if global logger is initialized.
    bool IsInitialized();
}

// Utility macros.
#define LOG_SCOPED_INDENT() Logger::ScopedIndent loggerIndent ## __LINE__(Logger::GetGlobalSink())

#ifndef NDEBUG
    #define LOG() Logger::ScopedMessage(Logger::GetGlobalSink()).SetSource(__FILE__).SetLine(__LINE__)
#else
    #define LOG() Logger::ScopedMessage(Logger::GetGlobalSink())
#endif

#define LOG_TRACE()   LOG().SetSeverity(Logger::Severity::Trace)
#define LOG_DEBUG()   LOG().SetSeverity(Logger::Severity::Debug)
#define LOG_INFO()    LOG().SetSeverity(Logger::Severity::Info)
#define LOG_WARNING() LOG().SetSeverity(Logger::Severity::Warning)
#define LOG_ERROR()   LOG().SetSeverity(Logger::Severity::Error)
#define LOG_FATAL()   LOG().SetSeverity(Logger::Severity::Fatal)
