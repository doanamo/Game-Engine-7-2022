/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <vector>
#include <mutex>

/*
    Sink

    Writes log messages to multiple logging outputs.

    void ExampleLoggerSink()
    {
        // Create logging sink.
        Logger::Sink sink;

        // Open file output.
        Logger::FileOutput fileOutput;
        fileOutput.Open("Log.txt");

        // Add output to the sink.
        sink.AddOutput(&fileOutput);

        // Write log message.
        Logger::Message message;
        message.Format("Hello {}!", "world");
        sink.Write(message);
    }
*/

namespace Logger
{
    class Output;
    class Message;

    struct SinkContext
    {
        std::string name;
        int referenceFrame = 0;
        int messageIndent = 0;
        bool messageWritten = false;
    };

    class Sink
    {
    public:
        using OutputList = std::vector<Logger::Output*>;

        Sink();
        ~Sink();

        Sink(const Sink&) = delete;
        Sink& operator=(const Sink&) = delete;

        void SetName(std::string name);
        void AddOutput(Logger::Output* output);
        void RemoveOutput(Logger::Output* output);
        void Write(const Logger::Message& message);
        int AdvanceFrameReference();
        void IncreaseIndent();
        void DecreaseIndent();

        const SinkContext& GetContext() const;

    private:
        std::mutex m_lock;
        SinkContext m_context;
        OutputList m_outputs;
    };
}

/*
    Scoped Indent

    Increases logging indent in a sink for a duration of a scope.

    void ExampleLoggerScopedIndent()
    {
        // Initialize logging system.
        Logger::Initialize();

        // Use macro to create an indent.
        LOG("Initializing nothingness...");
        LOG_SCOPED_INDENT();

        // Write log message.
        LOG_DEBUG("Success!");
    }
*/

namespace Logger
{
    class ScopedIndent
    {
    public:
        ScopedIndent(Sink& sink);
        ~ScopedIndent();

        ScopedIndent(const ScopedIndent&) = delete;
        ScopedIndent& operator=(const ScopedIndent&) = delete;

    private:
        Sink& m_sink;
    };
}
