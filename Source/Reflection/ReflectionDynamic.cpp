/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Reflection/Precompiled.hpp"
#include "Reflection/ReflectionDynamic.hpp"
#include "Reflection/ReflectionTypes.hpp"
using namespace Reflection;

const DynamicTypeInfo DynamicTypeInfo::Invalid;

void DynamicTypeInfo::Register(std::string_view name,
    IdentifierType identifier, DynamicTypeInfo* baseType)
{
    m_registered = true;
    m_name = name;
    m_identifier = identifier;

    if(!IsNullType())
    {
        m_baseType = baseType;

        auto ContainsThisDerivedType = [&derivedTypes = baseType->m_derivedTypes, this]() -> bool
        {
            auto it = std::find(derivedTypes.begin(), derivedTypes.end(), this);
            return it != derivedTypes.end();
        };

        ASSERT(!ContainsThisDerivedType(), "Found same existing entry in list of derived types!");
        baseType->m_derivedTypes.push_back(this);
    }
    else
    {
        m_baseType = this;
    }
}
