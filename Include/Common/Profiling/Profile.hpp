/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Scoped Log Profile
*/

namespace Common
{
    class ScopedLogProfile final
    {
    public:
        using TimeType = std::chrono::time_point<std::chrono::steady_clock>;

    public:
        ScopedLogProfile(std::string name, const char* source, unsigned int line)
            : m_time(std::chrono::steady_clock::now())
            , m_name(name)
            , m_source(source)
            , m_line(line)
        {
        }

        ~ScopedLogProfile()
        {
            float duration = std::chrono::duration<float>(
                std::chrono::steady_clock::now() - m_time).count();

            Logger::ScopedMessage(Logger::GetGlobalSink())
                .Format("{} took {:.4f}s.", m_name, duration)
                .SetSeverity(Logger::Severity::Profile)
                .SetSource(m_source)
                .SetLine(m_line);
        }

    private:
        TimeType m_time;
        std::string m_name;
        const char* m_source;
        unsigned int m_line;
    };
}

// Utility macros.
#ifndef CONFIG_RELEASE
    #define LOG_PROFILE_SCOPE_NAME(name, ...) \
        Common::ScopedLogProfile scopedLogProfile( \
            fmt::format(name, ## __VA_ARGS__), __FILE__, __LINE__)

    #define LOG_PROFILE_SCOPE_FUNC() \
        Common::ScopedLogProfile scopedLogProfile( \
            fmt::format("{}()", __FUNCTION__), __FILE__, __LINE__)
#else
    #define LOG_PROFILE_SCOPE_NAME(...) ((void)0)
    #define LOG_PROFILE_SCOPE_FUNC(...) ((void)0)
#endif
