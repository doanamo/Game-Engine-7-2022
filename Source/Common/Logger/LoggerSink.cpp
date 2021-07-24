/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Common/Precompiled.hpp"
#include "Common/Logger/LoggerSink.hpp"
#include "Common/Logger/LoggerOutput.hpp"
#include "Common/Logger/LoggerMessage.hpp"
using namespace Logger;

Sink::Sink() = default;
Sink::~Sink() = default;

void Sink::SetName(std::string name)
{
    std::scoped_lock<std::mutex> lock(m_lock);
    m_context.name = name;
}

void Sink::AddOutput(Logger::Output* output)
{
    assert(output != nullptr && "Attempting to add a null output!");
    
    if(output->Initialize())
    {
        std::scoped_lock<std::mutex> lock(m_lock);
        m_outputs.push_back(output);
    }
}

void Sink::RemoveOutput(Logger::Output* output)
{
    assert(output != nullptr && "Attempting to remove a null output!");

    std::scoped_lock<std::mutex> lock(m_lock);
    m_outputs.erase(std::remove(m_outputs.begin(), m_outputs.end(), output), m_outputs.end());
}

void Sink::Write(const Logger::Message& message)
{
#ifdef NDEBUG
    // Do not print messages of severity debug if not in debug configuration.
    if(message.GetSeverity() == Severity::Debug)
        return;
#endif

    // Write message to all outputs.
    std::scoped_lock<std::mutex> lock(m_lock);
    for(auto output : m_outputs)
    {
        output->Write(message, m_context);
    }

    m_context.messageWritten = true;
}

int Sink::AdvanceFrameReference()
{
    // Advance the frame of reference only if a message has
    // been written since the last time counter was incremented.
    std::scoped_lock<std::mutex> lock(m_lock);
    if(m_context.messageWritten)
    {
        m_context.referenceFrame++;
        m_context.messageWritten = false;
    }

    return m_context.referenceFrame;
}

void Sink::IncreaseIndent()
{
    std::scoped_lock<std::mutex> lock(m_lock);
    m_context.messageIndent++;
}

void Sink::DecreaseIndent()
{
    std::scoped_lock<std::mutex> lock(m_lock);
    if(m_context.messageIndent > 0)
    {
        m_context.messageIndent--;
    }
}

const SinkContext& Sink::GetContext() const
{
    return m_context;
}
