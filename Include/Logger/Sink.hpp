/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Logger Sink

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
    // Forward declarations.
    class Output;
    class Message;

    // Sink context structure.
    struct SinkContext
    {
        std::string name;
        int referenceFrame = 0;
        int messageIndent = 0;
        bool messageWritten = false;
    };

    // Sink class.
    class Sink
    {
    public:
        // Type declarations.
        using OutputList = std::vector<Logger::Output*>;

    public:
        Sink();
        ~Sink();

        // Disable copying.
        Sink(const Sink&) = delete;
        Sink& operator=(const Sink&) = delete;

        // Sets sink name.
        void SetName(std::string name);

        // Adds output.
        void AddOutput(Logger::Output* output);

        // Removes output.
        void RemoveOutput(Logger::Output* output);

        // Writes log message.
        void Write(const Logger::Message& message);

        // Advances frame of reference.
        int AdvanceFrameReference();

        // Increase current message indent.
        void IncreaseIndent();

        // Decrease current message indent.
        void DecreaseIndent();

        // Gets context needed for writing messages.
        const SinkContext& GetContext() const;

    private:
        // Sink mutex.
        std::mutex m_lock;

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
    // Scoped indent class.
    class ScopedIndent
    {
    public:
        ScopedIndent(Sink& sink);
        ~ScopedIndent();

        // Disable copying.
        ScopedIndent(const ScopedIndent&) = delete;
        ScopedIndent& operator=(const ScopedIndent&) = delete;

    private:
        // Logger sink.
        Sink& m_sink;
    };
}
