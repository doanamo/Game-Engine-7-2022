/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Logger/Format.hpp"
#include "Logger/Message.hpp"
#include "Logger/Sink.hpp"
using namespace Logger;

namespace
{
    char* MessageSeverityMarker(Severity::Type severity)
    {
        switch(severity)
        {
            case Severity::Trace:   return ".";
            case Severity::Debug:   return "~";
            case Severity::Info:    return "-";
            case Severity::Warning: return "?";
            case Severity::Error:   return "!";
            case Severity::Fatal:   return "X";
        }

        return " ";
    }

    bool CaseInsensitiveCharacterComparison(char a, char b)
    {
        return std::tolower(a) == std::tolower(b);
    };
}

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

    // Print log message legend.
    stream << "Log message legend: ";
    stream << "[" << MessageSeverityMarker(Severity::Trace) << "] Trace, ";
    stream << "[" << MessageSeverityMarker(Severity::Debug) << "] Debug, ";
    stream << "[" << MessageSeverityMarker(Severity::Info) << "] Info, ";
    stream << "[" << MessageSeverityMarker(Severity::Warning) << "] Warning, ";
    stream << "[" << MessageSeverityMarker(Severity::Error) << "] Error, ";
    stream << "[" << MessageSeverityMarker(Severity::Fatal) << "] Fatal";
    stream << "\n";

    // Print log message format.
    stream << "Log message format: ";
    stream << "[Time][Frame][Type] Message {source:line}\n";
    stream << "\n";

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
    stream << "[";
    stream << MessageSeverityMarker(message.GetSeverity());
    stream << "]";

    // Write message indent.
    for(int i = 0; i < context.messageIndent; ++i)
    {
        stream << " ";
    }

    // Write message text.
    stream << " " << message.GetText();

    // Write message source.
    if(message.GetSource())
    {
        std::string sourcePath = message.GetSource();

        // Normalize source path delimiters.
        std::replace(sourcePath.begin(), sourcePath.end(), '\\', '/');

        // Find and remove base path to source directory.
        std::string sourceDir = "Source/";

        auto reverseIt = std::search(
            sourcePath.rbegin(), sourcePath.rend(),
            sourceDir.rbegin(), sourceDir.rend(),
            CaseInsensitiveCharacterComparison
        );

        if(reverseIt != sourcePath.rend())
        {
            reverseIt += sourceDir.length();
        }

        // Find and remove base path to include directory.
        std::string includeDir = "Include/";

        if(reverseIt == sourcePath.rend())
        {
            reverseIt = std::search(
                sourcePath.rbegin(), sourcePath.rend(),
                includeDir.rbegin(), includeDir.rend(),
                CaseInsensitiveCharacterComparison
            );

            if(reverseIt != sourcePath.rend())
            {
                reverseIt += includeDir.length();
            }
        }

        // Remove the base path to a file.
        if(reverseIt != sourcePath.rend())
        {
            sourcePath.erase(sourcePath.begin(), reverseIt.base());
        }

        // Output formatted source path.
        stream << " {";
        stream << sourcePath;
        stream << ":";
        stream << message.GetLine();
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
