/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Logger/Sink.hpp"
#include "Logger/Output.hpp"
#include "Logger/Message.hpp"
using namespace Logger;

void Sink::SetName(std::string name)
{
    std::scoped_lock<std::mutex> lock(m_lock);

    m_context.name = name;
}

void Sink::AddOutput(Logger::Output* output)
{
    assert(output != nullptr && "Attempting to add a null output!");

    std::scoped_lock<std::mutex> lock(m_lock);

    // Add an output to the list.
    m_outputs.push_back(output);
}

void Sink::RemoveOutput(Logger::Output* output)
{
    assert(output != nullptr && "Attempting to remove a null output!");

    std::scoped_lock<std::mutex> lock(m_lock);

    // Find and remove an output from the list.
    m_outputs.erase(std::remove(m_outputs.begin(), m_outputs.end(), output), m_outputs.end());
}

void Sink::Write(const Logger::Message& message)
{
    std::scoped_lock<std::mutex> lock(m_lock);

    // Do not print messages of severity debug if not in debug configuration.
#ifdef NDEBUG
    if(message.GetSeverity() == Severity::Debug)
        return;
#endif

    // Write a message to all outputs.
    for(auto output : m_outputs)
    {
        output->Write(message, m_context);
    }

    m_context.messageWritten = true;
}

int Sink::AdvanceFrameReference()
{
    std::scoped_lock<std::mutex> lock(m_lock);

    // Advance the frame of reference only if a message has
    // been written since the last time counter was incremented.
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

ScopedIndent::ScopedIndent(Sink& sink) :
    m_sink(sink)
{
    m_sink.IncreaseIndent();
}

ScopedIndent::~ScopedIndent()
{
    m_sink.DecreaseIndent();
}
