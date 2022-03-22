/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
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

Registry::Registry()
{
    RegisterType<Reflection::NullType>();
    RegisterType<Reflection::BaseAttribute>();
    RegisterType<Reflection::GenericAttribute>();
    RegisterType<Reflection::TypeAttribute>();
    RegisterType<Reflection::FieldAttribute>();
    RegisterType<Reflection::MethodAttribute>();
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
