/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

class Undefined
{
};

class Empty
{
    REFLECTION_ENABLE(Empty)
};

REFLECTION_TYPE_BEGIN(Empty)
REFLECTION_TYPE_END

class BaseAttribute : public Reflection::TypeAttribute
{
    REFLECTION_ENABLE(BaseAttribute, Reflection::TypeAttribute)

public:
    bool operator==(const BaseAttribute& other) const
    {
        return true;
    }
};

REFLECTION_TYPE(BaseAttribute, Reflection::TypeAttribute)

class TextAttribute : public Reflection::FieldAttribute
{
    REFLECTION_ENABLE(TextAttribute, Reflection::FieldAttribute)

public:
    bool operator==(const TextAttribute& other) const
    {
        return true;
    }
};

REFLECTION_TYPE(TextAttribute, Reflection::FieldAttribute)

class Base
{
    REFLECTION_ENABLE(Base)

public:
    std::string textWithoutAttribute;
    const char* textPtrWithAttribute;
};

REFLECTION_TYPE_BEGIN(Base)
    REFLECTION_ATTRIBUTES(BaseAttribute())
    REFLECTION_FIELD(textWithoutAttribute)
    REFLECTION_FIELD(textPtrWithAttribute, TextAttribute())
REFLECTION_TYPE_END

class DerivedAttribute : public Reflection::TypeAttribute
{
    REFLECTION_ENABLE(DerivedAttribute, Reflection::TypeAttribute)

public:
    constexpr DerivedAttribute() = default;
    constexpr DerivedAttribute(bool state) :
        state(state)
    {
    }

    const bool state = false;
};

REFLECTION_TYPE(DerivedAttribute, Reflection::TypeAttribute)

class CounterAttribute : public Reflection::FieldAttribute
{
    REFLECTION_ENABLE(CounterAttribute, Reflection::FieldAttribute)

public:
    constexpr CounterAttribute() = default;
    constexpr CounterAttribute(bool state) :
        state(state)
    {
    }

    const bool state = false;
};

REFLECTION_TYPE(CounterAttribute, Reflection::FieldAttribute)

class Derived : public Base
{
    REFLECTION_ENABLE(Derived, Base)

public:
    Derived()
    {
    }

public:
    int counter;
};

REFLECTION_TYPE_BEGIN(Derived, Base)
    REFLECTION_ATTRIBUTES(DerivedAttribute(false))
    REFLECTION_FIELD(counter, CounterAttribute(true))
REFLECTION_TYPE_END

class InnerAttribute : public Reflection::FieldAttribute
{
    REFLECTION_ENABLE(InnerAttribute, Reflection::FieldAttribute)

public:
    constexpr InnerAttribute() = default;
    constexpr InnerAttribute(int counter) :
        counter(counter)
    {
    }

    const int counter = 0;
};

REFLECTION_TYPE(InnerAttribute, Reflection::FieldAttribute)

class Inner
{
    REFLECTION_ENABLE(Inner)

public:
    uint8_t value;
};

REFLECTION_TYPE_BEGIN(Inner)
    REFLECTION_FIELD(value, InnerAttribute(20))
REFLECTION_TYPE_END

class ToggleOnAttribute : public Reflection::FieldAttribute
{
    REFLECTION_ENABLE(ToggleOnAttribute, Reflection::FieldAttribute)

public:
    const bool state = true;
};

REFLECTION_TYPE(ToggleOnAttribute, Reflection::FieldAttribute)

class ToggleOffAttribute : public Reflection::FieldAttribute
{
    REFLECTION_ENABLE(ToggleOffAttribute, Reflection::FieldAttribute)

public:
    const bool state = false;
};

REFLECTION_TYPE(ToggleOffAttribute, Reflection::FieldAttribute)

class BranchedOne : public Derived
{
    REFLECTION_ENABLE(BranchedOne, Derived)

public:
    bool _toggle;
    Inner _inner;
};

REFLECTION_TYPE_BEGIN(BranchedOne, Derived)
    REFLECTION_FIELD(_toggle, ToggleOnAttribute(), ToggleOffAttribute())
    REFLECTION_FIELD(_inner)
REFLECTION_TYPE_END

class BranchedAttributeOne : public Reflection::TypeAttribute
{
    REFLECTION_ENABLE(BranchedAttributeOne, Reflection::TypeAttribute)

public:
    constexpr BranchedAttributeOne() = default;
    constexpr BranchedAttributeOne(std::string_view modifier) :
        modifier(modifier)
    {
    }

    const std::string_view modifier;
};

REFLECTION_TYPE(BranchedAttributeOne, Reflection::TypeAttribute)

class BranchedAttributeTwo : public Reflection::TypeAttribute
{
    REFLECTION_ENABLE(BranchedAttributeTwo, Reflection::TypeAttribute)

public:
    constexpr BranchedAttributeTwo() = default;
    constexpr BranchedAttributeTwo(std::string_view modifier) :
        modifier(modifier)
    {
    }

    const std::string_view modifier;
};

REFLECTION_TYPE(BranchedAttributeTwo, Reflection::TypeAttribute)

class LetterAttribute : public Reflection::FieldAttribute
{
    REFLECTION_ENABLE(LetterAttribute, Reflection::FieldAttribute)

public:
    constexpr LetterAttribute() = default;
    constexpr LetterAttribute(std::string_view modifier) :
        modifier(modifier)
    {
    }

    const std::string_view modifier;
};

REFLECTION_TYPE(LetterAttribute, Reflection::FieldAttribute)

class BranchedTwo : public Derived
{
    REFLECTION_ENABLE(BranchedTwo, Derived)

public:
    char m_letterOne;
    char m_letterTwo;
};

REFLECTION_TYPE_BEGIN(BranchedTwo, Derived)
    REFLECTION_ATTRIBUTES(BranchedAttributeOne("Small"), BranchedAttributeTwo("Big"))
    REFLECTION_FIELD(m_letterOne, LetterAttribute("Pretty"))
    REFLECTION_FIELD(m_letterTwo, LetterAttribute("Ugly"))
REFLECTION_TYPE_END

class CrossUnit
{
};
