/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <sstream>

/*
    Base Output
    
    Interface for output implementations that are added to logger sinks.
*/

namespace Logger
{
    class Message;
    struct SinkContext;

    class Output
    {
    public:
        virtual void Write(const Logger::Message& message, const Logger::SinkContext& context) = 0;
    };
}

/*
    File Output

    Writes log messages to a file.

    void ExampleLoggerFileOutput()
    {
        // Create logger sink.
        Logger::Sink sink;

        // Open file output.
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
        FileOutput() = default;
        ~FileOutput();

        bool Open(std::string filename);
        void Write(const Message& message, const SinkContext& context) override;

    private:
        std::ofstream m_file;
    };
}

/*
    Console Output

    Writes log messages to the application's console window.

    void ExampleLoggerConsoleOutput()
    {
        // Create logger sink.
        Logger::Sink sink;

        // Open console output.
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

        void Write(const Message& message, const SinkContext& context) override;
    };
}

/*
    Debugger Output

    Writes log messages to the debugger's window.

    void ExampleLoggerDebuggerOutput()
    {
        // Create logger sink.
        Logger::Sink sink;

        // Open debugger output.
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
        DebuggerOutput() = default;
        ~DebuggerOutput() = default;

        void Write(const Message& message, const SinkContext& context) override;

    private:
        std::stringstream m_stream;
    };
}
