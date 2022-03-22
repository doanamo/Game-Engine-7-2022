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
    class NullType final
    {
        REFLECTION_ENABLE(NullType)
    };

    class BaseAttribute
    {
        REFLECTION_ENABLE(BaseAttribute)

    protected:
        BaseAttribute() = default;
    };

    class GenericAttribute : public BaseAttribute
    {
        REFLECTION_ENABLE(GenericAttribute, BaseAttribute)

    protected:
        GenericAttribute() = default;
    };

    class TypeAttribute : public BaseAttribute
    {
        REFLECTION_ENABLE(TypeAttribute, BaseAttribute)

    protected:
        TypeAttribute() = default;
    };

    class FieldAttribute : public BaseAttribute
    {
        REFLECTION_ENABLE(FieldAttribute, BaseAttribute)

    protected:
        FieldAttribute() = default;
    };

    class MethodAttribute : public BaseAttribute
    {
        REFLECTION_ENABLE(MethodAttribute, BaseAttribute)

    protected:
        MethodAttribute() = default;
    };
}

// Types declared here must be manually registered in Registry::Register().
REFLECTION_TYPE(Reflection::NullType)
REFLECTION_TYPE(Reflection::BaseAttribute)
REFLECTION_TYPE(Reflection::GenericAttribute, Reflection::BaseAttribute)
REFLECTION_TYPE(Reflection::TypeAttribute, Reflection::BaseAttribute)
REFLECTION_TYPE(Reflection::FieldAttribute, Reflection::BaseAttribute)
REFLECTION_TYPE(Reflection::MethodAttribute, Reflection::BaseAttribute)
