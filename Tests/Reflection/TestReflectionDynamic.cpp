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

    SUBCASE("Check registered types")
    {
        CHECK(Reflection::Reflect(REFLECTION_IDENTIFIER(Reflection::NullType)).Reflected);
        CHECK_FALSE(Reflection::Reflect(REFLECTION_IDENTIFIER(Undefined)).Reflected);
        CHECK_FALSE(Reflection::Reflect(REFLECTION_IDENTIFIER(CrossUnit)).Reflected);
        CHECK(Reflection::Reflect(REFLECTION_IDENTIFIER(Empty)).Reflected);
        CHECK(Reflection::Reflect(REFLECTION_IDENTIFIER(Base)).Reflected);
        CHECK(Reflection::Reflect(REFLECTION_IDENTIFIER(Derived)).Reflected);
        CHECK(Reflection::Reflect(REFLECTION_IDENTIFIER(Inner)).Reflected);
        CHECK(Reflection::Reflect(REFLECTION_IDENTIFIER(BranchedOne)).Reflected);
        CHECK(Reflection::Reflect(REFLECTION_IDENTIFIER(BranchedTwo)).Reflected);
    }

    SUBCASE("Check registered type names")
    {
        CHECK_EQ(Reflection::Reflect(REFLECTION_IDENTIFIER(Empty)).Name, "Empty");
        CHECK_EQ(Reflection::Reflect(REFLECTION_IDENTIFIER(Base)).Name, "Base");
        CHECK_EQ(Reflection::Reflect(REFLECTION_IDENTIFIER(Derived)).Name, "Derived");
        CHECK_EQ(Reflection::Reflect(REFLECTION_IDENTIFIER(Inner)).Name, "Inner");
        CHECK_EQ(Reflection::Reflect(REFLECTION_IDENTIFIER(BranchedOne)).Name, "BranchedOne");
        CHECK_EQ(Reflection::Reflect(REFLECTION_IDENTIFIER(BranchedTwo)).Name, "BranchedTwo");
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
