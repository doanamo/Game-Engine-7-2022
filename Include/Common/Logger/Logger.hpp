/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <fmt/core.h>
#include "Common/Logger/LoggerMessage.hpp"

/*
    Logger

    Writes log messages to multiple outputs for debugging purposes.
    - Logger:  Interface for global logger sink.
    - Message: Single log message that will be written.
    - Output:  Message output streams (e.g. file writer, console, debugger, history).
    - Format:  Defines how log messages are formatted before being written.
    - Sink:    Collects messages and sends them to registered outputs.
*/

namespace Logger
{
    class Sink;
    class History;

    void Initialize();
    void Write(const Message& message);
    int AdvanceFrameReference();

    Sink& GetGlobalSink();
    History& GetGlobalHistory();
    bool IsInitialized();
}

// Utility macros.
#define LOG_EXPAND(expression) expression
#define LOG_STRINGIFY(expression) #expression

#define LOG_SCOPED_INDENT_CONCAT(first, second) first ## second
#define LOG_SCOPED_INDENT_NAME(line) LOG_SCOPED_INDENT_CONCAT(loggerIndent, line)
#define LOG_SCOPED_INDENT() Logger::ScopedIndent LOG_SCOPED_INDENT_NAME(__LINE__)(Logger::GetGlobalSink())

#ifndef NDEBUG
    #define LOG_SCOPED_MESSAGE() Logger::ScopedMessage(Logger::GetGlobalSink()).SetSource(__FILE__).SetLine(__LINE__)
#else
    #define LOG_SCOPED_MESSAGE() Logger::ScopedMessage(Logger::GetGlobalSink())
#endif

#define LOG(format, ...)         LOG_SCOPED_MESSAGE().Format(format, ## __VA_ARGS__)
#define LOG_TRACE(format, ...)   LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Trace)
#define LOG_DEBUG(format, ...)   LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Debug)
#define LOG_PROFILE(format, ...) LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Profile)
#define LOG_INFO(format, ...)    LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Info)
#define LOG_SUCCESS(format, ...) LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Success)
#define LOG_WARNING(format, ...) LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Warning)
#define LOG_ERROR(format, ...)   LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Error)
#define LOG_FATAL(format, ...)   LOG(format, ## __VA_ARGS__).SetSeverity(Logger::Severity::Fatal)
