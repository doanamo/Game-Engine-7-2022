/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Reflection/Precompiled.hpp"
#include "Reflection/ReflectionRegistry.hpp"
#include "Reflection/ReflectionTypes.hpp"
using namespace Reflection;

DynamicTypeInfo ReflectionRegistry::InvalidTypeInfo;

ReflectionRegistry& Reflection::GetRegistry()
{
    static ReflectionRegistry registry;
    return registry;
}

const Detail::ReflectionRegistry& Detail::GetRegistry()
{
    return Reflection::GetRegistry();
}

ReflectionRegistry::ReflectionRegistry()
{
    RegisterType<Reflection::NullType>();
    RegisterType<Reflection::TypeAttribute>();
    RegisterType<Reflection::FieldAttribute>();
    RegisterType<Reflection::MethodAttribute>();
}

ReflectionRegistry::~ReflectionRegistry() = default;

const DynamicTypeInfo& ReflectionRegistry::LookupType(IdentifierType identifier) const
{
    auto it = m_registry.find(identifier);
    if(it == m_registry.end())
    {
        return InvalidTypeInfo;
    }

    return it->second;
}
