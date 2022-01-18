/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "TestReflectionHeader.hpp"

TEST(StaticReflection, BuiltTypes)
{
    EXPECT_TRUE(Reflection::IsReflected<Reflection::NullType>());
    EXPECT_TRUE(Reflection::IsReflected<Reflection::TypeAttribute>());
    EXPECT_TRUE(Reflection::IsReflected<Reflection::FieldAttribute>());
    EXPECT_TRUE(Reflection::IsReflected<Reflection::MethodAttribute>());

    EXPECT_TRUE(Reflection::StaticType<Reflection::NullType>().IsNullType());
    EXPECT_EQ(Reflection::StaticType<Reflection::NullType>().Name, "Reflection::NullType");
    EXPECT_FALSE(Reflection::StaticType<Undefined>().IsNullType());
    EXPECT_FALSE(Reflection::StaticType<Derived>().IsNullType());
}

TEST(StaticReflection, RegularTypes)
{
    EXPECT_FALSE(Reflection::IsReflected<Undefined>());
    EXPECT_FALSE(Reflection::IsReflected<CrossUnit>());
    EXPECT_TRUE(Reflection::IsReflected<Empty>());
    EXPECT_TRUE(Reflection::IsReflected<Base>());
    EXPECT_TRUE(Reflection::IsReflected<Derived>());
    EXPECT_TRUE(Reflection::IsReflected<Inner>());
    EXPECT_TRUE(Reflection::IsReflected<BranchedOne>());
    EXPECT_TRUE(Reflection::IsReflected<BranchedTwo>());
}

TEST(StaticReflection, ConstTypes)
{
    EXPECT_FALSE(Reflection::IsReflected<const Undefined>());
    EXPECT_FALSE(Reflection::IsReflected<const CrossUnit>());
    EXPECT_TRUE(Reflection::IsReflected<const Empty>());
    EXPECT_TRUE(Reflection::IsReflected<const Base>());

    volatile Derived derived;
    EXPECT_TRUE(Reflection::IsReflected(derived));

    const BranchedOne& branchedOne = BranchedOne();
    EXPECT_TRUE(Reflection::IsReflected(branchedOne));

    const BranchedTwo* branchedTwo = nullptr;
    EXPECT_TRUE(Reflection::IsReflected(branchedTwo));

    EXPECT_EQ(Reflection::GetIdentifier<const Empty&>(), Reflection::GetIdentifier<Empty>());
    EXPECT_EQ(Reflection::GetIdentifier<volatile Empty*>(), Reflection::GetIdentifier<Empty>());
}

TEST(StaticReflection, TypeIdentifiers)
{
    EXPECT_EQ(Reflection::StaticType<Empty>().Identifier, Reflection::GetIdentifier("Empty"));
    EXPECT_EQ(Reflection::StaticType<Base>().Identifier, Reflection::GetIdentifier("Base"));
    EXPECT_EQ(Reflection::StaticType<Derived>().Identifier, Reflection::GetIdentifier("Derived"));
    EXPECT_EQ(Reflection::StaticType<Inner>().Identifier, Reflection::GetIdentifier("Inner"));
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Identifier, Reflection::GetIdentifier("BranchedOne"));
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Identifier, Reflection::GetIdentifier("BranchedTwo"));
    EXPECT_NE(Reflection::StaticType<BranchedOne>().Identifier, Reflection::StaticType<BranchedTwo>().Identifier);
}

TEST(StaticReflection, TypeNames)
{
    EXPECT_EQ(Reflection::StaticType<Empty>().Name, "Empty");
    EXPECT_EQ(Reflection::StaticType<Base>().Name, "Base");
    EXPECT_EQ(Reflection::StaticType<Derived>().Name, "Derived");
    EXPECT_EQ(Reflection::StaticType<Inner>().Name, "Inner");
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Name, "BranchedOne");
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Name, "BranchedTwo");
}

TEST(StaticReflection, NamespacedTypeName)
{
    using namespace Reflection;
    EXPECT_EQ(Reflection::StaticType<NullType>().Name, "Reflection::NullType");
}

