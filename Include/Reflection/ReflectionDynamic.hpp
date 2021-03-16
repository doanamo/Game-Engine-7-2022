/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Reflection/ReflectionStatic.hpp"

/*
    Reflection Dynamic

    Run-time reflection interface.
*/

namespace Reflection
{
    struct DynamicTypeInfo
    {
        DynamicTypeInfo() = default;

        template<typename Type>
        DynamicTypeInfo(const TypeDescription<Type>& staticType) :
            Reflected(staticType.Reflected),
            Name(staticType.Name),
            Identifier(staticType.Identifier)
        {
        }

        bool Reflected = false;
        std::string_view Name = "<UnregisteredType>";
        IdentifierType Identifier = InvalidIdentifier;
    };

    inline const DynamicTypeInfo& Reflect(IdentifierType identifier)
    {
        return Detail::GetRegistry().LookupType(identifier);
    }
}
