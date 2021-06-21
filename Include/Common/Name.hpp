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

    Note: Until consteval is introduced with C++20, NAME_CONSTEXPR() macro must be used to
    ensure that hash calculation for name is calculated at compile time. This applies only
    when name registry is disabled, otherwise constexpr cannot be enabled.
*/

#ifdef NAME_REGISTRY_ENABLED
    #define NAME_REGISTRY_CONSTEXPR
#else
    #define NAME_REGISTRY_CONSTEXPR constexpr
#endif

namespace Common
{
    class Name
    {
    public:
        using HashType = NameRegistry::HashType;
        static constexpr HashType EmptyHash = Common::StringHash<HashType>("");

        constexpr Name() noexcept = default;
        constexpr Name(const Name& other) = default;
        constexpr Name(Name&& other) noexcept = default;
        constexpr Name& operator=(const Name& other) = default;
        constexpr Name& operator=(Name&& other) noexcept = default;

        NAME_REGISTRY_CONSTEXPR Name(const char* string)
            : m_hash(Common::StringHash<HashType>(string))
        {
#ifdef NAME_REGISTRY_ENABLED
            NameRegistry::GetInstance().Register(*this, string);
#endif
        }

        NAME_REGISTRY_CONSTEXPR Name(const std::string_view string)
            : m_hash(Common::StringHash<HashType>(string))
        {
#ifdef NAME_REGISTRY_ENABLED
            NameRegistry::GetInstance().Register(*this, string);
#endif
        }

        NAME_REGISTRY_CONSTEXPR Name(const HashType hash)
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

        constexpr bool operator==(const Name& other) const
        {
            return m_hash == other.m_hash;
        }

        constexpr bool operator==(const HashType hash) const
        {
            return m_hash == hash;
        }

        constexpr HashType GetHash() const
        {
            return m_hash;
        }

        constexpr operator HashType() const
        {
            return m_hash;
        }

    private:
        HashType m_hash = EmptyHash;
    };
}

#ifdef NAME_REGISTRY_ENABLED
    #define NAME_CONSTEXPR(string) Common::Name(string)
#else
    #define NAME_CONSTEXPR(string) \
        [&]() { \
            NAME_REGISTRY_CONSTEXPR Common::Name name = \
                Common::StringHash<Common::Name::HashType>(string); \
            return name; \
        }()
#endif

namespace std
{
    template<>
    struct hash<Common::Name>
    {
        std::size_t operator()(const Common::Name& name) const noexcept
        {
            return name.GetHash();
        }
    };
}
