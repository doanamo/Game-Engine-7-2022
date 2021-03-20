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

    CHECK(REFLECTION_REGISTER(Empty));
    CHECK(REFLECTION_REGISTER(BaseAttribute));
    CHECK(REFLECTION_REGISTER(TextAttribute));
    CHECK(REFLECTION_REGISTER(Base));
    CHECK(REFLECTION_REGISTER(DerivedAttribute));
    CHECK(REFLECTION_REGISTER(CounterAttribute));
    CHECK(REFLECTION_REGISTER(Derived));
    CHECK(REFLECTION_REGISTER(InnerAttribute));
    CHECK(REFLECTION_REGISTER(Inner));
    CHECK(REFLECTION_REGISTER(ToggleOnAttribute));
    CHECK(REFLECTION_REGISTER(ToggleOffAttribute));
    CHECK(REFLECTION_REGISTER(BranchedOne));
    CHECK(REFLECTION_REGISTER(BranchedAttributeOne));
    CHECK(REFLECTION_REGISTER(BranchedAttributeTwo));
    CHECK(REFLECTION_REGISTER(LetterAttribute));
    CHECK(REFLECTION_REGISTER(BranchedTwo));

    registered = true;
}

TEST_CASE("Dynamic Reflection")
{
    RegisterTypesOnce();

    SUBCASE("Check registered built-in types")
    {
        CHECK(Reflection::IsRegistered(REFLECTION_IDENTIFIER(Reflection::NullType)));
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Reflection::NullType)).IsRegistered());
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Reflection::NullType)).IsNullType());
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Reflection::NullType)).GetName(),
            "Reflection::NullType");
        CHECK_FALSE(Reflection::DynamicType(REFLECTION_IDENTIFIER(Undefined)).IsNullType());
        CHECK_FALSE(Reflection::DynamicType(REFLECTION_IDENTIFIER(Derived)).IsNullType());
    }

    SUBCASE("Check registered types")
    {
        CHECK_FALSE(Reflection::IsRegistered(REFLECTION_IDENTIFIER(Undefined)));
        CHECK_FALSE(Reflection::IsRegistered(REFLECTION_IDENTIFIER(CrossUnit)));
        CHECK_FALSE(Reflection::DynamicType(REFLECTION_IDENTIFIER(Undefined)).IsRegistered());
        CHECK_FALSE(Reflection::DynamicType(REFLECTION_IDENTIFIER(CrossUnit)).IsRegistered());
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Empty)).IsRegistered());
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Base)).IsRegistered());
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Derived)).IsRegistered());
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Inner)).IsRegistered());
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedOne)).IsRegistered());
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedTwo)).IsRegistered());
    }

    SUBCASE("Check registered type names")
    {
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Empty)).GetName(), "Empty");
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Base)).GetName(), "Base");
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Derived)).GetName(), "Derived");
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Inner)).GetName(), "Inner");
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedOne)).GetName(), "BranchedOne");
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedTwo)).GetName(), "BranchedTwo");
    }

    SUBCASE("Check registered type identifier")
    {
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Empty)).GetIdentifier(),
            REFLECTION_IDENTIFIER(Empty));
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Base)).GetIdentifier(),
            REFLECTION_IDENTIFIER(Base));
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Derived)).GetIdentifier(),
            REFLECTION_IDENTIFIER(Derived));
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(Inner)).GetIdentifier(),
            REFLECTION_IDENTIFIER(Inner));
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedOne)).GetIdentifier(),
            REFLECTION_IDENTIFIER(BranchedOne));
        CHECK_EQ(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedTwo)).GetIdentifier(),
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

    SUBCASE("Check registered type for base type")
    {
        CHECK_FALSE(Reflection::DynamicType(REFLECTION_IDENTIFIER(Undefined)).HasBaseType());
        CHECK_FALSE(Reflection::DynamicType(REFLECTION_IDENTIFIER(Empty)).HasBaseType());
        CHECK_FALSE(Reflection::DynamicType(REFLECTION_IDENTIFIER(Base)).HasBaseType());
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(Derived)).HasBaseType());
        CHECK_FALSE(Reflection::DynamicType(REFLECTION_IDENTIFIER(Inner)).HasBaseType());
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedOne)).HasBaseType());
        CHECK(Reflection::DynamicType(REFLECTION_IDENTIFIER(BranchedTwo)).HasBaseType());
    }

    SUBCASE("Check registered base type")
    {
        CHECK_EQ(Reflection::DynamicType<Derived>().GetBaseType().GetName(), "Base");
        CHECK_EQ(Reflection::DynamicType<BranchedOne>().GetBaseType().GetName(), "Derived");
        CHECK_EQ(Reflection::DynamicType<BranchedTwo>().GetBaseType().GetName(), "Derived");
        CHECK(Reflection::DynamicType<Derived>().GetBaseType().IsType<Base>());
        CHECK(Reflection::DynamicType<BranchedOne>().GetBaseType().IsType<Derived>());
        CHECK(Reflection::DynamicType<BranchedTwo>().GetBaseType().IsType<Derived>());

        CHECK(Reflection::DynamicType<Derived>().IsDerivedFrom<Base>());
        CHECK(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Derived>());
        CHECK(Reflection::DynamicType<BranchedTwo>().IsDerivedFrom<Derived>());

        CHECK(Reflection::DynamicType<Base>().IsBaseOf<Derived>());
        CHECK(Reflection::DynamicType<Derived>().IsBaseOf<BranchedOne>());
        CHECK(Reflection::DynamicType<Derived>().IsBaseOf<BranchedOne>());
    }

    SUBCASE("Check registered super declaration")
    {
        CHECK_EQ(Reflection::DynamicType<Derived>().GetBaseType().GetIdentifier(),
            Reflection::DynamicType<Derived::Super>().GetIdentifier());

        CHECK(Reflection::DynamicType<BranchedOne>().GetBaseType().IsType<BranchedOne::Super>());
        CHECK(Reflection::DynamicType<BranchedTwo>().GetBaseType().IsType<BranchedTwo::Super>());
    }

    SUBCASE("Check registered polymorphic type")
    {
        CHECK(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Derived>());
        CHECK(Reflection::DynamicType<BranchedTwo>().IsDerivedFrom<Derived>());
        CHECK(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Base>());
        CHECK(Reflection::DynamicType<BranchedTwo>().IsDerivedFrom<Base>());

        CHECK_FALSE(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<BranchedOne>());
        CHECK_FALSE(Reflection::DynamicType<Derived>().IsDerivedFrom<BranchedOne>());
        CHECK_FALSE(Reflection::DynamicType<Base>().IsDerivedFrom<BranchedOne>());
        CHECK_FALSE(Reflection::DynamicType<Inner>().IsDerivedFrom<BranchedOne>());

        CHECK(Reflection::DynamicType<Derived>().IsBaseOf<BranchedOne>());
        CHECK(Reflection::DynamicType<Derived>().IsBaseOf<BranchedTwo>());
        CHECK(Reflection::DynamicType<Base>().IsBaseOf<BranchedOne>());
        CHECK(Reflection::DynamicType<Base>().IsBaseOf<BranchedTwo>());

        CHECK_FALSE(Reflection::DynamicType<Derived>().IsBaseOf<Derived>());
        CHECK_FALSE(Reflection::DynamicType<Inner>().IsBaseOf<Derived>());
        CHECK_FALSE(Reflection::DynamicType<Derived>().IsBaseOf<Inner>());
    }

    // TODO: Polymorphic casts

    // TODO: Multi level inheritance tests for IsBaseOf/IsType/IsDerivedFrom
    // This applies only to dynamic, static cannot do it!

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
