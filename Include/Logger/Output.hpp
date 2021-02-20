/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <fstream>

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
        FileOutput();
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
        DebuggerOutput();
        ~DebuggerOutput();

        void Write(const Message& message, const SinkContext& context) override;
    };
}
