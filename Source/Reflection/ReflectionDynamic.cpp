/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Reflection/ReflectionDynamic.hpp"
#include "Reflection/ReflectionTypes.hpp"
#include "Reflection/ReflectionRegistry.hpp"
#include "Reflection/ReflectionUtility.hpp"
using namespace Reflection;

const DynamicTypeInfo DynamicTypeInfo::Invalid{};

void DynamicTypeInfo::Register(const std::string_view name,
                               const InstantiateFunction instantiateFunction,
                               DynamicTypeInfo* baseType)
{
    m_registered = true;
    m_name = Common::Name(name);
    m_instantiateFunction = instantiateFunction;

    if(!IsNullType())
    {
        ASSERT(baseType, "Null base type is only valid for NullType to avoid cyclic dependency!");

        m_baseType = baseType;
        baseType->AddDerivedType(*this);
    }
    else
    {
        m_baseType = this;
    }
}

void DynamicTypeInfo::AddDerivedType(const DynamicTypeInfo& typeInfo)
{
    const auto exiting = std::find_if(
        m_derivedTypes.begin(), m_derivedTypes.end(),
        [&typeInfo](const DynamicTypeList::value_type& derivedType)
        {
            return std::addressof(derivedType.get()) == std::addressof(typeInfo);
        });

    ASSERT(exiting == m_derivedTypes.end(), "Found existing entry in list of derived types!");
    m_derivedTypes.emplace_back(typeInfo);
}

void* DynamicTypeInfo::Instantiate() const
{
    return m_instantiateFunction ? m_instantiateFunction() : nullptr;
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
