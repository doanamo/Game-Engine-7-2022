/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Script Attribute

    Marks reflected types and methods for binding to script interface.
*/

namespace Core
{
    class ScriptState;

    class ScriptBinding : public Reflection::GenericAttribute
    {
        REFLECTION_ENABLE(ScriptBinding, Reflection::GenericAttribute)

    public:
        constexpr ScriptBinding(const std::string_view name = "")
            : Name(name)
        {
        }

        const std::string_view Name;
    };

    bool BindScriptInterface(ScriptState& scriptState);
}

REFLECTION_TYPE(Core::ScriptBinding, Reflection::GenericAttribute)
