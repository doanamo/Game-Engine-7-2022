/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Logger/Logger.hpp"
#include "Logger/Output.hpp"
#include "Logger/Sink.hpp"
#include "Logger/Message.hpp"

namespace
{
    Logger::Sink GlobalSink;
    Logger::FileOutput GlobalFileOutput;
    Logger::ConsoleOutput GlobalConsoleOutput;
    Logger::DebuggerOutput GlobalDebuggerOutput;

    bool GlobalLoggerInitialized = false;

    void LazyInitialize()
    {
        // Make sure not to initialize twice.
        if(GlobalLoggerInitialized)
            return;

        // Add file output.
        if(GlobalFileOutput.Open("Log.txt"))
        {
            GlobalSink.AddOutput(&GlobalFileOutput);
        }

        // Add console output.
        GlobalSink.AddOutput(&GlobalConsoleOutput);

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

bool Logger::IsInitialized()
{
    return GlobalLoggerInitialized;
}
