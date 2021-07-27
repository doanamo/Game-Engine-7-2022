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

DOCTEST_TEST_CASE("Static Reflection")
{
    DOCTEST_SUBCASE("DOCTEST_CHECK reflected built-in types")
    {
        DOCTEST_CHECK(Reflection::IsReflected<Reflection::NullType>());
        DOCTEST_CHECK(Reflection::IsReflected<Reflection::TypeAttribute>());
        DOCTEST_CHECK(Reflection::IsReflected<Reflection::FieldAttribute>());
        DOCTEST_CHECK(Reflection::IsReflected<Reflection::MethodAttribute>());

        DOCTEST_CHECK(Reflection::StaticType<Reflection::NullType>().IsNullType());
        DOCTEST_CHECK_EQ(Reflection::StaticType<Reflection::NullType>().Name,
            "Reflection::NullType");
        DOCTEST_CHECK_FALSE(Reflection::StaticType<Undefined>().IsNullType());
        DOCTEST_CHECK_FALSE(Reflection::StaticType<Derived>().IsNullType());
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type name from namespace")
    {
        using namespace Reflection;
        DOCTEST_CHECK_EQ(Reflection::StaticType<NullType>().Name, "Reflection::NullType");
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflection for types")
    {
        DOCTEST_CHECK_FALSE(Reflection::IsReflected<Undefined>());
        DOCTEST_CHECK_FALSE(Reflection::IsReflected<CrossUnit>());
        DOCTEST_CHECK(Reflection::IsReflected<Empty>());
        DOCTEST_CHECK(Reflection::IsReflected<Base>());
        DOCTEST_CHECK(Reflection::IsReflected<Derived>());
        DOCTEST_CHECK(Reflection::IsReflected<Inner>());
        DOCTEST_CHECK(Reflection::IsReflected<BranchedOne>());
        DOCTEST_CHECK(Reflection::IsReflected<BranchedTwo>());
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflection for const types")
    {
        DOCTEST_CHECK_FALSE(Reflection::IsReflected<const Undefined>());
        DOCTEST_CHECK_FALSE(Reflection::IsReflected<const CrossUnit>());
        DOCTEST_CHECK(Reflection::IsReflected<const Empty>());
        DOCTEST_CHECK(Reflection::IsReflected<const Base>());

        volatile Derived derived;
        DOCTEST_CHECK(Reflection::IsReflected(derived));

        const BranchedOne& branchedOne = BranchedOne();
        DOCTEST_CHECK(Reflection::IsReflected(branchedOne));

        const BranchedTwo* branchedTwo = nullptr;
        DOCTEST_CHECK(Reflection::IsReflected(branchedTwo));

        DOCTEST_CHECK_EQ(Reflection::GetIdentifier<const Empty&>(),
            Reflection::GetIdentifier<Empty>());
        DOCTEST_CHECK_EQ(Reflection::GetIdentifier<volatile Empty*>(),
            Reflection::GetIdentifier<Empty>());
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type names")
    {
        DOCTEST_CHECK_EQ(Reflection::StaticType<Empty>().Name, "Empty");
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>().Name, "Base");
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>().Name, "Derived");
        DOCTEST_CHECK_EQ(Reflection::StaticType<Inner>().Name, "Inner");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>().Name, "BranchedOne");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Name, "BranchedTwo");
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type names against identifiers")
    {
        DOCTEST_CHECK_EQ(Reflection::GetName<Empty>(),
            Reflection::GetIdentifier<Empty>());
        DOCTEST_CHECK_EQ(Reflection::GetName<Base>(),
            Reflection::GetIdentifier<Base>());
        DOCTEST_CHECK_EQ(Reflection::GetName<Derived>(),
            Reflection::GetIdentifier<Derived>());
        DOCTEST_CHECK_EQ(Reflection::GetName<Inner>(),
            Reflection::GetIdentifier<Inner>());
        DOCTEST_CHECK_EQ(Reflection::GetName<BranchedOne>(),
            Reflection::GetIdentifier<BranchedOne>());
        DOCTEST_CHECK_EQ(Reflection::GetName<BranchedTwo>(),
            Reflection::GetIdentifier<BranchedTwo>());
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type identifier")
    {
        DOCTEST_CHECK_EQ(Reflection::StaticType<Empty>().Identifier,
            Reflection::GetIdentifier("Empty"));
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>().Identifier,
            Reflection::GetIdentifier("Base"));
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>().Identifier,
            Reflection::GetIdentifier("Derived"));
        DOCTEST_CHECK_EQ(Reflection::StaticType<Inner>().Identifier,
            Reflection::GetIdentifier("Inner"));
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>().Identifier,
            Reflection::GetIdentifier("BranchedOne"));
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Identifier,
            Reflection::GetIdentifier("BranchedTwo"));

        DOCTEST_CHECK_NE(Reflection::StaticType<BranchedOne>().Identifier,
            Reflection::StaticType<BranchedTwo>().Identifier);
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type by value")
    {
        DOCTEST_CHECK_FALSE(Reflection::StaticType(Undefined()).IsType<Empty>());
        DOCTEST_CHECK(Reflection::StaticType(Undefined()).IsType<Undefined>());
        DOCTEST_CHECK(Reflection::StaticType(Empty()).IsType<Empty>());
        DOCTEST_CHECK(Reflection::StaticType(Base()).IsType<Base>());
        DOCTEST_CHECK(Reflection::StaticType(Derived()).IsType<Derived>());
        DOCTEST_CHECK(Reflection::StaticType(Inner()).IsType<Inner>());
        DOCTEST_CHECK(Reflection::StaticType(BranchedOne()).IsType<BranchedOne>());
        DOCTEST_CHECK(Reflection::StaticType(BranchedTwo()).IsType<BranchedTwo>());
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type for base type")
    {
        DOCTEST_CHECK_FALSE(Reflection::StaticType<Undefined>().HasBaseType());
        DOCTEST_CHECK_FALSE(Reflection::StaticType<Empty>().HasBaseType());
        DOCTEST_CHECK_FALSE(Reflection::StaticType<Base>().HasBaseType());
        DOCTEST_CHECK(Reflection::StaticType<Derived>().HasBaseType());
        DOCTEST_CHECK_FALSE(Reflection::StaticType<Inner>().HasBaseType());
        DOCTEST_CHECK(Reflection::StaticType<BranchedOne>().HasBaseType());
        DOCTEST_CHECK(Reflection::StaticType<BranchedTwo>().HasBaseType());
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected base type")
    {
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>().GetBaseType().Name, "Base");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>().GetBaseType().Name, "Derived");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().GetBaseType().Name, "Derived");
        DOCTEST_CHECK(Reflection::StaticType<Derived>().GetBaseType().IsType<Base>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedOne>().GetBaseType().IsType<Derived>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedTwo>().GetBaseType().IsType<Derived>());
        DOCTEST_CHECK(Reflection::StaticType<Derived>().IsDerivedFrom<Base>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedOne>().IsDerivedFrom<Derived>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedTwo>().IsDerivedFrom<Derived>());
        DOCTEST_CHECK(Reflection::StaticType<Base>().IsBaseOf<Derived>());
        DOCTEST_CHECK(Reflection::StaticType<Derived>().IsBaseOf<BranchedOne>());
        DOCTEST_CHECK(Reflection::StaticType<Derived>().IsBaseOf<BranchedOne>());
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type attributes")
    {
        DOCTEST_CHECK_FALSE(Reflection::StaticType<Empty>().HasAttributes());
        DOCTEST_CHECK(Reflection::StaticType<Base>().HasAttributes());
        DOCTEST_CHECK(Reflection::StaticType<Derived>().HasAttributes());
        DOCTEST_CHECK_FALSE(Reflection::StaticType<Inner>().HasAttributes());
        DOCTEST_CHECK_FALSE(Reflection::StaticType<BranchedOne>().HasAttributes());
        DOCTEST_CHECK(Reflection::StaticType<BranchedTwo>().HasAttributes());
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type attribute count")
    {
        DOCTEST_CHECK_EQ(Reflection::StaticType<Empty>().Attributes.Count, 0);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>().Attributes.Count, 1);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>().Attributes.Count, 1);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Inner>().Attributes.Count, 0);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>().Attributes.Count, 0);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Attributes.Count, 2);
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type attribute names")
    {
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>().Attribute<0>().Name,
            "BaseAttribute");
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>().Attribute<0>().Name,
            "DerivedAttribute");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Attribute<0>().Name,
            "BranchedAttributeOne");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Attribute<1>().Name,
            "BranchedAttributeTwo");
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type attribute types")
    {
        DOCTEST_CHECK_FALSE(Reflection::StaticType<Base>()
            .Attribute<0>().IsType<DerivedAttribute>());
        DOCTEST_CHECK(Reflection::StaticType<Base>()
            .Attribute<0>().IsType<BaseAttribute>());
        DOCTEST_CHECK(Reflection::StaticType<Derived>()
            .Attribute<0>().IsType<DerivedAttribute>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedTwo>()
            .Attribute<0>().IsType<BranchedAttributeOne>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedTwo>()
            .Attribute<1>().IsType<BranchedAttributeTwo>());
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type attribute instances")
    {
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>().Attribute<0>().Instance,
            BaseAttribute());
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>().Attribute<0>().Instance.state,
            false);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Attribute<0>().Instance.modifier,
            "Small");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Attribute<1>().Instance.modifier,
            "Big");
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type member count")
    {
        DOCTEST_CHECK_EQ(Reflection::StaticType<Empty>().Members.Count, 0);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>().Members.Count, 2);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>().Members.Count, 1);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Inner>().Members.Count, 1);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>().Members.Count, 2);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Members.Count, 2);
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type member names")
    {
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>().Member<0>().Name, "textWithoutAttribute");
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>().Member<1>().Name, "textPtrWithAttribute");
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>().Member<0>().Name, "counter");
        DOCTEST_CHECK_EQ(Reflection::StaticType<Inner>().Member<0>().Name, "value");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Name, "toggle");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>().Member<1>().Name, "inner");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Member<0>().Name, "letterOne");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Member<1>().Name, "letterTwo");
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type member types")
    {
        DOCTEST_CHECK_FALSE(Reflection::StaticType<Base>().Member<0>().IsType<void>());
        DOCTEST_CHECK(Reflection::StaticType<Base>().Member<0>().IsType<std::string>());
        DOCTEST_CHECK(Reflection::StaticType<Base>().Member<1>().IsType<const char*>());
        DOCTEST_CHECK(Reflection::StaticType<Derived>().Member<0>().IsType<int>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedOne>().Member<0>().IsType<bool>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedOne>().Member<1>().IsType<Inner>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedTwo>().Member<0>().IsType<char>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedTwo>().Member<1>().IsType<char>());
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected type member pointers")
    {
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>().Member<0>().Pointer,
            &Base::textWithoutAttribute);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>().Member<1>().Pointer,
            &Base::textPtrWithAttribute);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>().Member<0>().Pointer,
            &Derived::counter);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Inner>().Member<0>().Pointer,
            &Inner::value);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Pointer,
            &BranchedOne::toggle);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>().Member<1>().Pointer,
            &BranchedOne::inner);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Member<0>().Pointer,
            &BranchedTwo::letterOne);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Member<1>().Pointer,
            &BranchedTwo::letterTwo);
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected member attribute count")
    {
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>().Member<0>().Attributes.Count, 0);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>().Member<1>().Attributes.Count, 1);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>().Member<0>().Attributes.Count, 1);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Inner>().Member<0>().Attributes.Count, 1);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Attributes.Count, 2);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>().Member<1>().Attributes.Count, 0);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Member<0>().Attributes.Count, 1);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>().Member<1>().Attributes.Count, 1);
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected member attribute names")
    {
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>()
            .Member<1>().Attribute<0>().Name, "TextAttribute");
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>()
            .Member<0>().Attribute<0>().Name, "CounterAttribute");
        DOCTEST_CHECK_EQ(Reflection::StaticType<Inner>()
            .Member<0>().Attribute<0>().Name, "InnerAttribute");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>()
            .Member<0>().Attribute<0>().Name, "ToggleOnAttribute");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>()
            .Member<0>().Attribute<1>().Name, "ToggleOffAttribute");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>()
            .Member<0>().Attribute<0>().Name, "LetterAttribute");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>()
            .Member<1>().Attribute<0>().Name, "LetterAttribute");
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected member attribute types")
    {
        DOCTEST_CHECK(Reflection::StaticType<Base>()
            .Member<1>().Attribute<0>().IsType<TextAttribute>());
        DOCTEST_CHECK(Reflection::StaticType<Derived>()
            .Member<0>().Attribute<0>().IsType<CounterAttribute>());
        DOCTEST_CHECK(Reflection::StaticType<Inner>()
            .Member<0>().Attribute<0>().IsType<InnerAttribute>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedOne>()
            .Member<0>().Attribute<0>().IsType<ToggleOnAttribute>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedOne>()
            .Member<0>().Attribute<1>().IsType<ToggleOffAttribute>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedTwo>()
            .Member<0>().Attribute<0>().IsType<LetterAttribute>());
        DOCTEST_CHECK(Reflection::StaticType<BranchedTwo>()
            .Member<1>().Attribute<0>().IsType<LetterAttribute>());
    }

    DOCTEST_SUBCASE("DOCTEST_CHECK reflected members attribute instances")
    {
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>()
            .Member<1>().Attribute<0>().Instance, TextAttribute());
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>()
            .Member<0>().Attribute<0>().Instance.state, true);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Inner>()
            .Member<0>().Attribute<0>().Instance.counter, 20);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>()
            .Member<0>().Attribute<0>().Instance.state, true);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedOne>()
            .Member<0>().Attribute<1>().Instance.state, false);
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>()
            .Member<0>().Attribute<0>().Instance.modifier, "Pretty");
        DOCTEST_CHECK_EQ(Reflection::StaticType<BranchedTwo>()
            .Member<1>().Attribute<0>().Instance.modifier, "Ugly");
    }

    DOCTEST_SUBCASE("Enumerate reflected attribtues")
    {
        DOCTEST_SUBCASE("For base type")
        {
            std::vector<std::string_view> expectedAttributes;
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::StaticType<Empty>().Attributes,
                [&presentAttributes](const auto& attribute)
            {
                presentAttributes.push_back(attribute.Name);
            });

            DOCTEST_CHECK_EQ(presentAttributes, expectedAttributes);
        }

        DOCTEST_SUBCASE("For derived type")
        {
            std::vector<std::string_view> expectedAttributes = { "DerivedAttribute" };
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::StaticType<Derived>().Attributes,
                [&presentAttributes](const auto& attribute)
            {
                presentAttributes.push_back(attribute.Name);
            });

            DOCTEST_CHECK_EQ(presentAttributes, expectedAttributes);
        }

        DOCTEST_SUBCASE("For branched type")
        {
            std::vector<std::string_view> expectedAttributes =
                { "BranchedAttributeOne", "BranchedAttributeTwo" };
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::StaticType<BranchedTwo>().Attributes,
                [&presentAttributes](const auto& attribute)
            {
                presentAttributes.push_back(attribute.Name);
            });

            DOCTEST_CHECK_EQ(presentAttributes, expectedAttributes);
        }

        DOCTEST_SUBCASE("For their instances")
        {
            std::vector<std::string_view> expectedAttributes = { "Small", "Big" };
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::StaticType<BranchedTwo>().Attributes,
                [&presentAttributes](const auto& attribute)
            {
                presentAttributes.push_back(attribute.Instance.modifier);
            });

            DOCTEST_CHECK_EQ(presentAttributes, expectedAttributes);
        }
    }

    DOCTEST_SUBCASE("Enumarate reflected members")
    {
        DOCTEST_SUBCASE("For empty type")
        {
            std::vector<std::string_view> expectedMembers;
            std::vector<std::string_view> presentMembers;

            Reflection::ForEach(Reflection::StaticType<Empty>().Members,
                [&presentMembers](const auto& member)
            {
                presentMembers.push_back(member.Name);
            });

            DOCTEST_CHECK_EQ(presentMembers, expectedMembers);
        }

        DOCTEST_SUBCASE("For base type")
        {
            std::vector<std::string_view> expectedMembers =
                { "textWithoutAttribute", "textPtrWithAttribute" };
            std::vector<std::string_view> presentMembers;

            Reflection::ForEach(Reflection::StaticType<Base>().Members,
                [&presentMembers](const auto& member)
            {
                presentMembers.push_back(member.Name);
            });

            DOCTEST_CHECK_EQ(presentMembers, expectedMembers);
        }

        DOCTEST_SUBCASE("For derived type")
        {
            std::vector<std::string_view> expectedMembers = { "toggle", "inner" };
            std::vector<std::string_view> presentMembers;

            Reflection::ForEach(Reflection::StaticType<BranchedOne>().Members,
                [&presentMembers](const auto& member)
            {
                presentMembers.push_back(member.Name);
            });

            DOCTEST_CHECK_EQ(presentMembers, expectedMembers);
        }
    }

    DOCTEST_SUBCASE("Enumerate reflected member attributes")
    {
        DOCTEST_SUBCASE("For base type")
        {
            std::vector<std::string_view> expectedAttributes;
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::StaticType<Base>().Member<0>().Attributes,
                [&presentAttributes](const auto& attribute)
            {
                presentAttributes.push_back(attribute.Name);
            });

            DOCTEST_CHECK_EQ(presentAttributes, expectedAttributes);
        }

        DOCTEST_SUBCASE("For derived type")
        {
            std::vector<std::string_view> expectedAttributes =
                { "ToggleOnAttribute", "ToggleOffAttribute" };
            std::vector<std::string_view> presentAttributes;

            Reflection::ForEach(Reflection::StaticType<BranchedOne>().Member<0>().Attributes,
                [&presentAttributes](const auto& attribute)
            {
                presentAttributes.push_back(attribute.Name);
            });

            DOCTEST_CHECK_EQ(presentAttributes, expectedAttributes);
        }
    }

    /*
    DOCTEST_SUBCASE("Retrieving reflected type by name via constexpr")
    {
        // Find type members and attributes by name.
        // String literal needs to be a static variable passed via template argument to
        // be allowed in constexpr evaluation. This limitation will be lifted in C++20.
        // Disabled for now because of issues with compilation on Clang (GCC/MSVC is fine).
        // Maybe use constexpr hashing or somehow allow to evaluate via argument.
        
        constexpr static char BaseMemberName[] = "textWithoutAttribute";
        constexpr static char DerivedAttributeName[] = "DerivedAttribute";
        constexpr static char DerivedMemberName[] = "counter";
        constexpr static char DerivedMemberAttributeName[] = "CounterAttribute";
        
        DOCTEST_CHECK_EQ(Reflection::StaticType<Base>()
            .FindMember<BaseMemberName>().Name, "textWithoutAttribute");
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>()
            .FindAttribute<DerivedAttributeName>().Name, "DerivedAttribute");
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>()
            .FindAttribute<DerivedAttributeName>().Instance.state, false);
        DOCTEST_CHECK_EQ(Reflection::StaticType<Derived>()
            .FindMember<DerivedMemberName>()
            .FindAttribute<DerivedMemberAttributeName>().Instance.state, true);
    }
    */
}
