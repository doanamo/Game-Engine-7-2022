/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Reflection/ReflectionStatic.hpp"

/*
    Reflection Macros
*/

#define REFLECTION_SUPER(ReflectedBaseType) \
    using Super = ReflectedBaseType;

#define REFLECTION_STORAGE \
        static Reflection::DynamicTypeStorage& GetTypeStorage() \
        { \
            static Reflection::DynamicTypeStorage TypeStorage; \
            return TypeStorage; \
        }

#define REFLECTION_ENABLE_BASE(ReflectedType) \
    public: \
        REFLECTION_SUPER(Reflection::NullType) \
        REFLECTION_STORAGE
    
#define REFLECTION_ENABLE_DERIVED(ReflectedType, ReflectedBaseType) \
    public: \
        REFLECTION_SUPER(ReflectedBaseType) \
        REFLECTION_STORAGE

#define REFLECTION_CHECK_DERIVED(ReflectedType, ReflectedBaseType) \
    static_assert(std::is_same<ReflectedBaseType, Reflection::NullType>::value || \
        std::is_base_of<ReflectedBaseType, ReflectedType>::value, \
        "Incorrect derived class specified for reflected type!"); \
    static_assert(std::is_same<ReflectedBaseType, Reflection::NullType>::value || \
        Reflection::Detail::TypeInfo<ReflectedBaseType>::Reflected, \
        "Base type must be reflected to be used in reflection!");

#define REFLECTION_ENABLE_DEDUCE(_1, _2, _3, ...) _3
#define REFLECTION_ENABLE_CHOOSER(...) REFLECTION_EXPAND( \
    REFLECTION_ENABLE_DEDUCE(__VA_ARGS__, REFLECTION_ENABLE_DERIVED, REFLECTION_ENABLE_BASE))
#define REFLECTION_ENABLE(...) REFLECTION_EXPAND(REFLECTION_ENABLE_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#define REFLECTION_TYPE_INFO_BEGIN(ReflectedType, ReflectedBaseType) \
    template<> struct Reflection::Detail::TypeInfo<ReflectedType> : public TypeInfoBase \
    { \
    private: \
        REFLECTION_CHECK_DERIVED(ReflectedType, ReflectedBaseType) \
        static constexpr std::size_t MemberIndexOffset = __COUNTER__ + 1; \
    public: \
        using Type = ReflectedType; \
        using BaseType = ReflectedBaseType; \
        static constexpr bool Reflected = true; \
        static constexpr std::string_view Name = REFLECTION_STRINGIFY(ReflectedType); \
        template<std::size_t Index, typename Type = ReflectedType, typename Dummy = void> \
        struct MemberInfo;

#define REFLECTION_ATTRIBUTES(...) \
        static constexpr auto Attributes = \
            MakeAttributeList<Reflection::TypeAttribute>(__VA_ARGS__);

#define REFLECTION_TYPE_INFO_END \
        static constexpr std::size_t MemberCount = __COUNTER__ - MemberIndexOffset; \
        static constexpr auto Members = MakeMemberList<Type>( \
            std::make_index_sequence<MemberCount>()); \
    };

#define REFLECTION_TYPE_BASE_BEGIN(ReflectedType) \
    REFLECTION_TYPE_INFO_BEGIN(ReflectedType, Reflection::NullType)

#define REFLECTION_TYPE_DERIVED_BEGIN(ReflectedType, ReflectedBaseType) \
    REFLECTION_TYPE_INFO_BEGIN(ReflectedType, ReflectedBaseType)

#define REFLECTION_TYPE_BEGIN_DEDUCE(_1, _2, _3, ...) _3
#define REFLECTION_TYPE_BEGIN_CHOOSER(...) \
    REFLECTION_EXPAND(REFLECTION_TYPE_BEGIN_DEDUCE(__VA_ARGS__, \
    REFLECTION_TYPE_DERIVED_BEGIN, REFLECTION_TYPE_BASE_BEGIN))
#define REFLECTION_TYPE_BEGIN(...) \
    REFLECTION_EXPAND(REFLECTION_TYPE_BEGIN_CHOOSER(__VA_ARGS__)(__VA_ARGS__))
#define REFLECTION_TYPE_END REFLECTION_TYPE_INFO_END

#define REFLECTION_TYPE_BASE(ReflectedType) \
    REFLECTION_TYPE_INFO_BEGIN(ReflectedType, Reflection::NullType) \
    REFLECTION_TYPE_INFO_END

#define REFLECTION_TYPE_DERIVED(ReflectedType, ReflectedBaseType) \
    REFLECTION_TYPE_INFO_BEGIN(ReflectedType, ReflectedBaseType) \
    REFLECTION_TYPE_INFO_END

#define REFLECTION_TYPE_DEDUCE(_1, _2, _3, ...) _3
#define REFLECTION_TYPE_CHOOSER(...) REFLECTION_EXPAND( \
    REFLECTION_TYPE_DEDUCE(__VA_ARGS__, REFLECTION_TYPE_DERIVED, REFLECTION_TYPE_BASE))
#define REFLECTION_TYPE(...) REFLECTION_EXPAND(REFLECTION_TYPE_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#define REFLECTION_FIELD_BEGIN(Field) \
    template<typename ReflectedType, typename Dummy> \
    struct MemberInfo<__COUNTER__ - MemberIndexOffset, ReflectedType, Dummy> \
    { \
        using Type = decltype(ReflectedType::Field); \
        static constexpr std::string_view Name = REFLECTION_STRINGIFY(Field); \
        static constexpr auto Pointer = &ReflectedType::Field;

#define REFLECTION_FIELD_ATTRIBUTES(...) \
        static constexpr auto Attributes = \
            Reflection::Detail::MakeAttributeList<Reflection::FieldAttribute>(__VA_ARGS__);

#define REFLECTION_FIELD_END \
    };

#define REFLECTION_FIELD(Field, ...) \
    REFLECTION_FIELD_BEGIN(Field) \
    REFLECTION_FIELD_ATTRIBUTES(__VA_ARGS__) \
    REFLECTION_FIELD_END
