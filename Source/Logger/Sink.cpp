/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Logger/Sink.hpp"
#include "Logger/Output.hpp"
#include "Logger/Message.hpp"
using namespace Logger;

SinkContext::SinkContext() :
    referenceFrame(0),
    messageIndent(0),
    messageWritten(false)
{
}

Sink::Sink() :
    m_context()
{
}

Sink::~Sink()
{
}

void Sink::SetName(std::string name)
{
    m_context.name = name;
}

void Sink::AddOutput(Logger::Output* output)
{
    VERIFY(output != nullptr, "Attempting to add a null output!");

    // Add an output to the list.
    m_outputs.push_back(output);
}

void Sink::RemoveOutput(Logger::Output* output)
{
    ASSERT(output != nullptr, "Attempting to remove a null output!");

    // Find and remove an output from the list.
    m_outputs.erase(std::remove(m_outputs.begin(), m_outputs.end(), output), m_outputs.end());
}

void Sink::Write(const Logger::Message& message)
{
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
    m_context.messageIndent++;
}

void Sink::DecreaseIndent()
{
    if(m_context.messageIndent > 0)
    {
        m_context.messageIndent--;
    }
}

const SinkContext& Sink::GetContext() const
{
    return m_context;
}

ScopedIndent::ScopedIndent(Sink* sink) :
    m_sink(sink)
{
    VERIFY(m_sink != nullptr, "Argument for sink reference cannot be nullptr!");

    m_sink->IncreaseIndent();
}

ScopedIndent::~ScopedIndent()
{
    VERIFY(m_sink != nullptr, "Saved sink reference somehow became nullptr!");

    m_sink->DecreaseIndent();
}
