/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Reflection/ReflectionRegistry.hpp"
#include "Reflection/ReflectionTypes.hpp"
using namespace Reflection;

Registry& Reflection::GetRegistry()
{
    static Registry registry;
    return registry;
}

const Detail::ReflectionRegistry& Detail::GetRegistry()
{
    return Reflection::GetRegistry();
}

Registry::Registry()
{
    RegisterType<Reflection::NullType>();
}

Registry::~Registry() = default;

DynamicTypeInfo* Registry::FindTypeInfo(const TypeIdentifier identifier)
{
    auto it = m_types.find(identifier);
    if(it == m_types.end())
    {
        return nullptr;
    }

    return &it->second;
}

const DynamicTypeInfo& Registry::LookupType(const TypeIdentifier identifier) const
{
    const auto it = m_types.find(identifier);
    if(it == m_types.end())
    {
        return DynamicTypeInfo::Invalid;
    }

    return it->second;
}
