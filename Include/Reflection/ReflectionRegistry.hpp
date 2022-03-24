/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <unordered_map>
#include "Reflection/ReflectionDetail.hpp"
#include "Reflection/ReflectionDynamic.hpp"

/*
    Reflection Registry
*/

namespace Reflection
{
    class Registry final : public Detail::Registry
    {
    public:
        using TypeInfoMap = std::unordered_map<TypeIdentifier, DynamicTypeInfo&>;

    public:
        Registry();
        ~Registry();

        template<typename Type>
        bool RegisterType();

        const DynamicTypeInfo& LookupType(TypeIdentifier identifier) const override;
        const TypeInfoMap& GetTypes() const
        {
            return m_types;
        }

    private:
        template<typename Type>
        void ValidateType();

        template<typename Type>
        DynamicTypeInfo* RegisterTypeInfo();

        template<typename Type>
        bool RegisterBasics(DynamicTypeInfo* typeInfo);

        template<typename Type>
        bool RegisterBaseType(DynamicTypeInfo* typeInfo);

        template<typename Type>
        bool RegisterAttributes(DynamicTypeInfo* typeInfo);

        DynamicTypeInfo* FindTypeInfo(TypeIdentifier identifier);

    private:
        TypeInfoMap m_types;
    };

    Registry& GetRegistry();
}

#define REFLECTION_REGISTER_TYPE(Type) \
    Reflection::GetRegistry().RegisterType<Type>()

#include "ReflectionRegistry.inl"
