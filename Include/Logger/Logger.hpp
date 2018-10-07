/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Logger/Message.hpp"

/*
    Logger

    Writes log messages to multiple outputs for debugging purposes.
    - Message: Single log message that will be written.
    - Sink: Collects messages and sends them to registered outputs.
    - Format: Defines how log messages are formatted when processed.
    - Output: Message output streams (e.g. file writer, console output).
    
    void ExampleLogger()
    {
        // Initialize the default logger.
        Logger::Initialize();

        // Write a log message.
        LOG() << "Hello world!";

        // Write log messages of different severity.
        // This will allow us to filter them if needed.
        LOG_DEBUG() << "Writing debug trace.";
        LOG_INFO() << "Writing some diagnostic info.";
        LOG_WARNING() << "Writing a warning message.";
        LOG_ERROR() << "Writing an error message.";

        // Create an indent until the end of the current scope.
        {
            LOG() << "Non indented message..." << LOG_INDENT();
            LOG() << "Indented message!";
        }

        LOG() << "Non indented message.";
    }
*/

namespace Logger
{
    // Forward declarations.
    class Sink;

    // Initializes the default logger sink.
    void Initialize();

    // Writes to the global logger sink.
    void Write(const Message& message);

    // Advances the frame of reference.
    // This helps distinguish messages from different frames of reference.
    int AdvanceFrameCounter();

    // Checks if the global logger is initialized.
    bool IsInitialized();
    
    // Gets the global logger sink.
    Sink& GetGlobalSink();
}

// Utility macros.
#ifndef NDEBUG
    #define LOG() Logger::ScopedMessage(&Logger::GetGlobalSink()).SetSource(__FILE__).SetLine(__LINE__)
#else
    #define LOG() Logger::ScopedMessage(&Logger::GetGlobalSink())
#endif

#define LOG_DEBUG()   LOG().SetSeverity(Logger::Severity::Debug)
#define LOG_INFO()    LOG().SetSeverity(Logger::Severity::Info)
#define LOG_WARNING() LOG().SetSeverity(Logger::Severity::Warning)
#define LOG_ERROR()   LOG().SetSeverity(Logger::Severity::Error)

#define LOG_INDENT() ""; Logger::ScopedIndent loggerIndent ## __LINE__(&Logger::GetGlobalSink())
