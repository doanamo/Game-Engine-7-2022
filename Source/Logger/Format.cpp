/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Logger/Format.hpp"
#include "Logger/Message.hpp"
#include "Logger/Sink.hpp"
using namespace Logger;

namespace
{
    const char* MessageSeverityMarker(Severity::Type severity)
    {
        switch(severity)
        {
            case Severity::Trace:   return "t";
            case Severity::Debug:   return "d";
            case Severity::Info:    return "i";
            case Severity::Success: return "s";
            case Severity::Warning: return "w";
            case Severity::Error:   return "e";
            case Severity::Fatal:   return "f";
            default:                return " ";
        }
    }

    bool CaseInsensitiveCharacterComparison(char a, char b)
    {
        return std::tolower(a) == std::tolower(b);
    };
}

std::string DefaultFormat::ComposeSessionStart()
{
    std::string sessionText;

    // Retrieve current system time.
    std::tm time = fmt::localtime(std::time(nullptr));

    // Format session start text.
    sessionText += fmt::format("Session started at {:%Y-%m-%d %H:%M:%S}\n\n", time);

    // Format log message legend text.
    sessionText += fmt::format(
        "Log message legend: [{}] Trace, [{}] Debug, [{}] Info, [{}] Success, [{}] Warning, [{}] Error, [{}] Fatal\n",
        MessageSeverityMarker(Severity::Trace),
        MessageSeverityMarker(Severity::Debug),
        MessageSeverityMarker(Severity::Info),
        MessageSeverityMarker(Severity::Success),
        MessageSeverityMarker(Severity::Warning),
        MessageSeverityMarker(Severity::Error),
        MessageSeverityMarker(Severity::Fatal)
    );

    // Format log message format text.
    sessionText += fmt::format("Log message format: [Time][Frame][Type] Message {{source:line}}\n\n");

    // Return formated string.
    return sessionText;
}

std::string DefaultFormat::ComposeMessage(const Message& message, const SinkContext& context)
{
    std::string messageText;
    messageText.reserve(256);

    // Retrieve current system time.
    // fmt::localtime() is a thread safe version of std::localtime().
    std::tm time = fmt::localtime(std::time(nullptr));

    // Format current time.
    messageText += fmt::format("[{:%H:%M:%S}]", time);

    // Format frame reference.
    messageText += fmt::format("[{:03d}]", context.referenceFrame % 1000);

    // Format message severity.
    messageText += fmt::format("[{}]", MessageSeverityMarker(message.GetSeverity()));

    // Format message text with indent.
    messageText += fmt::format(" {: >{}}{}", "", context.messageIndent, message.GetText());
        
    // Format message source.
    if(message.GetSource())
    {
        std::string sourcePath = message.GetSource();

        // Normalize source path delimiters.
        std::replace(sourcePath.begin(), sourcePath.end(), '\\', '/');

        // Find and remove base path to include directory.
        std::string includeDir = "Include/";

        auto reverseIt = std::search(
            sourcePath.rbegin(), sourcePath.rend(),
            includeDir.rbegin(), includeDir.rend(),
            CaseInsensitiveCharacterComparison
        );

        if(reverseIt != sourcePath.rend())
        {
            reverseIt += includeDir.length();
        }

        // Find and remove base path to source directory.
        if(reverseIt == sourcePath.rend())
        {
            std::string sourceDir = "Source/";

            reverseIt = std::search(
                sourcePath.rbegin(), sourcePath.rend(),
                sourceDir.rbegin(), sourceDir.rend(),
                CaseInsensitiveCharacterComparison
            );

            if(reverseIt != sourcePath.rend())
            {
                reverseIt += sourceDir.length();
            }
        }

        // Remove base path to a file.
        if(reverseIt != sourcePath.rend())
        {
            sourcePath.erase(sourcePath.begin(), reverseIt.base());
        }

        // Format source path.
        messageText += fmt::format(" {{{}:{}}}", sourcePath, message.GetLine());
    }

    // Write message suffix.
    messageText += '\n';

    // Return composed string.
    return messageText;
}

std::string DefaultFormat::ComposeSessionEnd()
{
    std::string sessionText;

    // Retrieve current system time.
    std::tm time = fmt::localtime(std::time(nullptr));

    // Format session end string.
    sessionText += fmt::format("\nSession ended at {:%Y-%m-%d %H:%M:%S}\n\n", time);

    // Return composed string.
    return sessionText;
}
