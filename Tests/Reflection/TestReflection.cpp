/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string>
#include <vector>
#include <cinttypes>
#include <Reflection/Reflection.hpp>
#include "TestReflectionHeader.hpp"

TEST_CASE("Reflection Types")
{
    SUBCASE("Check reflectedbuilt-in types")
    {
        CHECK(Reflection::IsReflected<Reflection::NullType>());
        CHECK(Reflection::Reflect<Reflection::NullType>().IsNullType());
        CHECK_EQ(Reflection::Reflect<Reflection::NullType>().Name, "Reflection::NullType");
        CHECK_FALSE(Reflection::Reflect<Undefined>().IsNullType());
        CHECK_FALSE(Reflection::Reflect<Derived>().IsNullType());
    }

    SUBCASE("Check reflected name of type in namespace")
    {
        using namespace Reflection;
        CHECK_EQ(Reflection::Reflect<NullType>().Name, "Reflection::NullType");
    }

    SUBCASE("Check reflection presence for types")
    {
        CHECK_FALSE(Reflection::IsReflected<Undefined>());
        CHECK_FALSE(Reflection::IsReflected<CrossUnit>());
        CHECK(Reflection::IsReflected<Empty>());
        CHECK(Reflection::IsReflected<Base>());
        CHECK(Reflection::IsReflected<Derived>());
        CHECK(Reflection::IsReflected<Inner>());
        CHECK(Reflection::IsReflected<BranchedOne>());
        CHECK(Reflection::IsReflected<BranchedTwo>());
    }

    SUBCASE("Check reflected type names")
    {
        CHECK_EQ(Reflection::Reflect<Empty>().Name, "Empty");
        CHECK_EQ(Reflection::Reflect<Base>().Name, "Base");
        CHECK_EQ(Reflection::Reflect<Derived>().Name, "Derived");
        CHECK_EQ(Reflection::Reflect<Inner>().Name, "Inner");
        CHECK_EQ(Reflection::Reflect<BranchedOne>().Name, "BranchedOne");
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Name, "BranchedTwo");
    }

    SUBCASE("Check reflected type by value")
    {
        CHECK_FALSE(Reflection::Reflect(Undefined()).IsType<Empty>());
        CHECK(Reflection::Reflect(Undefined()).IsType<Undefined>());
        CHECK(Reflection::Reflect(Empty()).IsType<Empty>());
        CHECK(Reflection::Reflect(Base()).IsType<Base>());
        CHECK(Reflection::Reflect(Derived()).IsType<Derived>());
        CHECK(Reflection::Reflect(Inner()).IsType<Inner>());
        CHECK(Reflection::Reflect(BranchedOne()).IsType<BranchedOne>());
        CHECK(Reflection::Reflect(BranchedTwo()).IsType<BranchedTwo>());
    }

    SUBCASE("Check reflection presence for base types")
    {
        CHECK_FALSE(Reflection::Reflect<Undefined>().HasBaseType());
        CHECK_FALSE(Reflection::Reflect<Empty>().HasBaseType());
        CHECK_FALSE(Reflection::Reflect<Base>().HasBaseType());
        CHECK(Reflection::Reflect<Derived>().HasBaseType());
        CHECK_FALSE(Reflection::Reflect<Inner>().HasBaseType());
        CHECK(Reflection::Reflect<BranchedOne>().HasBaseType());
        CHECK(Reflection::Reflect<BranchedTwo>().HasBaseType());
    }

    SUBCASE("Check reflected base type")
    {
        CHECK_EQ(Reflection::Reflect<Derived>().GetBaseType().Name, "Base");
        CHECK_EQ(Reflection::Reflect<BranchedOne>().GetBaseType().Name, "Derived");
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().GetBaseType().Name, "Derived");
        CHECK(Reflection::Reflect<Derived>().GetBaseType().IsType<Base>());
        CHECK(Reflection::Reflect<BranchedOne>().GetBaseType().IsType<Derived>());
        CHECK(Reflection::Reflect<BranchedTwo>().GetBaseType().IsType<Derived>());
        CHECK(Reflection::Reflect<Derived>().IsDerivedFrom<Base>());
        CHECK(Reflection::Reflect<BranchedOne>().IsDerivedFrom<Derived>());
        CHECK(Reflection::Reflect<BranchedTwo>().IsDerivedFrom<Derived>());
        CHECK(Reflection::Reflect<Base>().IsBaseOf<Derived>());
        CHECK(Reflection::Reflect<Derived>().IsBaseOf<BranchedOne>());
        CHECK(Reflection::Reflect<Derived>().IsBaseOf<BranchedOne>());
    }

    SUBCASE("Check reflection attributes for types")
    {
        CHECK_FALSE(Reflection::Reflect<Empty>().HasAttributes());
        CHECK(Reflection::Reflect<Base>().HasAttributes());
        CHECK(Reflection::Reflect<Derived>().HasAttributes());
        CHECK_FALSE(Reflection::Reflect<Inner>().HasAttributes());
        CHECK_FALSE(Reflection::Reflect<BranchedOne>().HasAttributes());
        CHECK(Reflection::Reflect<BranchedTwo>().HasAttributes());
    }

    SUBCASE("Check reflection attribute count")
    {
        CHECK_EQ(Reflection::Reflect<Empty>().Attributes.Count, 0);
        CHECK_EQ(Reflection::Reflect<Base>().Attributes.Count, 1);
        CHECK_EQ(Reflection::Reflect<Derived>().Attributes.Count, 1);
        CHECK_EQ(Reflection::Reflect<Inner>().Attributes.Count, 0);
        CHECK_EQ(Reflection::Reflect<BranchedOne>().Attributes.Count, 0);
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Attributes.Count, 2);
    }

    SUBCASE("Check reflection attribute names")
    {
        CHECK_EQ(Reflection::Reflect<Base>().Attribute<0>().Name, "BaseAttribute");
        CHECK_EQ(Reflection::Reflect<Derived>().Attribute<0>().Name, "DerivedAttribute");
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Attribute<0>().Name, "BranchedAttributeOne");
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Attribute<1>().Name, "BranchedAttributeTwo");
    }

    SUBCASE("Check reflection attribute types")
    {
        CHECK_FALSE(Reflection::Reflect<Base>().Attribute<0>().IsType<DerivedAttribute>());
        CHECK(Reflection::Reflect<Base>().Attribute<0>().IsType<BaseAttribute>());
        CHECK(Reflection::Reflect<Derived>().Attribute<0>().IsType<DerivedAttribute>());
        CHECK(Reflection::Reflect<BranchedTwo>().Attribute<0>().IsType<BranchedAttributeOne>());
        CHECK(Reflection::Reflect<BranchedTwo>().Attribute<1>().IsType<BranchedAttributeTwo>());
    }

    SUBCASE("Check reflection attribute instances")
    {
        CHECK_EQ(Reflection::Reflect<Base>().Attribute<0>().Instance, BaseAttribute());
        CHECK_EQ(Reflection::Reflect<Derived>().Attribute<0>().Instance.state, false);
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Attribute<0>().Instance.modifier, "Small");
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Attribute<1>().Instance.modifier, "Big");
    }

    SUBCASE("Check reflected member count")
    {
        CHECK_EQ(Reflection::Reflect<Empty>().Members.Count, 0);
        CHECK_EQ(Reflection::Reflect<Base>().Members.Count, 2);
        CHECK_EQ(Reflection::Reflect<Derived>().Members.Count, 1);
        CHECK_EQ(Reflection::Reflect<Inner>().Members.Count, 1);
        CHECK_EQ(Reflection::Reflect<BranchedOne>().Members.Count, 2);
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Members.Count, 2);
    }

    SUBCASE("Check reflected member names")
    {
        CHECK_EQ(Reflection::Reflect<Base>().Member<0>().Name, "textWithoutAttribute");
        CHECK_EQ(Reflection::Reflect<Base>().Member<1>().Name, "textPtrWithAttribute");
        CHECK_EQ(Reflection::Reflect<Derived>().Member<0>().Name, "counter");
        CHECK_EQ(Reflection::Reflect<Inner>().Member<0>().Name, "value");
        CHECK_EQ(Reflection::Reflect<BranchedOne>().Member<0>().Name, "toggle");
        CHECK_EQ(Reflection::Reflect<BranchedOne>().Member<1>().Name, "inner");
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Member<0>().Name, "letterOne");
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Member<1>().Name, "letterTwo");
    }

    SUBCASE("Check reflected member types")
    {
        CHECK_FALSE(Reflection::Reflect<Base>().Member<0>().IsType<void>());
        CHECK(Reflection::Reflect<Base>().Member<0>().IsType<std::string>());
        CHECK(Reflection::Reflect<Base>().Member<1>().IsType<const char*>());
        CHECK(Reflection::Reflect<Derived>().Member<0>().IsType<int>());
        CHECK(Reflection::Reflect<BranchedOne>().Member<0>().IsType<bool>());
        CHECK(Reflection::Reflect<BranchedOne>().Member<1>().IsType<Inner>());
        CHECK(Reflection::Reflect<BranchedTwo>().Member<0>().IsType<char>());
        CHECK(Reflection::Reflect<BranchedTwo>().Member<1>().IsType<char>());
    }

    SUBCASE("Check reflected member pointers")
    {
        CHECK_EQ(Reflection::Reflect<Base>().Member<0>().Pointer, &Base::textWithoutAttribute);
        CHECK_EQ(Reflection::Reflect<Base>().Member<1>().Pointer, &Base::textPtrWithAttribute);
        CHECK_EQ(Reflection::Reflect<Derived>().Member<0>().Pointer, &Derived::counter);
        CHECK_EQ(Reflection::Reflect<Inner>().Member<0>().Pointer, &Inner::value);
        CHECK_EQ(Reflection::Reflect<BranchedOne>().Member<0>().Pointer, &BranchedOne::toggle);
        CHECK_EQ(Reflection::Reflect<BranchedOne>().Member<1>().Pointer, &BranchedOne::inner);
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Member<0>().Pointer, &BranchedTwo::letterOne);
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Member<1>().Pointer, &BranchedTwo::letterTwo);
    }

    SUBCASE("Check reflected attribute count for members")
    {
        CHECK_EQ(Reflection::Reflect<Base>().Member<0>().Attributes.Count, 0);
        CHECK_EQ(Reflection::Reflect<Base>().Member<1>().Attributes.Count, 1);
        CHECK_EQ(Reflection::Reflect<Derived>().Member<0>().Attributes.Count, 1);
        CHECK_EQ(Reflection::Reflect<Inner>().Member<0>().Attributes.Count, 1);
        CHECK_EQ(Reflection::Reflect<BranchedOne>().Member<0>().Attributes.Count, 2);
        CHECK_EQ(Reflection::Reflect<BranchedOne>().Member<1>().Attributes.Count, 0);
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Member<0>().Attributes.Count, 1);
        CHECK_EQ(Reflection::Reflect<BranchedTwo>().Member<1>().Attributes.Count, 1);
    }

    SUBCASE("Check reflected attribute names for members")
    {
        CHECK_EQ(Reflection::Reflect<Base>()
            .Member<1>().Attribute<0>().Name, "TextAttribute");
        CHECK_EQ(Reflection::Reflect<Derived>()
            .Member<0>().Attribute<0>().Name, "CounterAttribute");
        CHECK_EQ(Reflection::Reflect<Inner>()
            .Member<0>().Attribute<0>().Name, "InnerAttribute");
        CHECK_EQ(Reflection::Reflect<BranchedOne>()
            .Member<0>().Attribute<0>().Name, "ToggleOnAttribute");
        CHECK_EQ(Reflection::Reflect<BranchedOne>()
            .Member<0>().Attribute<1>().Name, "ToggleOffAttribute");
        CHECK_EQ(Reflection::Reflect<BranchedTwo>()
            .Member<0>().Attribute<0>().Name, "LetterAttribute");
        CHECK_EQ(Reflection::Reflect<BranchedTwo>()
            .Member<1>().Attribute<0>().Name, "LetterAttribute");
    }

    SUBCASE("Check reflected attribute types for members")
    {
        CHECK(Reflection::Reflect<Base>()
            .Member<1>().Attribute<0>().IsType<TextAttribute>());
        CHECK(Reflection::Reflect<Derived>()
            .Member<0>().Attribute<0>().IsType<CounterAttribute>());
        CHECK(Reflection::Reflect<Inner>()
            .Member<0>().Attribute<0>().IsType<InnerAttribute>());
        CHECK(Reflection::Reflect<BranchedOne>()
            .Member<0>().Attribute<0>().IsType<ToggleOnAttribute>());
        CHECK(Reflection::Reflect<BranchedOne>()
            .Member<0>().Attribute<1>().IsType<ToggleOffAttribute>());
        CHECK(Reflection::Reflect<BranchedTwo>()
            .Member<0>().Attribute<0>().IsType<LetterAttribute>());
        CHECK(Reflection::Reflect<BranchedTwo>()
            .Member<1>().Attribute<0>().IsType<LetterAttribute>());
    }

    SUBCASE("Check reflected attribute instance for members")
    {
        CHECK_EQ(Reflection::Reflect<Base>()
            .Member<1>().Attribute<0>().Instance, TextAttribute());
        CHECK_EQ(Reflection::Reflect<Derived>()
            .Member<0>().Attribute<0>().Instance.state, true);
        CHECK_EQ(Reflection::Reflect<Inner>()
            .Member<0>().Attribute<0>().Instance.counter, 20);
        CHECK_EQ(Reflection::Reflect<BranchedOne>()
            .Member<0>().Attribute<0>().Instance.state, true);
        CHECK_EQ(Reflection::Reflect<BranchedOne>()
            .Member<0>().Attribute<1>().Instance.state, false);
        CHECK_EQ(Reflection::Reflect<BranchedTwo>()
            .Member<0>().Attribute<0>().Instance.modifier, "Pretty");
        CHECK_EQ(Reflection::Reflect<BranchedTwo>()
            .Member<1>().Attribute<0>().Instance.modifier, "Ugly");
    }

    SUBCASE("Enumerate reflected attribtues")
    {
        SUBCASE("For base type")
        {
            std::vector<std::string_view> expectedAttributes;
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::Reflect<Empty>().Attributes,
                [&presentAttributes](const auto& attribute)
                {
                    presentAttributes.push_back(attribute.Name);
                }
            );

            CHECK_EQ(presentAttributes, expectedAttributes);
        }

        SUBCASE("For derived type")
        {
            std::vector<std::string_view> expectedAttributes = { "DerivedAttribute" };
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::Reflect<Derived>().Attributes,
                [&presentAttributes](const auto& attribute)
                {
                    presentAttributes.push_back(attribute.Name);
                }
            );

            CHECK_EQ(presentAttributes, expectedAttributes);
        }

        SUBCASE("For branched type")
        {
            std::vector<std::string_view> expectedAttributes =
                { "BranchedAttributeOne", "BranchedAttributeTwo" };
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::Reflect<BranchedTwo>().Attributes,
                [&presentAttributes](const auto& attribute)
                {
                    presentAttributes.push_back(attribute.Name);
                }
            );

            CHECK_EQ(presentAttributes, expectedAttributes);
        }

        SUBCASE("For their instances")
        {
            std::vector<std::string_view> expectedAttributes = { "Small", "Big" };
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::Reflect<BranchedTwo>().Attributes,
                [&presentAttributes](const auto& attribute)
                {
                    presentAttributes.push_back(attribute.Instance.modifier);
                }
            );

            CHECK_EQ(presentAttributes, expectedAttributes);
        }
    }

    SUBCASE("Enumarate reflected members")
    {
        SUBCASE("For empty type")
        {
            std::vector<std::string_view> expectedMembers;
            std::vector<std::string_view> presentMembers;

            Reflection::ForEach(Reflection::Reflect<Empty>().Members,
                [&presentMembers](const auto& member)
                {
                    presentMembers.push_back(member.Name);
                }
            );

            CHECK_EQ(presentMembers, expectedMembers);
        }

        SUBCASE("For base type")
        {
            std::vector<std::string_view> expectedMembers =
                { "textWithoutAttribute", "textPtrWithAttribute" };
            std::vector<std::string_view> presentMembers;

            Reflection::ForEach(Reflection::Reflect<Base>().Members,
                [&presentMembers](const auto& member)
                {
                    presentMembers.push_back(member.Name);
                }
            );

            CHECK_EQ(presentMembers, expectedMembers);
        }

        SUBCASE("For derived type")
        {
            std::vector<std::string_view> expectedMembers = { "toggle", "inner" };
            std::vector<std::string_view> presentMembers;

            Reflection::ForEach(Reflection::Reflect<BranchedOne>().Members,
                [&presentMembers](const auto& member)
                {
                    presentMembers.push_back(member.Name);
                }
            );

            CHECK_EQ(presentMembers, expectedMembers);
        }
    }

    SUBCASE("Enumerate reflected member attributes")
    {
        SUBCASE("For base type")
        {
            std::vector<std::string_view> expectedAttributes;
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::Reflect<Base>().Member<0>().Attributes,
                [&presentAttributes](const auto& attribute)
                {
                    presentAttributes.push_back(attribute.Name);
                }
            );

            CHECK_EQ(presentAttributes, expectedAttributes);
        }

        SUBCASE("For derived type")
        {
            std::vector<std::string_view> expectedAttributes =
                { "ToggleOnAttribute", "ToggleOffAttribute" };
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::Reflect<BranchedOne>().Member<0>().Attributes,
                [&presentAttributes](const auto& attribute)
                {
                    presentAttributes.push_back(attribute.Name);
                }
            );

            CHECK_EQ(presentAttributes, expectedAttributes);
        }
    }

    // TODO: Enumerate derived types from type.
    // TODO: Test reflection between compilation units.
}