TEST(StaticReflection, NamesFromIdentifier)
{
    EXPECT_EQ(Reflection::GetName<Empty>(), Reflection::GetIdentifier<Empty>());
    EXPECT_EQ(Reflection::GetName<Base>(), Reflection::GetIdentifier<Base>());
    EXPECT_EQ(Reflection::GetName<Derived>(), Reflection::GetIdentifier<Derived>());
    EXPECT_EQ(Reflection::GetName<Inner>(), Reflection::GetIdentifier<Inner>());
    EXPECT_EQ(Reflection::GetName<BranchedOne>(), Reflection::GetIdentifier<BranchedOne>());
    EXPECT_EQ(Reflection::GetName<BranchedTwo>(), Reflection::GetIdentifier<BranchedTwo>());
}

TEST(StaticReflection, TypeFromValue)
{
    EXPECT_FALSE(Reflection::StaticType(Undefined()).IsType<Empty>());
    EXPECT_TRUE(Reflection::StaticType(Undefined()).IsType<Undefined>());
    EXPECT_TRUE(Reflection::StaticType(Empty()).IsType<Empty>());
    EXPECT_TRUE(Reflection::StaticType(Base()).IsType<Base>());
    EXPECT_TRUE(Reflection::StaticType(Derived()).IsType<Derived>());
    EXPECT_TRUE(Reflection::StaticType(Inner()).IsType<Inner>());
    EXPECT_TRUE(Reflection::StaticType(BranchedOne()).IsType<BranchedOne>());
    EXPECT_TRUE(Reflection::StaticType(BranchedTwo()).IsType<BranchedTwo>());
}

TEST(StaticReflection, BaseTypes)
{
    EXPECT_FALSE(Reflection::StaticType<Undefined>().HasBaseType());
    EXPECT_FALSE(Reflection::StaticType<Empty>().HasBaseType());
    EXPECT_FALSE(Reflection::StaticType<Base>().HasBaseType());
    EXPECT_TRUE(Reflection::StaticType<Derived>().HasBaseType());
    EXPECT_FALSE(Reflection::StaticType<Inner>().HasBaseType());
    EXPECT_TRUE(Reflection::StaticType<BranchedOne>().HasBaseType());
    EXPECT_TRUE(Reflection::StaticType<BranchedTwo>().HasBaseType());

    EXPECT_EQ(Reflection::StaticType<Derived>().GetBaseType().Name, "Base");
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().GetBaseType().Name, "Derived");
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().GetBaseType().Name, "Derived");
    EXPECT_TRUE(Reflection::StaticType<Derived>().GetBaseType().IsType<Base>());
    EXPECT_TRUE(Reflection::StaticType<BranchedOne>().GetBaseType().IsType<Derived>());
    EXPECT_TRUE(Reflection::StaticType<BranchedTwo>().GetBaseType().IsType<Derived>());
    EXPECT_TRUE(Reflection::StaticType<Derived>().IsDerivedFrom<Base>());
    EXPECT_TRUE(Reflection::StaticType<BranchedOne>().IsDerivedFrom<Derived>());
    EXPECT_TRUE(Reflection::StaticType<BranchedTwo>().IsDerivedFrom<Derived>());
    EXPECT_TRUE(Reflection::StaticType<Base>().IsBaseOf<Derived>());
    EXPECT_TRUE(Reflection::StaticType<Derived>().IsBaseOf<BranchedOne>());
    EXPECT_TRUE(Reflection::StaticType<Derived>().IsBaseOf<BranchedOne>());
}

TEST(StaticReflection, Attributes)
{
    EXPECT_FALSE(Reflection::StaticType<Empty>().HasAttributes());
    EXPECT_TRUE(Reflection::StaticType<Base>().HasAttributes());
    EXPECT_TRUE(Reflection::StaticType<Derived>().HasAttributes());
    EXPECT_FALSE(Reflection::StaticType<Inner>().HasAttributes());
    EXPECT_FALSE(Reflection::StaticType<BranchedOne>().HasAttributes());
    EXPECT_TRUE(Reflection::StaticType<BranchedTwo>().HasAttributes());
}

TEST(StaticReflection, AttributeCount)
{
    EXPECT_EQ(Reflection::StaticType<Empty>().Attributes.Count, 0);
    EXPECT_EQ(Reflection::StaticType<Base>().Attributes.Count, 1);
    EXPECT_EQ(Reflection::StaticType<Derived>().Attributes.Count, 1);
    EXPECT_EQ(Reflection::StaticType<Inner>().Attributes.Count, 0);
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Attributes.Count, 0);
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Attributes.Count, 2);
}

