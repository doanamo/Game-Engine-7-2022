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

TEST_CASE("Static Reflection")
{
    SUBCASE("Check reflected built-in types")
    {
        CHECK(Reflection::IsReflected<Reflection::NullType>());
        CHECK(Reflection::IsReflected<Reflection::TypeAttribute>());
        CHECK(Reflection::IsReflected<Reflection::FieldAttribute>());
        CHECK(Reflection::IsReflected<Reflection::MethodAttribute>());

        CHECK(Reflection::StaticType<Reflection::NullType>().IsNullType());
        CHECK_EQ(Reflection::StaticType<Reflection::NullType>().Name, "Reflection::NullType");
        CHECK_FALSE(Reflection::StaticType<Undefined>().IsNullType());
        CHECK_FALSE(Reflection::StaticType<Derived>().IsNullType());
    }

    SUBCASE("Check reflected type name from namespace")
    {
        using namespace Reflection;
        CHECK_EQ(Reflection::StaticType<NullType>().Name, "Reflection::NullType");
    }

    SUBCASE("Check reflection for types")
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

    SUBCASE("Check reflection for const types")
    {
        CHECK_FALSE(Reflection::IsReflected<const Undefined>());
        CHECK_FALSE(Reflection::IsReflected<const CrossUnit>());
        CHECK(Reflection::IsReflected<const Empty>());
        CHECK(Reflection::IsReflected<const Base>());

        volatile Derived derived;
        CHECK(Reflection::IsReflected(derived));

        const BranchedOne& branchedOne = BranchedOne();
        CHECK(Reflection::IsReflected(branchedOne));

        const BranchedTwo* branchedTwo = nullptr;
        CHECK(Reflection::IsReflected(branchedTwo));

        CHECK_EQ(Reflection::GetIdentifier<const Empty&>(), Reflection::GetIdentifier<Empty>());
        CHECK_EQ(Reflection::GetIdentifier<volatile Empty*>(), Reflection::GetIdentifier<Empty>());
    }

    SUBCASE("Check reflected type names")
    {
        CHECK_EQ(Reflection::StaticType<Empty>().Name, "Empty");
        CHECK_EQ(Reflection::StaticType<Base>().Name, "Base");
        CHECK_EQ(Reflection::StaticType<Derived>().Name, "Derived");
        CHECK_EQ(Reflection::StaticType<Inner>().Name, "Inner");
        CHECK_EQ(Reflection::StaticType<BranchedOne>().Name, "BranchedOne");
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Name, "BranchedTwo");
    }

    SUBCASE("Check reflected type by value")
    {
        CHECK_FALSE(Reflection::StaticType(Undefined()).IsType<Empty>());
        CHECK(Reflection::StaticType(Undefined()).IsType<Undefined>());
        CHECK(Reflection::StaticType(Empty()).IsType<Empty>());
        CHECK(Reflection::StaticType(Base()).IsType<Base>());
        CHECK(Reflection::StaticType(Derived()).IsType<Derived>());
        CHECK(Reflection::StaticType(Inner()).IsType<Inner>());
        CHECK(Reflection::StaticType(BranchedOne()).IsType<BranchedOne>());
        CHECK(Reflection::StaticType(BranchedTwo()).IsType<BranchedTwo>());
    }

    SUBCASE("Check reflected type for base type")
    {
        CHECK_FALSE(Reflection::StaticType<Undefined>().HasBaseType());
        CHECK_FALSE(Reflection::StaticType<Empty>().HasBaseType());
        CHECK_FALSE(Reflection::StaticType<Base>().HasBaseType());
        CHECK(Reflection::StaticType<Derived>().HasBaseType());
        CHECK_FALSE(Reflection::StaticType<Inner>().HasBaseType());
        CHECK(Reflection::StaticType<BranchedOne>().HasBaseType());
        CHECK(Reflection::StaticType<BranchedTwo>().HasBaseType());
    }

    SUBCASE("Check reflected base type")
    {
        CHECK_EQ(Reflection::StaticType<Derived>().GetBaseType().Name, "Base");
        CHECK_EQ(Reflection::StaticType<BranchedOne>().GetBaseType().Name, "Derived");
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().GetBaseType().Name, "Derived");
        CHECK(Reflection::StaticType<Derived>().GetBaseType().IsType<Base>());
        CHECK(Reflection::StaticType<BranchedOne>().GetBaseType().IsType<Derived>());
        CHECK(Reflection::StaticType<BranchedTwo>().GetBaseType().IsType<Derived>());
        CHECK(Reflection::StaticType<Derived>().IsDerivedFrom<Base>());
        CHECK(Reflection::StaticType<BranchedOne>().IsDerivedFrom<Derived>());
        CHECK(Reflection::StaticType<BranchedTwo>().IsDerivedFrom<Derived>());
        CHECK(Reflection::StaticType<Base>().IsBaseOf<Derived>());
        CHECK(Reflection::StaticType<Derived>().IsBaseOf<BranchedOne>());
        CHECK(Reflection::StaticType<Derived>().IsBaseOf<BranchedOne>());
    }

    SUBCASE("Check reflected type attributes")
    {
        CHECK_FALSE(Reflection::StaticType<Empty>().HasAttributes());
        CHECK(Reflection::StaticType<Base>().HasAttributes());
        CHECK(Reflection::StaticType<Derived>().HasAttributes());
        CHECK_FALSE(Reflection::StaticType<Inner>().HasAttributes());
        CHECK_FALSE(Reflection::StaticType<BranchedOne>().HasAttributes());
        CHECK(Reflection::StaticType<BranchedTwo>().HasAttributes());
    }

    SUBCASE("Check reflected type attribute count")
    {
        CHECK_EQ(Reflection::StaticType<Empty>().Attributes.Count, 0);
        CHECK_EQ(Reflection::StaticType<Base>().Attributes.Count, 1);
        CHECK_EQ(Reflection::StaticType<Derived>().Attributes.Count, 1);
        CHECK_EQ(Reflection::StaticType<Inner>().Attributes.Count, 0);
        CHECK_EQ(Reflection::StaticType<BranchedOne>().Attributes.Count, 0);
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Attributes.Count, 2);
    }

    SUBCASE("Check reflected type attribute names")
    {
        CHECK_EQ(Reflection::StaticType<Base>().Attribute<0>().Name, "BaseAttribute");
        CHECK_EQ(Reflection::StaticType<Derived>().Attribute<0>().Name, "DerivedAttribute");
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Attribute<0>().Name, "BranchedAttributeOne");
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Attribute<1>().Name, "BranchedAttributeTwo");
    }

    SUBCASE("Check reflected type attribute types")
    {
        CHECK_FALSE(Reflection::StaticType<Base>().Attribute<0>().IsType<DerivedAttribute>());
        CHECK(Reflection::StaticType<Base>().Attribute<0>().IsType<BaseAttribute>());
        CHECK(Reflection::StaticType<Derived>().Attribute<0>().IsType<DerivedAttribute>());
        CHECK(Reflection::StaticType<BranchedTwo>().Attribute<0>().IsType<BranchedAttributeOne>());
        CHECK(Reflection::StaticType<BranchedTwo>().Attribute<1>().IsType<BranchedAttributeTwo>());
    }

    SUBCASE("Check reflected type attribute instances")
    {
        CHECK_EQ(Reflection::StaticType<Base>().Attribute<0>().Instance, BaseAttribute());
        CHECK_EQ(Reflection::StaticType<Derived>().Attribute<0>().Instance.state, false);
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Attribute<0>().Instance.modifier, "Small");
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Attribute<1>().Instance.modifier, "Big");
    }

    SUBCASE("Check reflected type member count")
    {
        CHECK_EQ(Reflection::StaticType<Empty>().Members.Count, 0);
        CHECK_EQ(Reflection::StaticType<Base>().Members.Count, 2);
        CHECK_EQ(Reflection::StaticType<Derived>().Members.Count, 1);
        CHECK_EQ(Reflection::StaticType<Inner>().Members.Count, 1);
        CHECK_EQ(Reflection::StaticType<BranchedOne>().Members.Count, 2);
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Members.Count, 2);
    }

    SUBCASE("Check reflected type member names")
    {
        CHECK_EQ(Reflection::StaticType<Base>().Member<0>().Name, "textWithoutAttribute");
        CHECK_EQ(Reflection::StaticType<Base>().Member<1>().Name, "textPtrWithAttribute");
        CHECK_EQ(Reflection::StaticType<Derived>().Member<0>().Name, "counter");
        CHECK_EQ(Reflection::StaticType<Inner>().Member<0>().Name, "value");
        CHECK_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Name, "toggle");
        CHECK_EQ(Reflection::StaticType<BranchedOne>().Member<1>().Name, "inner");
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Member<0>().Name, "letterOne");
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Member<1>().Name, "letterTwo");
    }

    SUBCASE("Check reflected type member types")
    {
        CHECK_FALSE(Reflection::StaticType<Base>().Member<0>().IsType<void>());
        CHECK(Reflection::StaticType<Base>().Member<0>().IsType<std::string>());
        CHECK(Reflection::StaticType<Base>().Member<1>().IsType<const char*>());
        CHECK(Reflection::StaticType<Derived>().Member<0>().IsType<int>());
        CHECK(Reflection::StaticType<BranchedOne>().Member<0>().IsType<bool>());
        CHECK(Reflection::StaticType<BranchedOne>().Member<1>().IsType<Inner>());
        CHECK(Reflection::StaticType<BranchedTwo>().Member<0>().IsType<char>());
        CHECK(Reflection::StaticType<BranchedTwo>().Member<1>().IsType<char>());
    }

    SUBCASE("Check reflected type member pointers")
    {
        CHECK_EQ(Reflection::StaticType<Base>().Member<0>().Pointer, &Base::textWithoutAttribute);
        CHECK_EQ(Reflection::StaticType<Base>().Member<1>().Pointer, &Base::textPtrWithAttribute);
        CHECK_EQ(Reflection::StaticType<Derived>().Member<0>().Pointer, &Derived::counter);
        CHECK_EQ(Reflection::StaticType<Inner>().Member<0>().Pointer, &Inner::value);
        CHECK_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Pointer, &BranchedOne::_toggle);
        CHECK_EQ(Reflection::StaticType<BranchedOne>().Member<1>().Pointer, &BranchedOne::_inner);
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Member<0>().Pointer, &BranchedTwo::m_letterOne);
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Member<1>().Pointer, &BranchedTwo::m_letterTwo);
    }

    SUBCASE("Check reflected member attribute count")
    {
        CHECK_EQ(Reflection::StaticType<Base>().Member<0>().Attributes.Count, 0);
        CHECK_EQ(Reflection::StaticType<Base>().Member<1>().Attributes.Count, 1);
        CHECK_EQ(Reflection::StaticType<Derived>().Member<0>().Attributes.Count, 1);
        CHECK_EQ(Reflection::StaticType<Inner>().Member<0>().Attributes.Count, 1);
        CHECK_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Attributes.Count, 2);
        CHECK_EQ(Reflection::StaticType<BranchedOne>().Member<1>().Attributes.Count, 0);
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Member<0>().Attributes.Count, 1);
        CHECK_EQ(Reflection::StaticType<BranchedTwo>().Member<1>().Attributes.Count, 1);
    }

    SUBCASE("Check reflected member attribute names")
    {
        CHECK_EQ(Reflection::StaticType<Base>()
            .Member<1>().Attribute<0>().Name, "TextAttribute");
        CHECK_EQ(Reflection::StaticType<Derived>()
            .Member<0>().Attribute<0>().Name, "CounterAttribute");
        CHECK_EQ(Reflection::StaticType<Inner>()
            .Member<0>().Attribute<0>().Name, "InnerAttribute");
        CHECK_EQ(Reflection::StaticType<BranchedOne>()
            .Member<0>().Attribute<0>().Name, "ToggleOnAttribute");
        CHECK_EQ(Reflection::StaticType<BranchedOne>()
            .Member<0>().Attribute<1>().Name, "ToggleOffAttribute");
        CHECK_EQ(Reflection::StaticType<BranchedTwo>()
            .Member<0>().Attribute<0>().Name, "LetterAttribute");
        CHECK_EQ(Reflection::StaticType<BranchedTwo>()
            .Member<1>().Attribute<0>().Name, "LetterAttribute");
    }

    SUBCASE("Check reflected member attribute types")
    {
        CHECK(Reflection::StaticType<Base>()
            .Member<1>().Attribute<0>().IsType<TextAttribute>());
        CHECK(Reflection::StaticType<Derived>()
            .Member<0>().Attribute<0>().IsType<CounterAttribute>());
        CHECK(Reflection::StaticType<Inner>()
            .Member<0>().Attribute<0>().IsType<InnerAttribute>());
        CHECK(Reflection::StaticType<BranchedOne>()
            .Member<0>().Attribute<0>().IsType<ToggleOnAttribute>());
        CHECK(Reflection::StaticType<BranchedOne>()
            .Member<0>().Attribute<1>().IsType<ToggleOffAttribute>());
        CHECK(Reflection::StaticType<BranchedTwo>()
            .Member<0>().Attribute<0>().IsType<LetterAttribute>());
        CHECK(Reflection::StaticType<BranchedTwo>()
            .Member<1>().Attribute<0>().IsType<LetterAttribute>());
    }

    SUBCASE("Check reflected members attribute instances")
    {
        CHECK_EQ(Reflection::StaticType<Base>()
            .Member<1>().Attribute<0>().Instance, TextAttribute());
        CHECK_EQ(Reflection::StaticType<Derived>()
            .Member<0>().Attribute<0>().Instance.state, true);
        CHECK_EQ(Reflection::StaticType<Inner>()
            .Member<0>().Attribute<0>().Instance.counter, 20);
        CHECK_EQ(Reflection::StaticType<BranchedOne>()
            .Member<0>().Attribute<0>().Instance.state, true);
        CHECK_EQ(Reflection::StaticType<BranchedOne>()
            .Member<0>().Attribute<1>().Instance.state, false);
        CHECK_EQ(Reflection::StaticType<BranchedTwo>()
            .Member<0>().Attribute<0>().Instance.modifier, "Pretty");
        CHECK_EQ(Reflection::StaticType<BranchedTwo>()
            .Member<1>().Attribute<0>().Instance.modifier, "Ugly");
    }

    SUBCASE("Enumerate reflected attribtues")
    {
        SUBCASE("For base type")
        {
            std::vector<std::string_view> expectedAttributes;
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::StaticType<Empty>().Attributes,
                [&presentAttributes](const auto& attribute)
            {
                presentAttributes.push_back(attribute.Name);
            });

            CHECK_EQ(presentAttributes, expectedAttributes);
        }

        SUBCASE("For derived type")
        {
            std::vector<std::string_view> expectedAttributes = { "DerivedAttribute" };
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::StaticType<Derived>().Attributes,
                [&presentAttributes](const auto& attribute)
            {
                presentAttributes.push_back(attribute.Name);
            });

            CHECK_EQ(presentAttributes, expectedAttributes);
        }

        SUBCASE("For branched type")
        {
            std::vector<std::string_view> expectedAttributes =
            { "BranchedAttributeOne", "BranchedAttributeTwo" };
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::StaticType<BranchedTwo>().Attributes,
                [&presentAttributes](const auto& attribute)
            {
                presentAttributes.push_back(attribute.Name);
            });

            CHECK_EQ(presentAttributes, expectedAttributes);
        }

        SUBCASE("For their instances")
        {
            std::vector<std::string_view> expectedAttributes = { "Small", "Big" };
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::StaticType<BranchedTwo>().Attributes,
                [&presentAttributes](const auto& attribute)
            {
                presentAttributes.push_back(attribute.Instance.modifier);
            });

            CHECK_EQ(presentAttributes, expectedAttributes);
        }
    }

    SUBCASE("Enumarate reflected members")
    {
        SUBCASE("For empty type")
        {
            std::vector<std::string_view> expectedMembers;
            std::vector<std::string_view> presentMembers;

            Reflection::ForEach(Reflection::StaticType<Empty>().Members,
                [&presentMembers](const auto& member)
            {
                presentMembers.push_back(member.Name);
            });

            CHECK_EQ(presentMembers, expectedMembers);
        }

        SUBCASE("For base type")
        {
            std::vector<std::string_view> expectedMembers =
            { "textWithoutAttribute", "textPtrWithAttribute" };
            std::vector<std::string_view> presentMembers;

            Reflection::ForEach(Reflection::StaticType<Base>().Members,
                [&presentMembers](const auto& member)
            {
                presentMembers.push_back(member.Name);
            });

            CHECK_EQ(presentMembers, expectedMembers);
        }

        SUBCASE("For derived type")
        {
            std::vector<std::string_view> expectedMembers = { "toggle", "inner" };
            std::vector<std::string_view> presentMembers;

            Reflection::ForEach(Reflection::StaticType<BranchedOne>().Members,
                [&presentMembers](const auto& member)
            {
                presentMembers.push_back(member.Name);
            });

            CHECK_EQ(presentMembers, expectedMembers);
        }
    }

    SUBCASE("Enumerate reflected member attributes")
    {
        SUBCASE("For base type")
        {
            std::vector<std::string_view> expectedAttributes;
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::StaticType<Base>().Member<0>().Attributes,
                [&presentAttributes](const auto& attribute)
            {
                presentAttributes.push_back(attribute.Name);
            });

            CHECK_EQ(presentAttributes, expectedAttributes);
        }

        SUBCASE("For derived type")
        {
            std::vector<std::string_view> expectedAttributes =
            { "ToggleOnAttribute", "ToggleOffAttribute" };
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::StaticType<BranchedOne>().Member<0>().Attributes,
                [&presentAttributes](const auto& attribute)
            {
                presentAttributes.push_back(attribute.Name);
            });

            CHECK_EQ(presentAttributes, expectedAttributes);
        }
    }

    SUBCASE("Experimental")
    {
        constexpr char BaseMemberName[] = "textWithoutAttribute";
        constexpr char DerivedAttributeName[] = "DerivedAttribute";
        constexpr char DerivedMemberName[] = "counter";
        constexpr char DerivedMemberAttributeName[] = "CounterAttribute";

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

    // TODO: Enumerate derived types from type.
    // TODO: Test reflection between compilation units.
}
