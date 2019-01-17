/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

#include <vector>
#include "Common/NonCopyable.hpp"

/*
    Logger Sink

    Writes log messages to multiple logging outputs.

    void ExampleLoggerSink()
    {
        // Create a logging sink.
        Logger::Sink sink;

        // Open a file output.
        Logger::FileOutput fileOutput;
        fileOutput.Open("Log.txt");

        // Add output to the sink.
        sink.AddOutput(&fileOutput);

        // Write a log message.
        Logger::Message message;
        message << "Hello world!";
        sink.Write(message);
    }
*/

namespace Logger
{
    // Forward declarations.
    class Output;
    class Message;

    // Sink context structure.
    struct SinkContext
    {
        SinkContext();

        std::string name;
        int referenceFrame;
        int messageIndent;
        bool messageWritten;
    };

    // Sink class.
    class Sink : private NonCopyable
    {
    public:
        // Type declarations.
        using OutputList = std::vector<Logger::Output*>;

    public:
        Sink();
        ~Sink();

        // Sets the sink name.
        void SetName(std::string name);

        // Adds an output.
        void AddOutput(Logger::Output* output);

        // Removes an output.
        void RemoveOutput(Logger::Output* output);

        // Writes a log message.
        void Write(const Logger::Message& message);

        // Advance the frame of reference.
        int AdvanceFrameReference();

        // Increase the current message indent.
        void IncreaseIndent();

        // Decrease the current message indent.
        void DecreaseIndent();

        // Gets the context needed for writting messages.
        const SinkContext& GetContext() const;

    private:
        // Sink context.
        SinkContext m_context;

        // List of outputs.
        OutputList m_outputs;
    };
}

/*
    Logger Scoped Indent

    Increases logging indent in a sink for a duration of a scope.

    void ExampleLoggerScopedIndent()
    {
        // Initialize the logging system.
        Logger::Initialize();

        // Using a macro to create an indent.
        LOG() << "Initializing nothingness..." << LOG_INDENT();

        // Write a log message.
        LOG_DEBUG() << "Success!";
    }
*/

namespace Logger
{
    // Scoped indent class.
    class ScopedIndent : private NonCopyable
    {
    public:
        ScopedIndent(Sink* sink);
        ~ScopedIndent();

    private:
        // Logger sink.
        Sink* m_sink;
    };
}
