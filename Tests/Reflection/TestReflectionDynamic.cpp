/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#define DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
#include <doctest/doctest.h>

#include <string>
#include <vector>
#include <cinttypes>
#include <Reflection/Reflection.hpp>
#include "TestReflectionHeader.hpp"
#include "TestReflection/ReflectionGenerated.hpp"

DOCTEST_TEST_CASE("Dynamic Reflection")
{
    DOCTEST_SUBCASE("Check registered built-in types")
    {
        Reflection::TypeIdentifier nullTypeIdentifier =
            Reflection::GetIdentifier<Reflection::NullType>();

        DOCTEST_CHECK(Reflection::IsRegistered(nullTypeIdentifier));
        DOCTEST_CHECK(Reflection::DynamicType(nullTypeIdentifier).IsRegistered());
        DOCTEST_CHECK(Reflection::DynamicType(nullTypeIdentifier).IsNullType());
        DOCTEST_CHECK_EQ(Reflection::DynamicType(nullTypeIdentifier).GetName(),
            Common::Name("Reflection::NullType"));
        DOCTEST_CHECK_FALSE(Reflection::DynamicType(
            Reflection::GetIdentifier<Undefined>()).IsNullType());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType(
            Reflection::GetIdentifier<Derived>()).IsNullType());
    }

    DOCTEST_SUBCASE("Check registered types")
    {
        DOCTEST_CHECK_FALSE(Reflection::IsRegistered(Reflection::GetIdentifier<Undefined>()));
        DOCTEST_CHECK_FALSE(Reflection::IsRegistered(Reflection::GetIdentifier<CrossUnit>()));
        DOCTEST_CHECK_FALSE(Reflection::DynamicType(
            Reflection::GetIdentifier<Undefined>()).IsRegistered());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType(
            Reflection::GetIdentifier<CrossUnit>()).IsRegistered());
        DOCTEST_CHECK(Reflection::DynamicType<Empty>().IsRegistered());
        DOCTEST_CHECK(Reflection::DynamicType<Base>().IsRegistered());
        DOCTEST_CHECK(Reflection::DynamicType(
            Reflection::GetIdentifier<Derived>()).IsRegistered());
        DOCTEST_CHECK(Reflection::DynamicType(
            Reflection::GetIdentifier<Inner>()).IsRegistered());
        DOCTEST_CHECK(Reflection::DynamicType(
            Reflection::GetIdentifier<BranchedOne>()).IsRegistered());
        DOCTEST_CHECK(Reflection::DynamicType(
            Reflection::GetIdentifier<BranchedTwo>()).IsRegistered());
    }

    DOCTEST_SUBCASE("Check registered type names")
    {
        DOCTEST_CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Empty>())
            .GetName(), Common::Name("Empty"));
        DOCTEST_CHECK_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Base>())
            .GetName(), Common::Name("Base"));

        DOCTEST_CHECK_EQ(Reflection::GetName(Derived()), NAME_CONSTEXPR("Derived"));

        Inner inner;
        DOCTEST_CHECK_EQ(Reflection::GetName(&inner), NAME_CONSTEXPR("Inner"));

        std::unique_ptr<Derived> branchedOne = std::make_unique<BranchedOne>();
        DOCTEST_CHECK_EQ(Reflection::GetName(branchedOne), NAME_CONSTEXPR("BranchedOne"));

        Reflection::TypeIdentifier branchedTwo = Reflection::GetIdentifier<BranchedTwo>();
        DOCTEST_CHECK_EQ(Reflection::GetName(branchedTwo), NAME_CONSTEXPR("BranchedTwo"));
    }

    DOCTEST_SUBCASE("Check registered type identifier")
    {
        DOCTEST_CHECK_EQ(Reflection::DynamicType(
            Reflection::GetIdentifier<Empty>()).GetIdentifier(),
            Reflection::GetIdentifier<Empty>());
        DOCTEST_CHECK_EQ(Reflection::DynamicType(
            Reflection::GetIdentifier<Base>()).GetIdentifier(),
            Reflection::GetIdentifier<Base>());
        DOCTEST_CHECK_EQ(Reflection::DynamicType(
            Reflection::GetIdentifier<Derived>()).GetIdentifier(),
            Reflection::GetIdentifier<Derived>());

        Inner inner;
        DOCTEST_CHECK_EQ(Reflection::DynamicType(inner).GetIdentifier(),
            Reflection::GetIdentifier<Inner>());

        BranchedOne branchedOne;
        DOCTEST_CHECK_EQ(Reflection::DynamicType(&branchedOne).GetIdentifier(),
            Reflection::GetIdentifier<BranchedOne>());

        std::unique_ptr<BranchedTwo> branchedTwo = std::make_unique<BranchedTwo>();
        DOCTEST_CHECK_EQ(Reflection::DynamicType(branchedTwo).GetIdentifier(),
            Reflection::GetIdentifier<BranchedTwo>());
    }

    DOCTEST_SUBCASE("Check registered type by value")
    {
        DOCTEST_CHECK(Reflection::IsRegistered(Reflection::NullType()));
        DOCTEST_CHECK(Reflection::DynamicType(Empty()).IsType<Empty>());
        DOCTEST_CHECK(Reflection::DynamicType(Base()).IsType<Base>());
        DOCTEST_CHECK(Reflection::DynamicType(Derived()).IsType<Derived>());
        DOCTEST_CHECK(Reflection::DynamicType(Inner()).IsType<Inner>());
        DOCTEST_CHECK(Reflection::DynamicType(BranchedOne()).IsType<BranchedOne>());
        DOCTEST_CHECK(Reflection::DynamicType(BranchedTwo()).IsType<BranchedTwo>());
    }

    DOCTEST_SUBCASE("Check registered type for base type")
    {
        DOCTEST_CHECK_FALSE(Reflection::DynamicType(
            Reflection::GetIdentifier<Undefined>()).HasBaseType());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType(
            Reflection::GetIdentifier<Empty>()).HasBaseType());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType(
            Reflection::GetIdentifier<Base>()).HasBaseType());
        DOCTEST_CHECK(Reflection::DynamicType(
            Reflection::GetIdentifier<Derived>()).HasBaseType());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType(
            Reflection::GetIdentifier<Inner>()).HasBaseType());
        DOCTEST_CHECK(Reflection::DynamicType(
            Reflection::GetIdentifier<BranchedOne>()).HasBaseType());
        DOCTEST_CHECK(Reflection::DynamicType(
            Reflection::GetIdentifier<BranchedTwo>()).HasBaseType());
    }

    DOCTEST_SUBCASE("Check registered base type")
    {
        DOCTEST_CHECK_EQ(Reflection::DynamicType<Derived>().GetBaseType().GetName(),
            Common::Name("Base"));
        DOCTEST_CHECK_EQ(Reflection::DynamicType<BranchedOne>().GetBaseType().GetName(),
            Common::Name("Derived"));
        DOCTEST_CHECK_EQ(Reflection::DynamicType<BranchedTwo>().GetBaseType().GetName(),
            Common::Name("Derived"));
        DOCTEST_CHECK(Reflection::DynamicType<Derived>().GetBaseType().IsType<Base>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedOne>().GetBaseType().IsType<Derived>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedTwo>().GetBaseType().IsType<Derived>());

        DOCTEST_CHECK(Reflection::DynamicType<Derived>().IsDerivedFrom<Base>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Derived>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedTwo>().IsDerivedFrom<Derived>());

        DOCTEST_CHECK(Reflection::DynamicType<Base>().IsBaseOf<Derived>());
        DOCTEST_CHECK(Reflection::DynamicType<Derived>().IsBaseOf<BranchedOne>());
        DOCTEST_CHECK(Reflection::DynamicType<Derived>().IsBaseOf<BranchedOne>());
    }

    DOCTEST_SUBCASE("Check registered super declaration")
    {
        DOCTEST_CHECK_EQ(Reflection::DynamicType<Derived>().GetBaseType().GetIdentifier(),
            Reflection::DynamicType<Derived::Super>().GetIdentifier());

        DOCTEST_CHECK(Reflection::DynamicType<BranchedOne>()
            .GetBaseType().IsType<BranchedOne::Super>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedTwo>()
            .GetBaseType().IsType<BranchedTwo::Super>());
    }

    DOCTEST_SUBCASE("Check registered polymorphic type")
    {
        DOCTEST_CHECK(Reflection::DynamicType<BranchedOne>().IsType<BranchedOne>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedTwo>().IsType<BranchedTwo>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedOne>().IsType<Derived>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedTwo>().IsType<Derived>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedOne>().IsType<Base>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedTwo>().IsType<Base>());

        DOCTEST_CHECK_FALSE(Reflection::DynamicType<BranchedOne>().IsType<Reflection::NullType>());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType<BranchedOne>().IsType<BranchedTwo>());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType<BranchedTwo>().IsType<BranchedOne>());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType<BranchedOne>().IsType<Inner>());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType<BranchedTwo>().IsType<Inner>());

        DOCTEST_CHECK(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Derived>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedTwo>().IsDerivedFrom<Derived>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Base>());
        DOCTEST_CHECK(Reflection::DynamicType<BranchedTwo>().IsDerivedFrom<Base>());

        DOCTEST_CHECK_FALSE(Reflection::DynamicType<Base>()
            .IsDerivedFrom<Reflection::NullType>());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType<BranchedOne>()
            .IsDerivedFrom<Reflection::NullType>());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType<BranchedOne>()
            .IsDerivedFrom<BranchedOne>());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType<Derived>()
            .IsDerivedFrom<BranchedOne>());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType<Base>()
            .IsDerivedFrom<BranchedOne>());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType<Inner>()
            .IsDerivedFrom<BranchedOne>());

        DOCTEST_CHECK(Reflection::DynamicType<Derived>().IsBaseOf<BranchedOne>());
        DOCTEST_CHECK(Reflection::DynamicType<Derived>().IsBaseOf<BranchedTwo>());
        DOCTEST_CHECK(Reflection::DynamicType<Base>().IsBaseOf<BranchedOne>());
        DOCTEST_CHECK(Reflection::DynamicType<Base>().IsBaseOf<BranchedTwo>());

        DOCTEST_CHECK_FALSE(Reflection::DynamicType<Derived>().IsBaseOf<Reflection::NullType>());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType<Derived>().IsBaseOf<Derived>());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType<Inner>().IsBaseOf<Derived>());
        DOCTEST_CHECK_FALSE(Reflection::DynamicType<Derived>().IsBaseOf<Inner>());
    }

    DOCTEST_SUBCASE("Check registered polymorphic instance")
    {
        BranchedOne branchedOne;
        Derived& branchedOneDerived = branchedOne;
        Base& branchedOneBase = branchedOne;

        DOCTEST_CHECK(branchedOne.GetTypeInfo().IsType<BranchedOne>());
        DOCTEST_CHECK(branchedOne.GetTypeInfo().IsType<Derived>());
        DOCTEST_CHECK(branchedOne.GetTypeInfo().IsType<Base>());
        DOCTEST_CHECK(branchedOneDerived.GetTypeInfo().IsType<BranchedOne>());
        DOCTEST_CHECK(branchedOneDerived.GetTypeInfo().IsType<Base>());
        DOCTEST_CHECK(branchedOneBase.GetTypeInfo().IsType<BranchedOne>());
        DOCTEST_CHECK(branchedOneBase.GetTypeInfo().IsType<Derived>());

        DOCTEST_CHECK(branchedOne.GetTypeInfo().IsType(branchedOne));
        DOCTEST_CHECK(branchedOne.GetTypeInfo().IsType(branchedOneDerived));
        DOCTEST_CHECK(branchedOne.GetTypeInfo().IsType(branchedOneBase));
        DOCTEST_CHECK(branchedOneDerived.GetTypeInfo().IsType(branchedOne));
        DOCTEST_CHECK(branchedOneDerived.GetTypeInfo().IsType(branchedOneBase));
        DOCTEST_CHECK(branchedOneBase.GetTypeInfo().IsType(branchedOne));
        DOCTEST_CHECK(branchedOneBase.GetTypeInfo().IsType(branchedOneDerived));

        BranchedTwo branchedTwo;
        Derived& branchedTwoDerived = branchedTwo;
        Base& branchedTwoBase = branchedTwo;

        DOCTEST_CHECK_FALSE(branchedTwo.GetTypeInfo().IsType(branchedOne));
        DOCTEST_CHECK_FALSE(branchedTwo.GetTypeInfo().IsType(branchedOneDerived));
        DOCTEST_CHECK_FALSE(branchedTwo.GetTypeInfo().IsType(branchedOneBase));
        DOCTEST_CHECK_FALSE(branchedTwoDerived.GetTypeInfo().IsType(branchedOne));
        DOCTEST_CHECK_FALSE(branchedTwoDerived.GetTypeInfo().IsType(branchedOneDerived));
        DOCTEST_CHECK_FALSE(branchedTwoDerived.GetTypeInfo().IsType(branchedOneBase));
        DOCTEST_CHECK_FALSE(branchedTwoBase.GetTypeInfo().IsType(branchedOne));
        DOCTEST_CHECK_FALSE(branchedTwoBase.GetTypeInfo().IsType(branchedOneDerived));
        DOCTEST_CHECK_FALSE(branchedTwoBase.GetTypeInfo().IsType(branchedOneBase));
    }

    DOCTEST_SUBCASE("Check registered type casting")
    {
        BranchedOne branchedOne;
        branchedOne.inner.value = 42;

        BranchedOne* branchedOnePtr = Reflection::Cast<BranchedOne>(&branchedOne);
        DOCTEST_REQUIRE_NE(branchedOnePtr, nullptr);
        DOCTEST_CHECK_EQ(branchedOnePtr->inner.value, 42);
        DOCTEST_CHECK(branchedOnePtr->GetTypeInfo().IsType<BranchedOne>());
        DOCTEST_CHECK(branchedOnePtr->GetTypeInfo().IsType<Derived>());
        DOCTEST_CHECK(branchedOnePtr->GetTypeInfo().IsType<Base>());

        Derived* derivedPtr = Reflection::Cast<Derived>(branchedOnePtr);
        DOCTEST_REQUIRE_NE(derivedPtr, nullptr);
        DOCTEST_CHECK(derivedPtr->GetTypeInfo().IsType<BranchedOne>());
        DOCTEST_CHECK(derivedPtr->GetTypeInfo().IsType<Derived>());
        DOCTEST_CHECK(derivedPtr->GetTypeInfo().IsType<Base>());

        Base* basePtr = Reflection::Cast<Base>(branchedOnePtr);
        DOCTEST_REQUIRE_NE(basePtr, nullptr);
        DOCTEST_CHECK(basePtr->GetTypeInfo().IsType<BranchedOne>());
        DOCTEST_CHECK(basePtr->GetTypeInfo().IsType<Derived>());
        DOCTEST_CHECK(basePtr->GetTypeInfo().IsType<Base>());

        derivedPtr = Reflection::Cast<Derived>(basePtr);
        DOCTEST_REQUIRE_NE(derivedPtr, nullptr);
        DOCTEST_CHECK(derivedPtr->GetTypeInfo().IsType<BranchedOne>());
        DOCTEST_CHECK(derivedPtr->GetTypeInfo().IsType<Derived>());
        DOCTEST_CHECK(derivedPtr->GetTypeInfo().IsType<Base>());

        branchedOnePtr = Reflection::Cast<BranchedOne>(derivedPtr);
        DOCTEST_REQUIRE_NE(branchedOnePtr, nullptr);
        DOCTEST_CHECK_EQ(branchedOnePtr->inner.value, 42);
        DOCTEST_CHECK(branchedOnePtr->GetTypeInfo().IsType<BranchedOne>());
        DOCTEST_CHECK(branchedOnePtr->GetTypeInfo().IsType<Derived>());
        DOCTEST_CHECK(branchedOnePtr->GetTypeInfo().IsType<Base>());

        branchedOnePtr = Reflection::Cast<BranchedOne>(basePtr);
        DOCTEST_REQUIRE_NE(branchedOnePtr, nullptr);
        DOCTEST_CHECK_EQ(branchedOnePtr->inner.value, 42);
        DOCTEST_CHECK(branchedOnePtr->GetTypeInfo().IsType<BranchedOne>());
        DOCTEST_CHECK(branchedOnePtr->GetTypeInfo().IsType<Derived>());
        DOCTEST_CHECK(branchedOnePtr->GetTypeInfo().IsType<Base>());

        BranchedTwo* branchedTwoPtr = Reflection::Cast<BranchedTwo>(branchedOnePtr);
        DOCTEST_CHECK_EQ(branchedTwoPtr, nullptr);

        branchedTwoPtr = Reflection::Cast<BranchedTwo>(derivedPtr);
        DOCTEST_CHECK_EQ(branchedTwoPtr, nullptr);

        branchedTwoPtr = Reflection::Cast<BranchedTwo>(basePtr);
        DOCTEST_CHECK_EQ(branchedTwoPtr, nullptr);
    }

    DOCTEST_SUBCASE("Construct types from identifier")
    {
        DOCTEST_CHECK(Reflection::StaticType<Derived>().IsConstructible());
        std::unique_ptr<Base> derived(Reflection::Construct<Derived>());
        DOCTEST_CHECK(derived->GetTypeInfo().IsType<Derived>());

        DOCTEST_CHECK(Reflection::DynamicType<BranchedOne>().IsConstructible());
        std::unique_ptr<Base> branchedOne(Reflection::Construct<Base>(
            Reflection::GetIdentifier<BranchedOne>()));
        DOCTEST_CHECK(branchedOne->GetTypeInfo().IsType<BranchedOne>());

        DOCTEST_CHECK(Reflection::DynamicType(
            Reflection::GetIdentifier<BranchedTwo>()).IsConstructible());
        std::unique_ptr<Base> branchedTwo(Reflection::Construct<Base>(
            Reflection::GetIdentifier<BranchedTwo>()));
        DOCTEST_CHECK(branchedTwo->GetTypeInfo().IsType<BranchedTwo>());

        DOCTEST_CHECK_FALSE(Reflection::StaticType<
            Reflection::TypeAttribute>().IsConstructible());
        auto* typeAttribute = Reflection::Construct<Reflection::TypeAttribute>();
        DOCTEST_CHECK_EQ(typeAttribute, nullptr);

        DOCTEST_CHECK_FALSE(Reflection::StaticType<
            Reflection::MethodAttribute>().IsConstructible());
        auto* methodAttribute = Reflection::Construct<Reflection::MethodAttribute>();
        DOCTEST_CHECK_EQ(methodAttribute, nullptr);

        DOCTEST_CHECK_FALSE(Reflection::StaticType<
            Reflection::FieldAttribute>().IsConstructible());
        auto* fieldAttribute = Reflection::Construct<Reflection::FieldAttribute>();
        DOCTEST_CHECK_EQ(fieldAttribute, nullptr);

        DOCTEST_SUBCASE("Cast constructed types to base type")
        {
            std::unique_ptr<Base> base;

            base = Reflection::Cast<Derived>(derived);
            DOCTEST_CHECK(base->GetTypeInfo().IsType<Derived>());

            base = Reflection::Cast<Base>(branchedOne);
            DOCTEST_CHECK(base->GetTypeInfo().IsType<BranchedOne>());

            base = Reflection::Cast<Base>(branchedTwo);
            DOCTEST_CHECK(base->GetTypeInfo().IsType<BranchedTwo>());
        }
    }
}
