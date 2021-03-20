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
        Reflection::IdentifierType NullTypeIdentifier =
            Reflection::GetIdentifier<Reflection::NullType>();

        CHECK(Reflection::IsRegistered(NullTypeIdentifier));
        CHECK(Reflection::DynamicType(NullTypeIdentifier).IsRegistered());
        CHECK(Reflection::DynamicType(NullTypeIdentifier).IsNullType());
        CHECK_EQ(Reflection::DynamicType(NullTypeIdentifier).GetName(), "Reflection::NullType");
        CHECK_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Undefined>()).IsNullType());
        CHECK_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Derived>()).IsNullType());
    }

    SUBCASE("Check registered types")
    {
        CHECK_FALSE(Reflection::IsRegistered(Reflection::GetIdentifier<Undefined>()));
        CHECK_FALSE(Reflection::IsRegistered(Reflection::GetIdentifier<CrossUnit>()));
        CHECK_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Undefined>()).IsRegistered());
        CHECK_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<CrossUnit>()).IsRegistered());
        CHECK(Reflection::DynamicType(Reflection::GetIdentifier<Empty>()).IsRegistered());
        CHECK(Reflection::DynamicType(Reflection::GetIdentifier<Base>()).IsRegistered());
        CHECK(Reflection::DynamicType(Reflection::GetIdentifier<Derived>()).IsRegistered());
        CHECK(Reflection::DynamicType(Reflection::GetIdentifier<Inner>()).IsRegistered());
        CHECK(Reflection::DynamicType(Reflection::GetIdentifier<BranchedOne>()).IsRegistered());
        CHECK(Reflection::DynamicType(Reflection::GetIdentifier<BranchedTwo>()).IsRegistered());
    }

    SUBCASE("Check registered type names")
    {
        CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Empty>()).GetName(), "Empty");
        CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Base>()).GetName(), "Base");
        CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Derived>()).GetName(), "Derived");
        CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Inner>()).GetName(), "Inner");
        CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<BranchedOne>()).GetName(), "BranchedOne");
        CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<BranchedTwo>()).GetName(), "BranchedTwo");
    }

    SUBCASE("Check registered type identifier")
    {
        CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Empty>()).GetIdentifier(),
            Reflection::GetIdentifier<Empty>());
        CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Base>()).GetIdentifier(),
            Reflection::GetIdentifier<Base>());
        CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Derived>()).GetIdentifier(),
            Reflection::GetIdentifier<Derived>());
        CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Inner>()).GetIdentifier(),
            Reflection::GetIdentifier<Inner>());
        CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<BranchedOne>()).GetIdentifier(),
            Reflection::GetIdentifier<BranchedOne>());
        CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<BranchedTwo>()).GetIdentifier(),
            Reflection::GetIdentifier<BranchedTwo>());
    }

    SUBCASE("Check registered type by value")
    {
        CHECK(Reflection::IsRegistered(Reflection::NullType()));
        CHECK(Reflection::DynamicType(Empty()).IsType<Empty>());
        CHECK(Reflection::DynamicType(Base()).IsType<Base>());
        CHECK(Reflection::DynamicType(Derived()).IsType<Derived>());
        CHECK(Reflection::DynamicType(Inner()).IsType<Inner>());
        CHECK(Reflection::DynamicType(BranchedOne()).IsType<BranchedOne>());
        CHECK(Reflection::DynamicType(BranchedTwo()).IsType<BranchedTwo>());
    }

    SUBCASE("Check registered type for base type")
    {
        CHECK_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Undefined>()).HasBaseType());
        CHECK_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Empty>()).HasBaseType());
        CHECK_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Base>()).HasBaseType());
        CHECK(Reflection::DynamicType(Reflection::GetIdentifier<Derived>()).HasBaseType());
        CHECK_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Inner>()).HasBaseType());
        CHECK(Reflection::DynamicType(Reflection::GetIdentifier<BranchedOne>()).HasBaseType());
        CHECK(Reflection::DynamicType(Reflection::GetIdentifier<BranchedTwo>()).HasBaseType());
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
        CHECK(Reflection::DynamicType<BranchedOne>().IsType<BranchedOne>());
        CHECK(Reflection::DynamicType<BranchedTwo>().IsType<BranchedTwo>());
        CHECK(Reflection::DynamicType<BranchedOne>().IsType<Derived>());
        CHECK(Reflection::DynamicType<BranchedTwo>().IsType<Derived>());
        CHECK(Reflection::DynamicType<BranchedOne>().IsType<Base>());
        CHECK(Reflection::DynamicType<BranchedTwo>().IsType<Base>());

        CHECK_FALSE(Reflection::DynamicType<BranchedOne>().IsType<Reflection::NullType>());
        CHECK_FALSE(Reflection::DynamicType<BranchedOne>().IsType<BranchedTwo>());
        CHECK_FALSE(Reflection::DynamicType<BranchedTwo>().IsType<BranchedOne>());
        CHECK_FALSE(Reflection::DynamicType<BranchedOne>().IsType<Inner>());
        CHECK_FALSE(Reflection::DynamicType<BranchedTwo>().IsType<Inner>());

        CHECK(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Derived>());
        CHECK(Reflection::DynamicType<BranchedTwo>().IsDerivedFrom<Derived>());
        CHECK(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Base>());
        CHECK(Reflection::DynamicType<BranchedTwo>().IsDerivedFrom<Base>());

        CHECK_FALSE(Reflection::DynamicType<Base>().IsDerivedFrom<Reflection::NullType>());
        CHECK_FALSE(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Reflection::NullType>());
        CHECK_FALSE(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<BranchedOne>());
        CHECK_FALSE(Reflection::DynamicType<Derived>().IsDerivedFrom<BranchedOne>());
        CHECK_FALSE(Reflection::DynamicType<Base>().IsDerivedFrom<BranchedOne>());
        CHECK_FALSE(Reflection::DynamicType<Inner>().IsDerivedFrom<BranchedOne>());

        CHECK(Reflection::DynamicType<Derived>().IsBaseOf<BranchedOne>());
        CHECK(Reflection::DynamicType<Derived>().IsBaseOf<BranchedTwo>());
        CHECK(Reflection::DynamicType<Base>().IsBaseOf<BranchedOne>());
        CHECK(Reflection::DynamicType<Base>().IsBaseOf<BranchedTwo>());

        CHECK_FALSE(Reflection::DynamicType<Derived>().IsBaseOf<Reflection::NullType>());
        CHECK_FALSE(Reflection::DynamicType<Derived>().IsBaseOf<Derived>());
        CHECK_FALSE(Reflection::DynamicType<Inner>().IsBaseOf<Derived>());
        CHECK_FALSE(Reflection::DynamicType<Derived>().IsBaseOf<Inner>());
    }

    SUBCASE("Check registered polymoprhic instance")
    {
        BranchedOne branchedOne;
        Derived& branchedOneDerived = branchedOne;
        Base& branchedOneBase = branchedOne;

        CHECK(branchedOne.GetTypeInfo().IsType<BranchedOne>());
        CHECK(branchedOne.GetTypeInfo().IsType<Derived>());
        CHECK(branchedOne.GetTypeInfo().IsType<Base>());
        CHECK(branchedOneDerived.GetTypeInfo().IsType<BranchedOne>());
        CHECK(branchedOneDerived.GetTypeInfo().IsType<Base>());
        CHECK(branchedOneBase.GetTypeInfo().IsType<BranchedOne>());
        CHECK(branchedOneBase.GetTypeInfo().IsType<Derived>());

        CHECK(branchedOne.GetTypeInfo().IsType(branchedOne));
        CHECK(branchedOne.GetTypeInfo().IsType(branchedOneDerived));
        CHECK(branchedOne.GetTypeInfo().IsType(branchedOneBase));
        CHECK(branchedOneDerived.GetTypeInfo().IsType(branchedOne));
        CHECK(branchedOneDerived.GetTypeInfo().IsType(branchedOneBase));
        CHECK(branchedOneBase.GetTypeInfo().IsType(branchedOne));
        CHECK(branchedOneBase.GetTypeInfo().IsType(branchedOneDerived));

        BranchedTwo branchedTwo;
        Derived& branchedTwoDerived = branchedTwo;
        Base& branchedTwoBase = branchedTwo;

        CHECK_FALSE(branchedTwo.GetTypeInfo().IsType(branchedOne));
        CHECK_FALSE(branchedTwo.GetTypeInfo().IsType(branchedOneDerived));
        CHECK_FALSE(branchedTwo.GetTypeInfo().IsType(branchedOneBase));
        CHECK_FALSE(branchedTwoDerived.GetTypeInfo().IsType(branchedOne));
        CHECK_FALSE(branchedTwoDerived.GetTypeInfo().IsType(branchedOneDerived));
        CHECK_FALSE(branchedTwoDerived.GetTypeInfo().IsType(branchedOneBase));
        CHECK_FALSE(branchedTwoBase.GetTypeInfo().IsType(branchedOne));
        CHECK_FALSE(branchedTwoBase.GetTypeInfo().IsType(branchedOneDerived));
        CHECK_FALSE(branchedTwoBase.GetTypeInfo().IsType(branchedOneBase));
    }

    // TODO: Polymorphic casts
    // Create Utility header and move all free Reflection namespaced functions there

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
