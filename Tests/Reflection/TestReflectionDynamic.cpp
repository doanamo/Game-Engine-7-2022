/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <string>
#include <vector>
#include <cinttypes>
#include <doctest/doctest.h>
#include <Reflection/Reflection.hpp>
#include "TestReflectionHeader.hpp"

void RegisterTypesOnce()
{
    static bool registered = false;
    if(registered)
        return;

    REFLECTION_REGISTER(Empty);
    REFLECTION_REGISTER(BaseAttribute);
    REFLECTION_REGISTER(TextAttribute);
    REFLECTION_REGISTER(Base);
    REFLECTION_REGISTER(DerivedAttribute);
    REFLECTION_REGISTER(CounterAttribute);
    REFLECTION_REGISTER(Derived);
    REFLECTION_REGISTER(InnerAttribute);
    REFLECTION_REGISTER(Inner);
    REFLECTION_REGISTER(ToggleOnAttribute);
    REFLECTION_REGISTER(ToggleOffAttribute);
    REFLECTION_REGISTER(BranchedOne);
    REFLECTION_REGISTER(BranchedAttributeOne);
    REFLECTION_REGISTER(BranchedAttributeTwo);
    REFLECTION_REGISTER(LetterAttribute);
    REFLECTION_REGISTER(BranchedTwo);

    registered = true;
}

TEST_CASE("Dynamic Reflection")
{
    RegisterTypesOnce();

    SUBCASE("Check registered built-in types")
    {
        CHECK(Reflection::IsRegistered(REFLECTION_IDENTIFIER(Reflection::NullType)));
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Reflection::NullType)).Reflected);
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Reflection::NullType)).IsNullType());
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Reflection::NullType)).Name,
            "Reflection::NullType");
        CHECK_FALSE(Reflection::DynamicType(REFLECTION_IDENTIFIER(Undefined)).IsNullType());
        CHECK_FALSE(Reflection::DynamicType(REFLECTION_IDENTIFIER(Derived)).IsNullType());
    }

    SUBCASE("Check registered types")
    {
        CHECK_FALSE(Reflection::IsRegistered(REFLECTION_IDENTIFIER(Undefined)));
        CHECK_FALSE(Reflection::IsRegistered(REFLECTION_IDENTIFIER(CrossUnit)));
        CHECK_FALSE(Reflection::DynamicType(REFLECTION_IDENTIFIER(Undefined)).Reflected);
        CHECK_FALSE(Reflection::DynamicType(REFLECTION_IDENTIFIER(CrossUnit)).Reflected);
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Empty)).Reflected);
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Base)).Reflected);
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Derived)).Reflected);
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Inner)).Reflected);
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedOne)).Reflected);
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedTwo)).Reflected);
    }

    SUBCASE("Check registered type names")
    {
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Empty)).Name, "Empty");
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Base)).Name, "Base");
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Derived)).Name, "Derived");
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Inner)).Name, "Inner");
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedOne)).Name, "BranchedOne");
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedTwo)).Name, "BranchedTwo");
    }

    SUBCASE("Check register type identifier")
    {
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Empty)).Identifier,
            REFLECTION_IDENTIFIER(Empty));
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Base)).Identifier,
            REFLECTION_IDENTIFIER(Base));
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Derived)).Identifier,
            REFLECTION_IDENTIFIER(Derived));
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Inner)).Identifier,
            REFLECTION_IDENTIFIER(Inner));
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedOne)).Identifier,
            REFLECTION_IDENTIFIER(BranchedOne));
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedTwo)).Identifier,
            REFLECTION_IDENTIFIER(BranchedTwo));
    }

    SUBCASE("Check registered type by value")
    {
        CHECK(Reflection::IsRegistered(Reflection::NullType()));
        CHECK_FALSE(Reflection::IsRegistered(Undefined()));
        CHECK_FALSE(Reflection::IsRegistered(CrossUnit()));
        CHECK_FALSE(Reflection::DynamicType(Undefined()).IsType<Empty>());
        CHECK_FALSE(Reflection::DynamicType(CrossUnit()).IsType<CrossUnit>());
        CHECK(Reflection::DynamicType(Empty()).IsType<Empty>());
        CHECK(Reflection::DynamicType(Base()).IsType<Base>());
        CHECK(Reflection::DynamicType(Derived()).IsType<Derived>());
        CHECK(Reflection::DynamicType(Inner()).IsType<Inner>());
        CHECK(Reflection::DynamicType(BranchedOne()).IsType<BranchedOne>());
        CHECK(Reflection::DynamicType(BranchedTwo()).IsType<BranchedTwo>());
    }

    SUBCASE("Instantiate")
    {
        // TODO: Instantiate derived using its type identifier.
        // Base* instance = Base::create(Derived::Type);
        // CHECK_EQ(GetType(instance) == Derived::Type);
    }

    SUBCASE("Cast")
    {
        // TODO: Test casting from one type to another.
    }

    SUBCASE("Super")
    {
        // TODO: Test super typedef for classes.
    }
}
