/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Common/Precompiled.hpp"
#include "Common/Logger/Logger.hpp"
#include "Common/Logger/Output.hpp"
#include "Common/Logger/Message.hpp"
#include "Common/Logger/Format.hpp"
#include "Common/Logger/Sink.hpp"
using namespace Logger;

FileOutput::FileOutput() = default;
FileOutput::~FileOutput()
{
    if(m_file.is_open())
    {
        m_file << DefaultFormat::ComposeSessionEnd();
        m_file.flush();
        m_file.close();
    }
}

bool FileOutput::Open(std::string filename)
{
    assert(!m_file.is_open() && "File stream is already open!");

    m_file.open(filename);
    if(!m_file.is_open())
    {
        assert("Log file output could not be opened!");
        return false;
    }

    m_file << DefaultFormat::ComposeSessionStart();
    m_file.flush();
    return true;
}

void FileOutput::Write(const Message& message, const SinkContext& context)
{
    assert(m_file.is_open() && "File stream is not open!");

    m_file << DefaultFormat::ComposeMessage(message, context);
    m_file.flush();
}

ConsoleOutput::ConsoleOutput() = default;
ConsoleOutput::~ConsoleOutput() = default;

void ConsoleOutput::Write(const Message& message, const SinkContext& context)
{
    std::cout << DefaultFormat::ComposeMessage(message, context);
}

DebuggerOutput::DebuggerOutput() = default;
DebuggerOutput::~DebuggerOutput() = default;

void DebuggerOutput::Write(const Message& message, const SinkContext& context)
{
#ifdef WIN32
    if(!IsDebuggerPresent())
        return;

    std::string output = DefaultFormat::ComposeMessage(message, context);
    OutputDebugStringA(output.c_str());
#endif
}
