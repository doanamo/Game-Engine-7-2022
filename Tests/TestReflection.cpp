/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "TestHelpers.hpp"
#include "Reflection/Reflection.hpp"
#include <cinttypes>

class Undefined
{
};

class Empty
{

};

REFLECTION_TYPE_BEGIN(Empty)
REFLECTION_TYPE_END

class Base
{
public:
    std::string textWithoutAttribute;
    std::string textWithAttribute;
};

class BaseAttribute : public Reflection::TypeAttribute
{
};

class TextAttribute : public Reflection::FieldAttribute
{
};

REFLECTION_TYPE_BEGIN(Base)
    REFLECTION_ATTRIBUTES(BaseAttribute())
    REFLECTION_FIELD(textWithoutAttribute)
    REFLECTION_FIELD(textWithAttribute, TextAttribute())
REFLECTION_TYPE_END

class Derived : public Base
{
public:
    int value;
};

class DerivedAttribute : public Reflection::TypeAttribute
{
};

class ValueAttribute : public Reflection::FieldAttribute
{
};

REFLECTION_TYPE_BEGIN(Derived, Base)
    REFLECTION_ATTRIBUTES(DerivedAttribute())
    REFLECTION_FIELD(value, ValueAttribute())
REFLECTION_TYPE_END

class Inner
{
public:
    uint8_t value;
};

class InnerAttribute : public Reflection::FieldAttribute
{
};

REFLECTION_TYPE_BEGIN(Inner)
    REFLECTION_FIELD(value, InnerAttribute())
REFLECTION_TYPE_END

class BranchedOne : public Derived
{
public:
    bool toggle;
    Inner inner;
};

class ToggleOnAttribute : public Reflection::FieldAttribute
{
};

class ToggleOffAttribute : public Reflection::FieldAttribute
{
};

REFLECTION_TYPE_BEGIN(BranchedOne, Derived)
    REFLECTION_FIELD(toggle, ToggleOnAttribute(), ToggleOffAttribute())
    REFLECTION_FIELD(inner)
REFLECTION_TYPE_END

class BranchedTwo : public Derived
{
public:
    char letterOne;
    char letterTwo;
};

class BranchedAttributeOne : public Reflection::TypeAttribute
{
public:
    constexpr BranchedAttributeOne(std::string_view modifier) :
        modifier(modifier)
    {
    }

    const std::string_view modifier;
};

class BranchedAttributeTwo : public Reflection::TypeAttribute
{
public:
    constexpr BranchedAttributeTwo(std::string_view modifier) :
        modifier(modifier)
    {
    }

    const std::string_view modifier;
};

class LetterAttribute : public Reflection::FieldAttribute
{
public:
    constexpr LetterAttribute(std::string_view modifier) :
        modifier(modifier)
    {
    }

    const std::string_view modifier;
};

REFLECTION_TYPE_BEGIN(BranchedTwo, Derived)
    REFLECTION_ATTRIBUTES(BranchedAttributeOne("Small"), BranchedAttributeTwo("Big"))
    REFLECTION_FIELD(letterOne, LetterAttribute("Pretty"))
    REFLECTION_FIELD(letterTwo, LetterAttribute("Ugly"))
REFLECTION_TYPE_END

bool TestTypes()
{
    TEST_FALSE(Reflection::IsReflected<Undefined>());
    TEST_TRUE(Reflection::IsReflected<Empty>());
    TEST_TRUE(Reflection::IsReflected<Base>());
    TEST_TRUE(Reflection::IsReflected<Derived>());
    TEST_TRUE(Reflection::IsReflected<Inner>());
    TEST_TRUE(Reflection::IsReflected<BranchedOne>());
    TEST_TRUE(Reflection::IsReflected<BranchedTwo>());

    TEST_EQ(Reflection::Reflect<Empty>().Name, "Empty");
    TEST_EQ(Reflection::Reflect<Base>().Name, "Base");
    TEST_EQ(Reflection::Reflect<Derived>().Name, "Derived");
    TEST_EQ(Reflection::Reflect<Inner>().Name, "Inner");
    TEST_EQ(Reflection::Reflect<BranchedOne>().Name, "BranchedOne");
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Name, "BranchedTwo");

    TEST_EQ(Reflection::Reflect<Empty>().Attributes.Count, 0);
    TEST_EQ(Reflection::Reflect<Base>().Attributes.Count, 1);
    TEST_EQ(Reflection::Reflect<Derived>().Attributes.Count, 1);
    TEST_EQ(Reflection::Reflect<Inner>().Attributes.Count, 0);
    TEST_EQ(Reflection::Reflect<BranchedOne>().Attributes.Count, 0);
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Attributes.Count, 2);

    TEST_EQ(Reflection::Reflect<Empty>().Members.Count, 0);
    TEST_EQ(Reflection::Reflect<Base>().Members.Count, 2);
    TEST_EQ(Reflection::Reflect<Derived>().Members.Count, 1);
    TEST_EQ(Reflection::Reflect<Inner>().Members.Count, 1);
    TEST_EQ(Reflection::Reflect<BranchedOne>().Members.Count, 2);
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Members.Count, 2);

    TEST_EQ(Reflection::Reflect<Base>().Member<0>().Name, "textWithoutAttribute");
    TEST_EQ(Reflection::Reflect<Base>().Member<1>().Name, "textWithAttribute");
    TEST_EQ(Reflection::Reflect<Derived>().Member<0>().Name, "value");
    TEST_EQ(Reflection::Reflect<Inner>().Member<0>().Name, "value");
    TEST_EQ(Reflection::Reflect<BranchedOne>().Member<0>().Name, "toggle");
    TEST_EQ(Reflection::Reflect<BranchedOne>().Member<1>().Name, "inner");
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Member<0>().Name, "letterOne");
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Member<1>().Name, "letterTwo");

    // check attribute/member types
    // check attribute values
    // TODO: enumerate members/attributes, for each

    return true;
}

bool TestCreate()
{
    // TODO: Instantiate derived using its type identifier.
    // Base* instance = Base::create(Derived::Type);
    // TEST_EQ(GetType(instance) == Derived::Type);

    return true;
}

bool TestCast()
{
    // TODO: Test casting from one type to another.

    return true;
}

bool TestSuper()
{
    // TODO: Test super typedef.

    return true;
}

int main()
{
    if(!TestTypes())
        return 1;

    if(!TestCreate())
        return 1;

    if(!TestCast())
        return 1;

    if(!TestSuper())
        return 1;

    return 0;
}