TEST(StaticReflection, AttributeNames)
{
    EXPECT_EQ(Reflection::StaticType<Base>().Attribute<0>().Name, "BaseAttribute");
    EXPECT_EQ(Reflection::StaticType<Derived>().Attribute<0>().Name, "DerivedAttribute");
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Attribute<0>().Name, "BranchedAttributeOne");
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Attribute<1>().Name, "BranchedAttributeTwo");
}

TEST(StaticReflection, AttributeTypes)
{
    EXPECT_FALSE(Reflection::StaticType<Base>().Attribute<0>().IsType<DerivedAttribute>());
    EXPECT_TRUE(Reflection::StaticType<Base>().Attribute<0>().IsType<BaseAttribute>());
    EXPECT_TRUE(Reflection::StaticType<Derived>().Attribute<0>().IsType<DerivedAttribute>());
    EXPECT_TRUE(Reflection::StaticType<BranchedTwo>().Attribute<0>().IsType<BranchedAttributeOne>());
    EXPECT_TRUE(Reflection::StaticType<BranchedTwo>().Attribute<1>().IsType<BranchedAttributeTwo>());
}

TEST(StaticReflection, AttributeInstances)
{
    EXPECT_EQ(Reflection::StaticType<Base>().Attribute<0>().Instance, BaseAttribute());
    EXPECT_EQ(Reflection::StaticType<Derived>().Attribute<0>().Instance.state, false);
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Attribute<0>().Instance.modifier, "Small");
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Attribute<1>().Instance.modifier, "Big");
}

TEST(StaticReflection, MemberCount)
{
    EXPECT_EQ(Reflection::StaticType<Empty>().Members.Count, 0);
    EXPECT_EQ(Reflection::StaticType<Base>().Members.Count, 2);
    EXPECT_EQ(Reflection::StaticType<Derived>().Members.Count, 1);
    EXPECT_EQ(Reflection::StaticType<Inner>().Members.Count, 1);
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Members.Count, 2);
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Members.Count, 2);
}

TEST(StaticReflection, MemberNames)
{
    EXPECT_EQ(Reflection::StaticType<Base>().Member<0>().Name, "textWithoutAttribute");
    EXPECT_EQ(Reflection::StaticType<Base>().Member<1>().Name, "textPtrWithAttribute");
    EXPECT_EQ(Reflection::StaticType<Derived>().Member<0>().Name, "counter");
    EXPECT_EQ(Reflection::StaticType<Inner>().Member<0>().Name, "value");
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Name, "toggle");
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Member<1>().Name, "inner");
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Member<0>().Name, "letterOne");
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Member<1>().Name, "letterTwo");
}

TEST(StaticReflection, MemberTypes)
{
    EXPECT_FALSE(Reflection::StaticType<Base>().Member<0>().IsType<void>());
    EXPECT_TRUE(Reflection::StaticType<Base>().Member<0>().IsType<std::string>());
    EXPECT_TRUE(Reflection::StaticType<Base>().Member<1>().IsType<const char*>());
    EXPECT_TRUE(Reflection::StaticType<Derived>().Member<0>().IsType<int>());
    EXPECT_TRUE(Reflection::StaticType<BranchedOne>().Member<0>().IsType<bool>());
    EXPECT_TRUE(Reflection::StaticType<BranchedOne>().Member<1>().IsType<Inner>());
    EXPECT_TRUE(Reflection::StaticType<BranchedTwo>().Member<0>().IsType<char>());
    EXPECT_TRUE(Reflection::StaticType<BranchedTwo>().Member<1>().IsType<char>());
}

TEST(StaticReflection, MemberPointers)
{
    EXPECT_EQ(Reflection::StaticType<Base>().Member<0>().Pointer, &Base::textWithoutAttribute);
    EXPECT_EQ(Reflection::StaticType<Base>().Member<1>().Pointer, &Base::textPtrWithAttribute);
    EXPECT_EQ(Reflection::StaticType<Derived>().Member<0>().Pointer, &Derived::counter);
    EXPECT_EQ(Reflection::StaticType<Inner>().Member<0>().Pointer, &Inner::value);
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Pointer, &BranchedOne::toggle);
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Member<1>().Pointer, &BranchedOne::inner);
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Member<0>().Pointer, &BranchedTwo::letterOne);
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Member<1>().Pointer, &BranchedTwo::letterTwo);
}

