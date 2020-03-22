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

class BaseAttribute : public Reflection::TypeAttribute
{
public:
    bool operator==(const BaseAttribute& other) const
    {
        return true;
    }
};

REFLECTION_TYPE(BaseAttribute, Reflection::TypeAttribute)

class TextAttribute : public Reflection::FieldAttribute
{
};

REFLECTION_TYPE(TextAttribute, Reflection::FieldAttribute)

class Base
{
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
public:
    constexpr DerivedAttribute(bool state) :
        state(state)
    {
    }

    const bool state;
};

REFLECTION_TYPE(DerivedAttribute, Reflection::TypeAttribute)

class CounterAttribute : public Reflection::FieldAttribute
{
};

REFLECTION_TYPE(CounterAttribute, Reflection::FieldAttribute)

class Derived : public Base
{
public:
    int counter;
};

REFLECTION_TYPE_BEGIN(Derived, Base)
    REFLECTION_ATTRIBUTES(DerivedAttribute(false))
    REFLECTION_FIELD(counter, CounterAttribute())
REFLECTION_TYPE_END

class InnerAttribute : public Reflection::FieldAttribute
{
};

REFLECTION_TYPE(InnerAttribute, Reflection::FieldAttribute)

class Inner
{
public:
    uint8_t value;
};

REFLECTION_TYPE_BEGIN(Inner)
    REFLECTION_FIELD(value, InnerAttribute())
REFLECTION_TYPE_END

class ToggleOnAttribute : public Reflection::FieldAttribute
{
};

REFLECTION_TYPE(ToggleOnAttribute, Reflection::FieldAttribute)

class ToggleOffAttribute : public Reflection::FieldAttribute
{
};

REFLECTION_TYPE(ToggleOffAttribute, Reflection::FieldAttribute)

class BranchedOne : public Derived
{
public:
    bool toggle;
    Inner inner;
};

REFLECTION_TYPE_BEGIN(BranchedOne, Derived)
    REFLECTION_FIELD(toggle, ToggleOnAttribute(), ToggleOffAttribute())
    REFLECTION_FIELD(inner)
REFLECTION_TYPE_END

class BranchedAttributeOne : public Reflection::TypeAttribute
{
public:
    constexpr BranchedAttributeOne(std::string_view modifier) :
        modifier(modifier)
    {
    }

    const std::string_view modifier;
};

REFLECTION_TYPE(BranchedAttributeOne, Reflection::TypeAttribute)

class BranchedAttributeTwo : public Reflection::TypeAttribute
{
public:
    constexpr BranchedAttributeTwo(std::string_view modifier) :
        modifier(modifier)
    {
    }

    const std::string_view modifier;
};

REFLECTION_TYPE(BranchedAttributeTwo, Reflection::TypeAttribute)

class LetterAttribute : public Reflection::FieldAttribute
{
public:
    constexpr LetterAttribute(std::string_view modifier) :
        modifier(modifier)
    {
    }

    const std::string_view modifier;
};

REFLECTION_TYPE(LetterAttribute, Reflection::FieldAttribute)

class BranchedTwo : public Derived
{
public:
    char letterOne;
    char letterTwo;
};

REFLECTION_TYPE_BEGIN(BranchedTwo, Derived)
    REFLECTION_ATTRIBUTES(BranchedAttributeOne("Small"), BranchedAttributeTwo("Big"))
    REFLECTION_FIELD(letterOne, LetterAttribute("Pretty"))
    REFLECTION_FIELD(letterTwo, LetterAttribute("Ugly"))
REFLECTION_TYPE_END

