/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <string>
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

        Name(const Name& other)
            : m_hash(other.m_hash)
        {
        }

        Name(const std::string_view string)
            : m_hash(Common::StringHash<HashType>(string))
        {
#ifdef NAME_REGISTRY_ENABLED
            NameRegistry::GetInstance().Register(*this, string);
#endif
        }

        Name(const HashType hash)
            : m_hash(hash)
        {
#ifdef NAME_REGISTRY_ENABLED
            ASSERT(NameRegistry::GetInstance().IsRegistered(hash),
                "Instantiating name with hash that is not registered!");
#endif
        }

#ifdef NAME_REGISTRY_ENABLED
        std::string_view GetString() const
        {
            return NameRegistry::GetInstance().Lookup(m_hash);
        }
#endif

        bool operator==(const Name& other) const
        {
            return m_hash == other.m_hash;
        }

        Name& operator=(const Name& other)
        {
            m_hash = other.m_hash;
            return *this;
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