TEST(StaticReflection, MemberAttributeCount)
{
    EXPECT_EQ(Reflection::StaticType<Base>().Member<0>().Attributes.Count, 0);
    EXPECT_EQ(Reflection::StaticType<Base>().Member<1>().Attributes.Count, 1);
    EXPECT_EQ(Reflection::StaticType<Derived>().Member<0>().Attributes.Count, 1);
    EXPECT_EQ(Reflection::StaticType<Inner>().Member<0>().Attributes.Count, 1);
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Attributes.Count, 2);
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Member<1>().Attributes.Count, 0);
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Member<0>().Attributes.Count, 1);
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Member<1>().Attributes.Count, 1);
}

TEST(StaticReflection, MemberAttributeNames)
{
    EXPECT_EQ(Reflection::StaticType<Base>().Member<1>().Attribute<0>().Name, "TextAttribute");
    EXPECT_EQ(Reflection::StaticType<Derived>().Member<0>().Attribute<0>().Name, "CounterAttribute");
    EXPECT_EQ(Reflection::StaticType<Inner>().Member<0>().Attribute<0>().Name, "InnerAttribute");
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Attribute<0>().Name, "ToggleOnAttribute");
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Attribute<1>().Name, "ToggleOffAttribute");
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Member<0>().Attribute<0>().Name, "LetterAttribute");
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Member<1>().Attribute<0>().Name, "LetterAttribute");
}

TEST(StaticReflection, MemberAttributeTypes)
{
    EXPECT_TRUE(Reflection::StaticType<Base>().Member<1>().Attribute<0>().IsType<TextAttribute>());
    EXPECT_TRUE(Reflection::StaticType<Derived>().Member<0>().Attribute<0>().IsType<CounterAttribute>());
    EXPECT_TRUE(Reflection::StaticType<Inner>().Member<0>().Attribute<0>().IsType<InnerAttribute>());
    EXPECT_TRUE(Reflection::StaticType<BranchedOne>().Member<0>().Attribute<0>().IsType<ToggleOnAttribute>());
    EXPECT_TRUE(Reflection::StaticType<BranchedOne>().Member<0>().Attribute<1>().IsType<ToggleOffAttribute>());
    EXPECT_TRUE(Reflection::StaticType<BranchedTwo>().Member<0>().Attribute<0>().IsType<LetterAttribute>());
    EXPECT_TRUE(Reflection::StaticType<BranchedTwo>().Member<1>().Attribute<0>().IsType<LetterAttribute>());
}

TEST(StaticReflection, MemberAttributeInstances)
{
    EXPECT_EQ(Reflection::StaticType<Base>().Member<1>().Attribute<0>().Instance, TextAttribute());
    EXPECT_EQ(Reflection::StaticType<Derived>().Member<0>().Attribute<0>().Instance.state, true);
    EXPECT_EQ(Reflection::StaticType<Inner>().Member<0>().Attribute<0>().Instance.counter, 20);
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Attribute<0>().Instance.state, true);
    EXPECT_EQ(Reflection::StaticType<BranchedOne>().Member<0>().Attribute<1>().Instance.state, false);
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Member<0>().Attribute<0>().Instance.modifier, "Pretty");
    EXPECT_EQ(Reflection::StaticType<BranchedTwo>().Member<1>().Attribute<0>().Instance.modifier, "Ugly");
}

TEST(StaticReflection, EnumerateAttributesBase)
{
    std::vector<std::string_view> expectedAttributes;
    std::vector<std::string_view> presentAttributes;

    Reflection::ForEach(Reflection::StaticType<Empty>().Attributes,
        [&presentAttributes](const auto& attribute)
    {
        presentAttributes.push_back(attribute.Name);
    });

    EXPECT_EQ(presentAttributes, expectedAttributes);
}

