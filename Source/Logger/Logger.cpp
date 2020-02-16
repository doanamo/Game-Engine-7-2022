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
    // Default logger sink.
    Logger::Sink sink;

    // Default logger outputs.
    Logger::FileOutput fileOutput;
    Logger::ConsoleOutput consoleOutput;
    Logger::DebuggerOutput debuggerOutput;

    // Initialization state.
    bool initialized = false;
}

void Logger::Initialize()
{
    // Make sure not to initialize twice.
    if(IsInitialized())
        return;

    // Add the file output.
    if(fileOutput.Open("Log.txt"))
    {
        sink.AddOutput(&fileOutput);
    }

    // Add the console output.
    sink.AddOutput(&consoleOutput);

    // Add the debugger output.
    sink.AddOutput(&debuggerOutput);

    // Set initialization state.
    initialized = true;
}

void Logger::Write(const Logger::Message& message)
{
    Logger::Initialize();
    sink.Write(message);
}

int Logger::AdvanceFrameReference()
{
    Logger::Initialize();
    return sink.AdvanceFrameReference();
}

Logger::Sink& Logger::GetGlobalSink()
{
    Logger::Initialize();
    return sink;
}

bool Logger::IsInitialized()
{
    return initialized;
}
