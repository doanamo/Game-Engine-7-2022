/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <fmt/core.h>
#include "Message.hpp"

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
        LOG("Hello {}!", "world");

        // Write log messages of different severity.
        // This will allow us to filter them if needed.
        LOG_TRACE("Writing trace message.");
        LOG_DEBUG("Writing debug message.");
        LOG_INFO("Writing diagnostic message.");
        LOG_WARNING("Writing warning message.");
        LOG_ERROR("Writing error message.");
        LOG_FATAL("Writing fatal message.");

        // Create an indent until the end of the current scope.
        {
            LOG_SCOPED_INDENT();
            LOG("Indented message!");
        }

        LOG("Non indented message.");
    }
*/

namespace Logger
{
    class Sink;

    void Initialize();
    void Write(const Message& message);
    int AdvanceFrameReference();

    Sink& GetGlobalSink();
    bool IsInitialized();
}

// Utility macros.
#define LOG_SCOPED_INDENT() Logger::ScopedIndent loggerIndent ## __LINE__(Logger::GetGlobalSink())

#ifndef NDEBUG
    #define LOG_SCOPED_MESSAGE() Logger::ScopedMessage(Logger::GetGlobalSink()).SetSource(__FILE__).SetLine(__LINE__)
#else
    #define LOG_SCOPED_MESSAGE() Logger::ScopedMessage(Logger::GetGlobalSink())
#endif

#define LOG(format, ...)         LOG_SCOPED_MESSAGE().Format(format, ## __VA_ARGS__)
#define LOG_TRACE(format, ...)   LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Trace)
#define LOG_DEBUG(format, ...)   LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Debug)
#define LOG_INFO(format, ...)    LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Info)
#define LOG_WARNING(format, ...) LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Warning)
#define LOG_ERROR(format, ...)   LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Error)
#define LOG_FATAL(format, ...)   LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Fatal)