TEST(StaticReflection, EnumerateAttributesDerived)
{
    std::vector<std::string_view> expectedAttributes = { "DerivedAttribute" };
    std::vector<std::string_view> presentAttributes;

    Reflection::ForEach(Reflection::StaticType<Derived>().Attributes,
        [&presentAttributes](const auto& attribute)
    {
        presentAttributes.push_back(attribute.Name);
    });

    EXPECT_EQ(presentAttributes, expectedAttributes);
}

TEST(StaticReflection, EnumerateAttributesBranched)
{
    std::vector<std::string_view> expectedAttributes = { "BranchedAttributeOne", "BranchedAttributeTwo" };
    std::vector<std::string_view> presentAttributes;

    Reflection::ForEach(Reflection::StaticType<BranchedTwo>().Attributes,
        [&presentAttributes](const auto& attribute)
    {
        presentAttributes.push_back(attribute.Name);
    });

    EXPECT_EQ(presentAttributes, expectedAttributes);
}

TEST(StaticReflection, EnumerateAttributesInstances)
{
    std::vector<std::string_view> expectedAttributes = { "Small", "Big" };
    std::vector<std::string_view> presentAttributes;

    Reflection::ForEach(Reflection::StaticType<BranchedTwo>().Attributes,
        [&presentAttributes](const auto& attribute)
    {
        presentAttributes.push_back(attribute.Instance.modifier);
    });

    EXPECT_EQ(presentAttributes, expectedAttributes);
}

TEST(StaticReflection, EnumerateMembersEmpty)
{
    std::vector<std::string_view> expectedMembers;
    std::vector<std::string_view> presentMembers;

    Reflection::ForEach(Reflection::StaticType<Empty>().Members,
        [&presentMembers](const auto& member)
    {
        presentMembers.push_back(member.Name);
    });

    EXPECT_EQ(presentMembers, expectedMembers);
}

TEST(StaticReflection, EnumerateMembersBase)
{
    std::vector<std::string_view> expectedMembers = { "textWithoutAttribute", "textPtrWithAttribute" };
    std::vector<std::string_view> presentMembers;

    Reflection::ForEach(Reflection::StaticType<Base>().Members,
        [&presentMembers](const auto& member)
    {
        presentMembers.push_back(member.Name);
    });

    EXPECT_EQ(presentMembers, expectedMembers);
}

TEST(StaticReflection, EnumerateMembersDerived)
{
    std::vector<std::string_view> expectedMembers = { "toggle", "inner" };
    std::vector<std::string_view> presentMembers;

    Reflection::ForEach(Reflection::StaticType<BranchedOne>().Members,
        [&presentMembers](const auto& member)
    {
        presentMembers.push_back(member.Name);
    });

    EXPECT_EQ(presentMembers, expectedMembers);
}

TEST(StaticReflection, EnumerateMemberAttributesBase)
{
    std::vector<std::string_view> expectedAttributes;
    std::vector<std::string_view> presentAttributes;

    Reflection::ForEach(Reflection::StaticType<Base>().Member<0>().Attributes,
        [&presentAttributes](const auto& attribute)
    {
        presentAttributes.push_back(attribute.Name);
    });

    EXPECT_EQ(presentAttributes, expectedAttributes);
}

TEST(StaticReflection, EnumerateMemberAttributesDerived)
{
    std::vector<std::string_view> expectedAttributes = { "ToggleOnAttribute", "ToggleOffAttribute" };
    std::vector<std::string_view> presentAttributes;

    Reflection::ForEach(Reflection::StaticType<BranchedOne>().Member<0>().Attributes,
        [&presentAttributes](const auto& attribute)
    {
        presentAttributes.push_back(attribute.Name);
    });

    EXPECT_EQ(presentAttributes, expectedAttributes);
}

/*
TEST(StaticReflection, EnumerateViaConstexprString)
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

    EXPECT_EQ(Reflection::StaticType<Base>()
        .FindMember<BaseMemberName>().Name, "textWithoutAttribute");
    EXPECT_EQ(Reflection::StaticType<Derived>()
        .FindAttribute<DerivedAttributeName>().Name, "DerivedAttribute");
    EXPECT_EQ(Reflection::StaticType<Derived>()
        .FindAttribute<DerivedAttributeName>().Instance.state, false);
    EXPECT_EQ(Reflection::StaticType<Derived>()
        .FindMember<DerivedMemberName>()
        .FindAttribute<DerivedMemberAttributeName>().Instance.state, true);
}
*/
