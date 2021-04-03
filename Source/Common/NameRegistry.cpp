/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Precompiled.hpp"
#include "Common/NameRegistry.hpp"
#include "Common/Name.hpp"
using namespace Common;

NameRegistry::NameRegistry()
{
#ifdef NAME_REGISTRY_ENABLED
    m_registry.emplace(Common::StringHash<HashType>(""), "");
#endif
}

NameRegistry::~NameRegistry() = default;

NameRegistry& NameRegistry::GetInstance()
{
    static NameRegistry instance;
    return instance;
}

#ifdef NAME_REGISTRY_ENABLED

void NameRegistry::Register(std::string_view string)
{
    Name registrar(string);
}

void NameRegistry::Register(const Name& name, std::string_view string)
{
    auto it = m_registry.find(name.GetHash());
    if(it != m_registry.end())
    {
        ASSERT_EVALUATE(it->second == string, "Detected name hash collision!");
    }
    else
    {
        auto result = m_registry.emplace(name.GetHash(), string);
        ASSERT(result.second, "Name registry insertion failed!");
    }
}

std::string_view NameRegistry::Lookup(HashType hash)
{
    auto it = m_registry.find(hash);
    if(it == m_registry.end())
        return {};

    return it->second;
}

bool NameRegistry::IsRegistered(std::string_view string)
{
    return IsRegistered(Common::StringHash<HashType>(string));
}

bool NameRegistry::IsRegistered(HashType hash)
{
    return m_registry.find(hash) != m_registry.end();
}

#endif // NAME_REGISTRY_ENABLED
