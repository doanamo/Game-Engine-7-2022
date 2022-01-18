/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Common/Logger/Logger.hpp"
#include "Common/Logger/LoggerMessage.hpp"
#include "Common/Logger/LoggerSink.hpp"
#include "Common/Logger/LoggerOutput.hpp"
#include "Common/Logger/LoggerHistory.hpp"

namespace
{
    Logger::Sink GlobalSink;
    Logger::History GlobalHistory;
    Logger::FileOutput GlobalFileOutput("Log.txt");
    Logger::ConsoleOutput GlobalConsoleOutput;
    Logger::DebuggerOutput GlobalDebuggerOutput;

    Logger::Mode GlobalLoggerMode = Logger::Mode::Normal;
    bool GlobalLoggerInitialized = false;
}

void Logger::Initialize()
{
    if(GlobalLoggerInitialized)
        return;

    LOG_PROFILE_SCOPE_FUNC();

    // Mark as initialized once done.
    SCOPE_GUARD([]()
    {
        GlobalLoggerInitialized = true;
    });

    // Add default output sinks.
    if(GlobalLoggerMode != Logger::Mode::UnitTests)
    {
        GlobalSink.AddOutput(&GlobalHistory);
        GlobalSink.AddOutput(&GlobalConsoleOutput);
    }

    GlobalSink.AddOutput(&GlobalFileOutput);
    GlobalSink.AddOutput(&GlobalDebuggerOutput);
}

void Logger::Write(const Logger::Message& message)
{
    Initialize();
    GlobalSink.Write(message);
}

int Logger::AdvanceFrameReference()
{
    Initialize();
    return GlobalSink.AdvanceFrameReference();
}

void Logger::SetMode(Mode mode)
{
    GlobalLoggerMode = mode;
}

Logger::Mode Logger::GetMode()
{
    return GlobalLoggerMode;
}

Logger::Sink& Logger::GetGlobalSink()
{
    Initialize();
    return GlobalSink;
}

Logger::History& Logger::GetGlobalHistory()
{
    Initialize();
    return GlobalHistory;
}

bool Logger::IsInitialized()
{
    return GlobalLoggerInitialized;
}