constexpr char BaseMemberName[] = "textWithoutAttribute";
constexpr char DerivedAttributeName[] = "DerivedAttribute";
constexpr char DerivedMemberName[] = "counter";
constexpr char DerivedMemberAttributeName[] = "CounterAttribute";

TEST_CASE("Reflection Experimental")
{
    // Find type members and attributes by name.
    // String literal needs to be a static variable passed via template argument to
    // be allowed in constexpr evaluation. This limitation will be lifted in C++20.
    // Disabled for now because of issues with compilation on Clang (GCC/MSVC is fine).
    // Maybe use constexpr hashing or somehow allow to evaluate via argument.
    /*
    CHECK_EQ(Reflection::Reflect<Base>()
        .FindMember<BaseMemberName>().Name, "textWithoutAttribute");
    CHECK_EQ(Reflection::Reflect<Derived>()
        .FindAttribute<DerivedAttributeName>().Name, "DerivedAttribute");
    CHECK_EQ(Reflection::Reflect<Derived>()
        .FindAttribute<DerivedAttributeName>().Instance.state, false);
    CHECK_EQ(Reflection::Reflect<Derived>()
        .FindMember<DerivedMemberName>()
        .FindAttribute<DerivedMemberAttributeName>().Instance.state, true);
    */
}

TEST_CASE("Reflection Instantiate")
{
    // TODO: Instantiate derived using its type identifier.
    // Base* instance = Base::create(Derived::Type);
    // CHECK_EQ(GetType(instance) == Derived::Type);
}

TEST_CASE("Reflection Cast")
{
    // TODO: Test casting from one type to another.
}

TEST_CASE("Reflection Super")
{
    // TODO: Test super typedef for classes.
}
