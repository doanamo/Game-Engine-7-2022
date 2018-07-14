/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Logger/Logger.hpp"
#include "Logger/Output.hpp"
#include "Logger/Sink.hpp"

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
    if(initialized)
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
    if(!initialized)
    {
        std::cerr << "Default logger has not been initialized yet!";
        DEBUG_BREAK();
    }

    sink.Write(message);
}

int Logger::AdvanceFrameReference()
{
    if (!initialized)
    {
        std::cerr << "Default logger has not been initialized yet!";
        DEBUG_BREAK();
    }

    return sink.AdvanceFrameReference();
}

bool Logger::IsInitialized()
{
    return initialized;
}

Logger::Sink& Logger::GetGlobalSink()
{
    if (!initialized)
    {
        std::cerr << "Default logger has not been initialized yet!";
        DEBUG_BREAK();
    }

    return sink;
}
