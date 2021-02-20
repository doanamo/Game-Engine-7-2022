/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Common/Precompiled.hpp"
#include "Common/Logger/Message.hpp"
#include "Common/Logger/Sink.hpp"
using namespace Logger;

Message::Message(Message&& other)
{
    m_text = std::move(other.m_text);

    m_severity = other.m_severity;
    other.m_severity = Severity::Info;

    m_source = other.m_source;
    other.m_source = nullptr;

    m_line = other.m_line;
    other.m_line = 0;
}
