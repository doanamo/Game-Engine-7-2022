/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
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

    struct GenericAttribute
    {
        REFLECTION_ENABLE(GenericAttribute)

    protected:
        GenericAttribute() = default;
    };

    struct TypeAttribute
    {
        REFLECTION_ENABLE(TypeAttribute)

    protected:
        TypeAttribute() = default;
    };

    struct FieldAttribute
    {
        REFLECTION_ENABLE(FieldAttribute)

    protected:
        FieldAttribute() = default;
    };

    struct MethodAttribute
    {
        REFLECTION_ENABLE(MethodAttribute)

    protected:
        MethodAttribute() = default;
    };
}

// Types declared here must be manually registered in Registry::Register().
REFLECTION_TYPE(Reflection::NullType)
REFLECTION_TYPE(Reflection::GenericAttribute)
REFLECTION_TYPE(Reflection::TypeAttribute)
REFLECTION_TYPE(Reflection::FieldAttribute)
REFLECTION_TYPE(Reflection::MethodAttribute)
