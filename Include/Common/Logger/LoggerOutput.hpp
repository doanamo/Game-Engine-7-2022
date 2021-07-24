/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <fstream>

/*
    Logger Output
    
    Writes log messages that are received from logger sink.
*/

namespace Logger
{
    /*
        Output Interface
    */

    class Message;
    struct SinkContext;

    class Output
    {
    public:
        virtual bool Initialize() const = 0;
        virtual void Write(const Logger::Message& message, const Logger::SinkContext& context) = 0;
    };

    /*
        File Output
    */

    class FileOutput : public Output
    {
    public:
        FileOutput(std::string filename);
        ~FileOutput();

        bool Initialize() const override;
        void Write(const Message& message, const SinkContext& context) override;

    private:
        std::ofstream m_file;
    };

    /*
        Console Output
    */

    class ConsoleOutput : public Output
    {
    public:
        ConsoleOutput();
        ~ConsoleOutput();

        bool Initialize() const override;
        void Write(const Message& message, const SinkContext& context) override;
    };

    /*
        Debugger Output
    */

    class DebuggerOutput : public Output
    {
    public:
        DebuggerOutput();
        ~DebuggerOutput();

        bool Initialize() const override;
        void Write(const Message& message, const SinkContext& context) override;
    };
}
