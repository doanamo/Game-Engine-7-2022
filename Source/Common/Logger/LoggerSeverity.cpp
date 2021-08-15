/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Common/Precompiled.hpp"
#include "Common/Logger/LoggerSeverity.hpp"
using namespace Logger;

const char* Logger::GetSeverityName(Severity::Type severity)
{
    switch(severity)
    {
        case Severity::Trace:   return "Trace";
        case Severity::Debug:   return "Debug";
        case Severity::Profile: return "Profile";
        case Severity::Info:    return "Info";
        case Severity::Success: return "Success";
        case Severity::Warning: return "Warning";
        case Severity::Error:   return "Error";
        case Severity::Fatal:   return "Fatal";
        default:                return "Unknown";
    }
}

const char* Logger::GetSeverityMarker(Severity::Type severity)
{
    switch(severity)
    {
        case Severity::Trace:   return "t";
        case Severity::Debug:   return "d";
        case Severity::Profile: return "p";
        case Severity::Info:    return "i";
        case Severity::Success: return "s";
        case Severity::Warning: return "w";
        case Severity::Error:   return "e";
        case Severity::Fatal:   return "f";
        default:                return " ";
    }
}
