/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Reflection/ReflectionDynamic.hpp"
#include "Reflection/ReflectionTypes.hpp"
#include "Reflection/ReflectionRegistry.hpp"
#include "Reflection/ReflectionUtility.hpp"
using namespace Reflection;

const DynamicTypeInfo DynamicTypeInfo::Invalid{};

void DynamicTypeInfo::SetName(const Common::Name& name)
{
    ASSERT(!m_registered);
    m_name = name;
}

void DynamicTypeInfo::SetConstructible(ConstructFunction constructFunction)
{
    ASSERT(!m_registered);
    m_constructFunction = constructFunction;
}

void DynamicTypeInfo::SetBaseType(DynamicTypeInfo* baseType)
{
    ASSERT(!m_registered);
    ASSERT(m_name != Invalid.GetName());

    if(!IsNullType())
    {
        ASSERT(baseType != nullptr, "Empty base type is only valid for NullType!");

        m_baseType = baseType;
        baseType->AddDerivedType(this);
    }
    else
    {
        m_baseType = this;
    }
}

void DynamicTypeInfo::AddAttribute(const BaseAttribute* attribute)
{
    ASSERT(!m_registered);

#ifndef CONFIG_RELEASE
    const auto it = std::find_if(m_attributes.begin(), m_attributes.end(),
        [attribute](const AttributeList::value_type& registeredAttribute)
        {
            // Compare pointers to static instances.
            return registeredAttribute == attribute;
        }
    );

    ASSERT(it == m_attributes.end(), "Found existing entry in list of attributes!");
#endif

    m_attributes.emplace_back(attribute);
}

void DynamicTypeInfo::AddDerivedType(const DynamicTypeInfo* typeInfo)
{
    ASSERT(m_registered);

#ifndef CONFIG_RELEASE
    const auto it = std::find_if(m_derivedTypes.begin(), m_derivedTypes.end(),
        [typeInfo](const DynamicTypeList::value_type& registeredDerivedType)
        {
            // Compare pointers to static instances.
            return registeredDerivedType == typeInfo;
        }
    );

    ASSERT(it == m_derivedTypes.end(), "Found existing entry in list of derived types!");
#endif

    m_derivedTypes.emplace_back(typeInfo);
}

void DynamicTypeInfo::MarkRegistered()
{
    ASSERT(!m_registered, "Cannot register same dynamic type info twice!");
    m_registered = true;
}

void* DynamicTypeInfo::Construct() const
{
    ASSERT(m_registered);
    return m_constructFunction ? m_constructFunction() : nullptr;
}

bool DynamicTypeInfo::IsNullType() const
{
    return GetIdentifier() == Reflection::GetIdentifier<NullType>();
}

bool DynamicTypeInfo::IsType(const TypeIdentifier identifier) const
{
    if(!m_registered)
        return false;

    if(GetIdentifier() == identifier)
        return true;

    return IsDerivedFrom(identifier);
}

bool DynamicTypeInfo::IsBaseOf(const TypeIdentifier identifier) const
{
    if(!m_registered)
        return false;

    const DynamicTypeInfo& typeInfo = Reflection::GetRegistry().LookupType(identifier);
    return typeInfo.IsDerivedFrom(GetIdentifier());
}

bool DynamicTypeInfo::IsDerivedFrom(const TypeIdentifier identifier) const
{
    if(!m_registered)
        return false;

    const DynamicTypeInfo* baseType = m_baseType;
    while(!baseType->IsNullType())
    {
        if(baseType->GetIdentifier() == identifier)
        {
            return true;
        }

        baseType = &baseType->GetBaseType();
    }

    return false;
}

bool DynamicTypeInfo::HasAttribute(TypeIdentifier identifier) const
{
    return GetAttribute(identifier) != nullptr;
}

const BaseAttribute* DynamicTypeInfo::GetAttribute(TypeIdentifier identifier) const
{
    const auto it = std::find_if(m_attributes.begin(), m_attributes.end(),
        [identifier](const AttributeList::value_type& registeredAttribute)
        {
            return Reflection::GetIdentifier(registeredAttribute) == identifier;
        }
    );

    return it != m_attributes.end() ? *it : nullptr;
}

const Reflection::BaseAttribute* DynamicTypeInfo::GetAttributeByIndex(size_t index) const
{
    if(index >= m_attributes.size())
        return nullptr;

    return m_attributes.at(index);
}