bool TestTypes()
{
    // Check reflection presence.
    TEST_FALSE(Reflection::IsReflected<Undefined>());
    TEST_TRUE(Reflection::IsReflected<Empty>());
    TEST_TRUE(Reflection::IsReflected<Base>());
    TEST_TRUE(Reflection::IsReflected<Derived>());
    TEST_TRUE(Reflection::IsReflected<Inner>());
    TEST_TRUE(Reflection::IsReflected<BranchedOne>());
    TEST_TRUE(Reflection::IsReflected<BranchedTwo>());

    // Check type name.
    TEST_EQ(Reflection::Reflect<Empty>().Name, "Empty");
    TEST_EQ(Reflection::Reflect<Base>().Name, "Base");
    TEST_EQ(Reflection::Reflect<Derived>().Name, "Derived");
    TEST_EQ(Reflection::Reflect<Inner>().Name, "Inner");
    TEST_EQ(Reflection::Reflect<BranchedOne>().Name, "BranchedOne");
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Name, "BranchedTwo");

    // Check type by value.
    TEST_FALSE(Reflection::Reflect(Undefined()).IsType<Empty>());
    TEST_TRUE(Reflection::Reflect(Undefined()).IsType<Undefined>());
    TEST_TRUE(Reflection::Reflect(Empty()).IsType<Empty>());
    TEST_TRUE(Reflection::Reflect(Base()).IsType<Base>());
    TEST_TRUE(Reflection::Reflect(Derived()).IsType<Derived>());
    TEST_TRUE(Reflection::Reflect(Inner()).IsType<Inner>());
    TEST_TRUE(Reflection::Reflect(BranchedOne()).IsType<BranchedOne>());
    TEST_TRUE(Reflection::Reflect(BranchedTwo()).IsType<BranchedTwo>());

    // Check attribute presence.
    // Add HasAttributes() method.

    // Check attribute count.
    TEST_EQ(Reflection::Reflect<Empty>().Attributes.Count, 0);
    TEST_EQ(Reflection::Reflect<Base>().Attributes.Count, 1);
    TEST_EQ(Reflection::Reflect<Derived>().Attributes.Count, 1);
    TEST_EQ(Reflection::Reflect<Inner>().Attributes.Count, 0);
    TEST_EQ(Reflection::Reflect<BranchedOne>().Attributes.Count, 0);
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Attributes.Count, 2);

    // Check attribute names.
    TEST_EQ(Reflection::Reflect<Base>().Attribute<0>().Name, "BaseAttribute");
    TEST_EQ(Reflection::Reflect<Derived>().Attribute<0>().Name, "DerivedAttribute");
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Attribute<0>().Name, "BranchedAttributeOne");
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Attribute<1>().Name, "BranchedAttributeTwo");

    // Check attribute types.
    TEST_FALSE(Reflection::Reflect<Base>().Attribute<0>().IsType<DerivedAttribute>());
    TEST_TRUE(Reflection::Reflect<Base>().Attribute<0>().IsType<BaseAttribute>());
    TEST_TRUE(Reflection::Reflect<Derived>().Attribute<0>().IsType<DerivedAttribute>());
    TEST_TRUE(Reflection::Reflect<BranchedTwo>().Attribute<0>().IsType<BranchedAttributeOne>());
    TEST_TRUE(Reflection::Reflect<BranchedTwo>().Attribute<1>().IsType<BranchedAttributeTwo>());

    // Check attribute instances.
    TEST_EQ(Reflection::Reflect<Base>().Attribute<0>().Instance, BaseAttribute());
    TEST_EQ(Reflection::Reflect<Derived>().Attribute<0>().Instance.state, false);
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Attribute<0>().Instance.modifier, "Small");
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Attribute<1>().Instance.modifier, "Big");

    // Check member count.
    TEST_EQ(Reflection::Reflect<Empty>().Members.Count, 0);
    TEST_EQ(Reflection::Reflect<Base>().Members.Count, 2);
    TEST_EQ(Reflection::Reflect<Derived>().Members.Count, 1);
    TEST_EQ(Reflection::Reflect<Inner>().Members.Count, 1);
    TEST_EQ(Reflection::Reflect<BranchedOne>().Members.Count, 2);
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Members.Count, 2);

    // Check member names.
    TEST_EQ(Reflection::Reflect<Base>().Member<0>().Name, "textWithoutAttribute");
    TEST_EQ(Reflection::Reflect<Base>().Member<1>().Name, "textPtrWithAttribute");
    TEST_EQ(Reflection::Reflect<Derived>().Member<0>().Name, "counter");
    TEST_EQ(Reflection::Reflect<Inner>().Member<0>().Name, "value");
    TEST_EQ(Reflection::Reflect<BranchedOne>().Member<0>().Name, "toggle");
    TEST_EQ(Reflection::Reflect<BranchedOne>().Member<1>().Name, "inner");
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Member<0>().Name, "letterOne");
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Member<1>().Name, "letterTwo");

    // Check member types.
    TEST_FALSE(Reflection::Reflect<Base>().Member<0>().IsType<void>());
    TEST_TRUE(Reflection::Reflect<Base>().Member<0>().IsType<std::string>());
    TEST_TRUE(Reflection::Reflect<Base>().Member<1>().IsType<const char*>());
    TEST_TRUE(Reflection::Reflect<Derived>().Member<0>().IsType<int>());
    TEST_TRUE(Reflection::Reflect<BranchedOne>().Member<0>().IsType<bool>());
    TEST_TRUE(Reflection::Reflect<BranchedOne>().Member<1>().IsType<Inner>());
    TEST_TRUE(Reflection::Reflect<BranchedTwo>().Member<0>().IsType<char>());
    TEST_TRUE(Reflection::Reflect<BranchedTwo>().Member<1>().IsType<char>());

    // Check member pointers.
    TEST_EQ(Reflection::Reflect<Base>().Member<0>().Pointer, &Base::textWithoutAttribute);
    TEST_EQ(Reflection::Reflect<Base>().Member<1>().Pointer, &Base::textPtrWithAttribute);
    TEST_EQ(Reflection::Reflect<Derived>().Member<0>().Pointer, &Derived::counter);
    TEST_EQ(Reflection::Reflect<Inner>().Member<0>().Pointer, &Inner::value);
    TEST_EQ(Reflection::Reflect<BranchedOne>().Member<0>().Pointer, &BranchedOne::toggle);
    TEST_EQ(Reflection::Reflect<BranchedOne>().Member<1>().Pointer, &BranchedOne::inner);
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Member<0>().Pointer, &BranchedTwo::letterOne);
    TEST_EQ(Reflection::Reflect<BranchedTwo>().Member<1>().Pointer, &BranchedTwo::letterTwo);

    // Check member attributes.

    // Getting members and attributes by name.

    // Enumerate attributes.

    // Enumerate members.

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
