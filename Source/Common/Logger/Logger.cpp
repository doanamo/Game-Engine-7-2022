/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Common/Precompiled.hpp"
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

    bool GlobalLoggerInitialized = false;

    void LazyInitialize()
    {
        if(GlobalLoggerInitialized)
            return;

        // Profile initialization time.
        auto startTime = std::chrono::steady_clock::now();

        // Add default output sinks.
        GlobalSink.AddOutput(&GlobalHistory);
        GlobalSink.AddOutput(&GlobalFileOutput);
        GlobalSink.AddOutput(&GlobalConsoleOutput);
        GlobalSink.AddOutput(&GlobalDebuggerOutput);

        // Finalize initialization.
        GlobalLoggerInitialized = true;

        LOG("Initialized logger in {:.4f}s.", std::chrono::duration<float>(
            std::chrono::steady_clock::now() - startTime).count());
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
