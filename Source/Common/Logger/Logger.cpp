/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Common/Precompiled.hpp"
#include "Common/Logger/Logger.hpp"
#include "Common/Logger/Message.hpp"
#include "Common/Logger/Sink.hpp"
#include "Common/Logger/Output.hpp"
#include "Common/Logger/History.hpp"

namespace
{
    Logger::Sink GlobalSink;
    Logger::History GlobalHistory;
    Logger::FileOutput GlobalFileOutput;
    Logger::ConsoleOutput GlobalConsoleOutput;
    Logger::DebuggerOutput GlobalDebuggerOutput;

    bool GlobalLoggerInitialized = false;

    void LazyInitialize()
    {
        // Make sure not to initialize twice.
        if(GlobalLoggerInitialized)
            return;

        // Add message history as output.
        GlobalSink.AddOutput(&GlobalHistory);

        // Add file output.
        if(GlobalFileOutput.Open("Log.txt"))
        {
            GlobalSink.AddOutput(&GlobalFileOutput);
        }

        // Add console output.
        // Console window is disabled by default.
        #if 0
            GlobalSink.AddOutput(&GlobalConsoleOutput);
        #endif

        // Add debugger output.
        GlobalSink.AddOutput(&GlobalDebuggerOutput);

        // Set initialization state.
        GlobalLoggerInitialized = true;
    }
}

void Logger::Initialize()
{
    LazyInitialize();
}

void Logger::Write(const Logger::Message& message)
{
    LazyInitialize();
    GlobalSink.Write(message);
}

int Logger::AdvanceFrameReference()
{
    LazyInitialize();
    return GlobalSink.AdvanceFrameReference();
}

Logger::Sink& Logger::GetGlobalSink()
{
    LazyInitialize();
    return GlobalSink;
}

Logger::History& Logger::GetGlobalHistory()
{
    LazyInitialize();
    return GlobalHistory;
}

bool Logger::IsInitialized()
{
    return GlobalLoggerInitialized;
}
