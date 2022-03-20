/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Reflection/ReflectionDetail.hpp"
#include "Reflection/ReflectionRegistry.hpp"
using namespace Reflection;

const Detail::ReflectionRegistry& Detail::GetRegistry()
{
    return Reflection::GetRegistry();
}
