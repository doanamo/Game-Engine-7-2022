/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <sstream>
#include "Common/NonCopyable.hpp"

/*
    Logger Base Output
    
    Interface for output implementations that are added to logger sinks.
*/

namespace Logger
{
    // Forward declarations.
    class Message;
    struct SinkContext;

    // Output interface.
    class Output : private NonCopyable
    {
    public:
        // Writes a message to an output.
        virtual void Write(const Logger::Message& message, const Logger::SinkContext& context) = 0;
    };
}

/*
    Logger File Output

    Writes log messages to a file.

    void ExampleLoggerFileOutput()
    {
        // Create a logger sink.
        Logger::Sink sink;

        // Open a file output.
        Logger::FileOutput fileOutput;
        fileOutput.Open("Log.txt");

        // Add output to the sink.
        sink.AddOutput(&fileOutput);
    }
*/

namespace Logger
{
    class FileOutput : public Output
    {
    public:
        FileOutput();
        ~FileOutput();

        // Open the file output.
        bool Open(std::string filename);

        // Writes a message to the file.
        void Write(const Message& message, const SinkContext& context) override;

    private:
        // File output stream.
        std::ofstream m_file;
    };
}

/*
    Logger Console Output

    Writes log messsages to the application's console window.

    void ExampleLoggerConsoleOutput()
    {
        // Create a logger sink.
        Logger::Sink sink;

        // Open a console output.
        Logger::ConsoleOutput consoleOutput;

        // Add output to the sink.
        sink.AddOutput(&consoleOutput);
    }
*/

namespace Logger
{
    class ConsoleOutput : public Output
    {
    public:
        ConsoleOutput();
        ~ConsoleOutput();

        // Writes a message to the console window.
        void Write(const Message& message, const SinkContext& context) override;
    };
}

/*
    Logger Debugger Output

    Writes log messages to the debugger's window.

    void ExampleLoggerDebuggerOutput()
    {
        // Create a logger sink.
        Logger::Sink sink;

        // Open a debugger output.
        Logger::DebuggerOutput debuggerOutput;

        // Add output to the sink.
        sink.AddOutput(&debuggerOutput);
    }
*/

namespace Logger
{
    class DebuggerOutput : public Output
    {
    public:
        DebuggerOutput();
        ~DebuggerOutput();

        // Writes a message to the debugger window.
        void Write(const Message& message, const SinkContext& context) override;

    private:
        // Reusable string stream.
        std::stringstream m_stream;
    };
}
