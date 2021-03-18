/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Reflection/ReflectionDeclare.hpp"

/*
    Reflection Types

    Built-in types defined out of the box.
*/

namespace Reflection
{
    struct NullType final
    {
        REFLECTION_ENABLE(NullType)
    };

    struct TypeAttribute
    {
        REFLECTION_ENABLE(TypeAttribute)
    };

    struct FieldAttribute
    {
        REFLECTION_ENABLE(FieldAttribute)
    };

    struct MethodAttribute
    {
        REFLECTION_ENABLE(MethodAttribute)
    };
}

REFLECTION_TYPE(Reflection::NullType)
REFLECTION_TYPE(Reflection::TypeAttribute)
REFLECTION_TYPE(Reflection::FieldAttribute)
REFLECTION_TYPE(Reflection::MethodAttribute)
