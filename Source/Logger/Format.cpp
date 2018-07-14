/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Logger/Format.hpp"
#include "Logger/Message.hpp"
#include "Logger/Sink.hpp"
using namespace Logger;

std::string DefaultFormat::ComposeSessionStart()
{
    // Retrieve the current system time.
    time_t timeData = time(nullptr);
    tm* timeInfo = localtime(&timeData);

    // Compose session start string.
    std::stringstream stream;
    stream << "Session started at ";
    stream << std::setfill('0');
    stream << std::setw(4) << timeInfo->tm_year + 1900 << "-";
    stream << std::setw(2) << timeInfo->tm_mon + 1     << "-";
    stream << std::setw(2) << timeInfo->tm_mday        << " ";
    stream << std::setw(2) << timeInfo->tm_hour        << ":";
    stream << std::setw(2) << timeInfo->tm_min         << ":";
    stream << std::setw(2) << timeInfo->tm_sec;
    stream << "\n\n";

    // Return a composed string.
    return stream.str();
}

std::string DefaultFormat::ComposeMessage(const Message& message, const SinkContext& context)
{
    // Retrieve the current system time.
    time_t timeData = time(nullptr);
    tm* timeInfo = localtime(&timeData);

    // Write current time.
    std::stringstream stream;
    stream << "[";
    stream << std::setfill('0');
    stream << std::setw(2) << timeInfo->tm_hour << ":";
    stream << std::setw(2) << timeInfo->tm_min << ":";
    stream << std::setw(2) << timeInfo->tm_sec;
    stream << std::setfill(' ') << std::setw(0);
    stream << "]";

    // Write frame reference.
    stream << "[";
    stream << std::setfill('0') << std::setw(3);
    stream << context.referenceFrame % 1000;
    stream << std::setfill(' ') << std::setw(0);
    stream << "]";

    // Write message severity.
    switch(message.GetSeverity())
    {
    case Severity::Error:
        stream << "[!]";
        break;

    case Severity::Warning:
        stream << "[?]";
        break;

    case Severity::Debug:
        stream << "[~]";
        break;

    default:
        stream << "[-]";
        break;
    }

    // Write message indent.
    for(int i = 0; i < context.messageIndent; ++i)
    {
        stream << "  ";
    }

    // Write message text.
    stream << " " << message.GetText();

    // Write message source.
    if(!message.GetSource().empty())
    {
        stream << " {";
        stream << message.GetSource();

        if(message.GetLine() != 0)
        {
            stream << ":";
            stream << message.GetLine();
        }

        stream << "}";
    }

    // Write message suffix.
    stream << "\n";

    // Return a composed string.
    return stream.str();
}

std::string DefaultFormat::ComposeSessionEnd()
{
    // Retrieve the current system time.
    time_t timeData = time(nullptr);
    tm* timeInfo = localtime(&timeData);

    // Compose session end string.
    std::stringstream stream;
    stream << "\n";
    stream << "Session ended at ";
    stream << std::setfill('0');
    stream << std::setw(4) << timeInfo->tm_year + 1900 << "-";
    stream << std::setw(2) << timeInfo->tm_mon + 1     << "-";
    stream << std::setw(2) << timeInfo->tm_mday        << " ";
    stream << std::setw(2) << timeInfo->tm_hour        << ":";
    stream << std::setw(2) << timeInfo->tm_min         << ":";
    stream << std::setw(2) << timeInfo->tm_sec;
    stream << "\n\n";

    // Return a composed string.
    return stream.str();
}
