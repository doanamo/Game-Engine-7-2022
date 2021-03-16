/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Reflection/Precompiled.hpp"
#include "Reflection/ReflectionDynamic.hpp"
#include "Reflection/ReflectionTypes.hpp"
using namespace Reflection;

bool DynamicTypeInfo::IsNullType() const
{
    return Registered && Identifier == StaticType<NullType>().Identifier;
}

bool DynamicTypeInfo::HasBaseType() const
{
    return Registered && BaseTypeIdentifier != StaticType<NullType>().Identifier;
}

const DynamicTypeInfo& DynamicTypeInfo::GetBaseType() const
{
    return Detail::GetRegistry().LookupType(BaseTypeIdentifier);
}
