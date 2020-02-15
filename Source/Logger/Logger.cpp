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
    // Global logger settings.
    static const bool AllowLazyInitialization = true;

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
    // This methods gets called whenever other functions are called as part of
    // the lazy initialization process. This is done so we can still handle cases
    // when logging macros (e.g. for ASSERT() or VERIFY()) are used before the
    // official initialization takes place. If we ever decide to change how logs
    // are initialized (by letting the user change initialization settings), then
    // we will have to take this into consideration when rewriting this code.

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
    if(AllowLazyInitialization)
    {
        Initialize();
    }

    if(!initialized)
    {
        std::cerr << "Default logger has not been initialized yet!";
        DEBUG_BREAK();
    }

    sink.Write(message);
}

int Logger::AdvanceFrameCounter()
{
    if(AllowLazyInitialization)
    {
        Initialize();
    }

    if(!initialized)
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
    if(AllowLazyInitialization)
    {
        Initialize();
    }

    if(!initialized)
    {
        std::cerr << "Default logger has not been initialized yet!";
        DEBUG_BREAK();
    }

    return sink;
}
