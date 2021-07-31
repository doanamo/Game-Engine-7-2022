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
        ScopedLogProfile(std::string name)
            : m_time(std::chrono::steady_clock::now())
            , m_name(name)
        {
        }

        ~ScopedLogProfile()
        {
            LOG_PROFILE("{} took {:.4f}s.", m_name, std::chrono::duration<float>(
                std::chrono::steady_clock::now() - m_time).count());
        }

    private:
        const TimeType m_time;
        const std::string m_name;
    };
}

// Utility macros.
#ifndef NDEBUG
    #define LOG_PROFILE_SCOPE(name, ...) \
        Common::ScopedLogProfile scopedLogProfile(fmt::format(name, ## __VA_ARGS__))
#else
    #define LOG_PROFILE_SCOPE(name, ...)
#endif
