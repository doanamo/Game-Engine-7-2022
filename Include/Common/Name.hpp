/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <string>
#include <fmt/core.h>
#include "Common/Utility.hpp"
#include "Common/NameRegistry.hpp"

/*
    Name

    Constant string literal that is internally represented as hashed identifier for performance
    reasons. It is possible to retrieve the actual string via name registry if enabled.
*/

namespace Common
{
    class Name
    {
    public:
        using HashType = NameRegistry::HashType;

        Name()
            : m_hash(Common::StringHash<HashType>(""))
        {
        }

        Name(const Name& other) = default;
        Name(Name&& other) noexcept = default;
        Name& operator=(const Name& other) = default;
        Name& operator=(Name&& other) noexcept = default;

        Name(const std::string_view string)
            : m_hash(Common::StringHash<HashType>(string))
        {
#ifdef NAME_REGISTRY_ENABLED
            NameRegistry::GetInstance().Register(*this, string);
#endif
        }

        Name(const char* string)
            : m_hash(Common::StringHash<HashType>(string))
        {
#ifdef NAME_REGISTRY_ENABLED
            NameRegistry::GetInstance().Register(*this, string);
#endif
        }

        explicit Name(const HashType hash)
            : m_hash(hash)
        {
#ifdef NAME_REGISTRY_ENABLED
            ASSERT(NameRegistry::GetInstance().IsRegistered(hash),
                "Instantiating name with hash that is not registered!");
#endif
        }

        std::string GetString() const
        {
#ifdef NAME_REGISTRY_ENABLED
            return std::string(NameRegistry::GetInstance().Lookup(m_hash));
#else
            return fmt::format("{{{}}}", m_hash);
#endif
        }

        bool operator==(const Name& other) const
        {
            return m_hash == other.m_hash;
        }

        HashType GetHash() const
        {
            return m_hash;
        }

        operator HashType() const
        {
            return m_hash;
        }

    private:
        HashType m_hash;
    };
}
