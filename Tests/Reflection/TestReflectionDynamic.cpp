/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "TestReflectionHeader.hpp"

TEST(DynamicReflection, BuiltTypes)
{
    Reflection::TypeIdentifier nullTypeIdentifier = Reflection::GetIdentifier<Reflection::NullType>();

    EXPECT_TRUE(Reflection::IsRegistered(nullTypeIdentifier));
    EXPECT_TRUE(Reflection::DynamicType(nullTypeIdentifier).IsRegistered());
    EXPECT_TRUE(Reflection::DynamicType(nullTypeIdentifier).IsNullType());
    EXPECT_EQ(Reflection::DynamicType(nullTypeIdentifier).GetName(), Common::Name("Reflection::NullType"));
    EXPECT_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Undefined>()).IsNullType());
    EXPECT_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Derived>()).IsNullType());
}

TEST(DynamicReflection, BaseTypes)
{
    EXPECT_FALSE(Reflection::IsRegistered(Reflection::GetIdentifier<Undefined>()));
    EXPECT_FALSE(Reflection::IsRegistered(Reflection::GetIdentifier<CrossUnit>()));
    EXPECT_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Undefined>()).IsRegistered());
    EXPECT_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<CrossUnit>()).IsRegistered());
    EXPECT_TRUE(Reflection::DynamicType<Empty>().IsRegistered());
    EXPECT_TRUE(Reflection::DynamicType<Base>().IsRegistered());
    EXPECT_TRUE(Reflection::DynamicType(Reflection::GetIdentifier<Derived>()).IsRegistered());
    EXPECT_TRUE(Reflection::DynamicType(Reflection::GetIdentifier<Inner>()).IsRegistered());
    EXPECT_TRUE(Reflection::DynamicType(Reflection::GetIdentifier<BranchedOne>()).IsRegistered());
    EXPECT_TRUE(Reflection::DynamicType(Reflection::GetIdentifier<BranchedTwo>()).IsRegistered());
}

TEST(DynamicReflection, TypeNames)
{
    EXPECT_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Empty>()).GetName(), Common::Name("Empty"));
    EXPECT_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Base>()).GetName(), Common::Name("Base"));
    EXPECT_EQ(Reflection::GetName(Derived()), NAME("Derived"));

    Inner inner;
    EXPECT_EQ(Reflection::GetName(&inner), NAME("Inner"));

    std::unique_ptr<Derived> branchedOne = std::make_unique<BranchedOne>();
    EXPECT_EQ(Reflection::GetName(branchedOne), NAME("BranchedOne"));

    Reflection::TypeIdentifier branchedTwo = Reflection::GetIdentifier<BranchedTwo>();
    EXPECT_EQ(Reflection::GetName(branchedTwo), NAME("BranchedTwo"));
}

TEST(DynamicReflection, TypeIdentifiers)
{
    EXPECT_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Empty>()).GetIdentifier(), Reflection::GetIdentifier<Empty>());
    EXPECT_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Base>()).GetIdentifier(), Reflection::GetIdentifier<Base>());
    EXPECT_EQ(Reflection::DynamicType(Reflection::GetIdentifier<Derived>()).GetIdentifier(), Reflection::GetIdentifier<Derived>());

    Inner inner;
    EXPECT_EQ(Reflection::DynamicType(inner).GetIdentifier(), Reflection::GetIdentifier<Inner>());

    BranchedOne branchedOne;
    EXPECT_EQ(Reflection::DynamicType(&branchedOne).GetIdentifier(), Reflection::GetIdentifier<BranchedOne>());

    std::unique_ptr<BranchedTwo> branchedTwo = std::make_unique<BranchedTwo>();
    EXPECT_EQ(Reflection::DynamicType(branchedTwo).GetIdentifier(), Reflection::GetIdentifier<BranchedTwo>());
}

TEST(DynamicReflection, TypeFromValue)
{
    EXPECT_TRUE(Reflection::IsRegistered(Reflection::NullType()));
    EXPECT_TRUE(Reflection::DynamicType(Empty()).IsType<Empty>());
    EXPECT_TRUE(Reflection::DynamicType(Base()).IsType<Base>());
    EXPECT_TRUE(Reflection::DynamicType(Derived()).IsType<Derived>());
    EXPECT_TRUE(Reflection::DynamicType(Inner()).IsType<Inner>());
    EXPECT_TRUE(Reflection::DynamicType(BranchedOne()).IsType<BranchedOne>());
    EXPECT_TRUE(Reflection::DynamicType(BranchedTwo()).IsType<BranchedTwo>());
}

TEST(DynamicReflection, BaseType)
{
    EXPECT_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Undefined>()).HasBaseType());
    EXPECT_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Empty>()).HasBaseType());
    EXPECT_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Base>()).HasBaseType());
    EXPECT_TRUE(Reflection::DynamicType(Reflection::GetIdentifier<Derived>()).HasBaseType());
    EXPECT_FALSE(Reflection::DynamicType(Reflection::GetIdentifier<Inner>()).HasBaseType());
    EXPECT_TRUE(Reflection::DynamicType(Reflection::GetIdentifier<BranchedOne>()).HasBaseType());
    EXPECT_TRUE(Reflection::DynamicType(Reflection::GetIdentifier<BranchedTwo>()).HasBaseType());

    EXPECT_EQ(Reflection::DynamicType<Derived>().GetBaseType().GetName(), Common::Name("Base"));
    EXPECT_EQ(Reflection::DynamicType<BranchedOne>().GetBaseType().GetName(), Common::Name("Derived"));
    EXPECT_EQ(Reflection::DynamicType<BranchedTwo>().GetBaseType().GetName(), Common::Name("Derived"));
    EXPECT_TRUE(Reflection::DynamicType<Derived>().GetBaseType().IsType<Base>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedOne>().GetBaseType().IsType<Derived>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedTwo>().GetBaseType().IsType<Derived>());

    EXPECT_TRUE(Reflection::DynamicType<Derived>().IsDerivedFrom<Base>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Derived>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedTwo>().IsDerivedFrom<Derived>());

    EXPECT_TRUE(Reflection::DynamicType<Base>().IsBaseOf<Derived>());
    EXPECT_TRUE(Reflection::DynamicType<Derived>().IsBaseOf<BranchedOne>());
    EXPECT_TRUE(Reflection::DynamicType<Derived>().IsBaseOf<BranchedOne>());
}

TEST(DynamicReflection, Attributes)
{
    EXPECT_FALSE(Reflection::DynamicType<Empty>().HasAttributes());
    EXPECT_TRUE(Reflection::DynamicType<Base>().HasAttributes());
    EXPECT_TRUE(Reflection::DynamicType<Derived>().HasAttributes());
    EXPECT_FALSE(Reflection::DynamicType<Inner>().HasAttributes());
    EXPECT_FALSE(Reflection::DynamicType<BranchedOne>().HasAttributes());
    EXPECT_TRUE(Reflection::DynamicType<BranchedTwo>().HasAttributes());

    EXPECT_TRUE(Reflection::DynamicType<Base>().HasAttribute<BasicAttribute>());
    EXPECT_TRUE(Reflection::DynamicType<Base>().HasAttribute<CommonAttribute>());
    EXPECT_TRUE(Reflection::DynamicType<Derived>().HasAttribute<DerivedAttribute>());
    EXPECT_FALSE(Reflection::DynamicType<BranchedOne>().HasAttribute<BranchedAttributeOne>());
    EXPECT_FALSE(Reflection::DynamicType<BranchedOne>().HasAttribute<BranchedAttributeTwo>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedTwo>().HasAttribute<BranchedAttributeOne>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedTwo>().HasAttribute<BranchedAttributeTwo>());
}

TEST(DynamicReflection, AttributeCount)
{
    EXPECT_EQ(Reflection::DynamicType<Empty>().GetAttributes().size(), 0);
    EXPECT_EQ(Reflection::DynamicType<Base>().GetAttributes().size(), 2);
    EXPECT_EQ(Reflection::DynamicType<Derived>().GetAttributes().size(), 1);
    EXPECT_EQ(Reflection::DynamicType<Inner>().GetAttributes().size(), 0);
    EXPECT_EQ(Reflection::DynamicType<BranchedOne>().GetAttributes().size(), 0);
    EXPECT_EQ(Reflection::DynamicType<BranchedTwo>().GetAttributes().size(), 2);
}

TEST(DynamicReflection, AttributeNames)
{
    EXPECT_EQ(Reflection::GetName(Reflection::DynamicType<Base>().GetAttributeByIndex(0)), "BasicAttribute");
    EXPECT_EQ(Reflection::GetName(Reflection::DynamicType<Base>().GetAttributeByIndex(1)), "CommonAttribute");
    EXPECT_EQ(Reflection::GetName(Reflection::DynamicType<Derived>().GetAttributeByIndex(0)), "DerivedAttribute");
    EXPECT_EQ(Reflection::GetName(Reflection::DynamicType<BranchedTwo>().GetAttributeByIndex(0)), "BranchedAttributeOne");
    EXPECT_EQ(Reflection::GetName(Reflection::DynamicType<BranchedTwo>().GetAttributeByIndex(1)), "BranchedAttributeTwo");
}

TEST(DynamicReflection, AttributeTypes)
{
    EXPECT_FALSE(Reflection::IsType<DerivedAttribute>(Reflection::DynamicType<Base>().GetAttributeByIndex(0)));
    EXPECT_TRUE(Reflection::IsType<BasicAttribute>(Reflection::DynamicType<Base>().GetAttributeByIndex(0)));
    EXPECT_TRUE(Reflection::IsType<CommonAttribute>(Reflection::DynamicType<Base>().GetAttributeByIndex(1)));
    EXPECT_TRUE(Reflection::IsType<DerivedAttribute>(Reflection::DynamicType<Derived>().GetAttributeByIndex(0)));
    EXPECT_TRUE(Reflection::IsType<BranchedAttributeOne>(Reflection::DynamicType<BranchedTwo>().GetAttributeByIndex(0)));
    EXPECT_TRUE(Reflection::IsType<BranchedAttributeTwo>(Reflection::DynamicType<BranchedTwo>().GetAttributeByIndex(1)));
}

TEST(DynamicReflection, AttributeInstances)
{
    EXPECT_EQ(*Reflection::DynamicType<Base>().GetAttribute<BasicAttribute>(), BasicAttribute());
    EXPECT_EQ(*Reflection::DynamicType<Base>().GetAttribute<CommonAttribute>(), CommonAttribute());
    EXPECT_EQ(Reflection::DynamicType<Derived>().GetAttribute<DerivedAttribute>()->state, false);
    EXPECT_EQ(Reflection::DynamicType<BranchedTwo>().GetAttribute<BranchedAttributeOne>()->modifier, "Small");
    EXPECT_EQ(Reflection::DynamicType<BranchedTwo>().GetAttribute<BranchedAttributeTwo>()->modifier, "Big");
}

TEST(DynamicReflection, NullTypeHierarchy)
{
    // Even though we use null type to deduce lack of base type, we do not want it to act as such.
    EXPECT_EQ(Reflection::DynamicType<Base>().GetBaseType().GetName(), Common::Name("Reflection::NullType"));
    EXPECT_FALSE(Reflection::DynamicType<Base>().IsDerivedFrom<Reflection::NullType>());
    EXPECT_FALSE(Reflection::DynamicType<Reflection::NullType>().IsBaseOf<Base>());
}

TEST(DynamicReflection, SuperDeclaration)
{
    EXPECT_EQ(Reflection::DynamicType<Derived>().GetBaseType().GetIdentifier(),Reflection::DynamicType<Derived::Super>().GetIdentifier());

    EXPECT_TRUE(Reflection::DynamicType<BranchedOne>().GetBaseType().IsType<BranchedOne::Super>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedTwo>().GetBaseType().IsType<BranchedTwo::Super>());
}

TEST(DynamicReflection, PolymorphicType)
{
    EXPECT_TRUE(Reflection::DynamicType<BranchedOne>().IsType<BranchedOne>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedTwo>().IsType<BranchedTwo>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedOne>().IsType<Derived>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedTwo>().IsType<Derived>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedOne>().IsType<Base>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedTwo>().IsType<Base>());

    EXPECT_FALSE(Reflection::DynamicType<BranchedOne>().IsType<Reflection::NullType>());
    EXPECT_FALSE(Reflection::DynamicType<BranchedOne>().IsType<BranchedTwo>());
    EXPECT_FALSE(Reflection::DynamicType<BranchedTwo>().IsType<BranchedOne>());
    EXPECT_FALSE(Reflection::DynamicType<BranchedOne>().IsType<Inner>());
    EXPECT_FALSE(Reflection::DynamicType<BranchedTwo>().IsType<Inner>());

    EXPECT_TRUE(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Derived>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedTwo>().IsDerivedFrom<Derived>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Base>());
    EXPECT_TRUE(Reflection::DynamicType<BranchedTwo>().IsDerivedFrom<Base>());

    EXPECT_FALSE(Reflection::DynamicType<Base>().IsDerivedFrom<Reflection::NullType>());
    EXPECT_FALSE(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<Reflection::NullType>());
    EXPECT_FALSE(Reflection::DynamicType<BranchedOne>().IsDerivedFrom<BranchedOne>());
    EXPECT_FALSE(Reflection::DynamicType<Derived>().IsDerivedFrom<BranchedOne>());
    EXPECT_FALSE(Reflection::DynamicType<Base>().IsDerivedFrom<BranchedOne>());
    EXPECT_FALSE(Reflection::DynamicType<Inner>().IsDerivedFrom<BranchedOne>());

    EXPECT_TRUE(Reflection::DynamicType<Derived>().IsBaseOf<BranchedOne>());
    EXPECT_TRUE(Reflection::DynamicType<Derived>().IsBaseOf<BranchedTwo>());
    EXPECT_TRUE(Reflection::DynamicType<Base>().IsBaseOf<BranchedOne>());
    EXPECT_TRUE(Reflection::DynamicType<Base>().IsBaseOf<BranchedTwo>());

    EXPECT_FALSE(Reflection::DynamicType<Derived>().IsBaseOf<Reflection::NullType>());
    EXPECT_FALSE(Reflection::DynamicType<Derived>().IsBaseOf<Derived>());
    EXPECT_FALSE(Reflection::DynamicType<Inner>().IsBaseOf<Derived>());
    EXPECT_FALSE(Reflection::DynamicType<Derived>().IsBaseOf<Inner>());
}

TEST(DynamicReflection, PolymorphicInstance)
{
    BranchedOne branchedOne;
    Derived& branchedOneDerived = branchedOne;
    Base& branchedOneBase = branchedOne;

    EXPECT_TRUE(branchedOne.GetTypeInfo().IsType<BranchedOne>());
    EXPECT_TRUE(branchedOne.GetTypeInfo().IsType<Derived>());
    EXPECT_TRUE(branchedOne.GetTypeInfo().IsType<Base>());
    EXPECT_TRUE(branchedOneDerived.GetTypeInfo().IsType<BranchedOne>());
    EXPECT_TRUE(branchedOneDerived.GetTypeInfo().IsType<Base>());
    EXPECT_TRUE(branchedOneBase.GetTypeInfo().IsType<BranchedOne>());
    EXPECT_TRUE(branchedOneBase.GetTypeInfo().IsType<Derived>());

    EXPECT_TRUE(branchedOne.GetTypeInfo().IsType(branchedOne));
    EXPECT_TRUE(branchedOne.GetTypeInfo().IsType(branchedOneDerived));
    EXPECT_TRUE(branchedOne.GetTypeInfo().IsType(branchedOneBase));
    EXPECT_TRUE(branchedOneDerived.GetTypeInfo().IsType(branchedOne));
    EXPECT_TRUE(branchedOneDerived.GetTypeInfo().IsType(branchedOneBase));
    EXPECT_TRUE(branchedOneBase.GetTypeInfo().IsType(branchedOne));
    EXPECT_TRUE(branchedOneBase.GetTypeInfo().IsType(branchedOneDerived));

    BranchedTwo branchedTwo;
    Derived& branchedTwoDerived = branchedTwo;
    Base& branchedTwoBase = branchedTwo;

    EXPECT_FALSE(branchedTwo.GetTypeInfo().IsType(branchedOne));
    EXPECT_FALSE(branchedTwo.GetTypeInfo().IsType(branchedOneDerived));
    EXPECT_FALSE(branchedTwo.GetTypeInfo().IsType(branchedOneBase));
    EXPECT_FALSE(branchedTwoDerived.GetTypeInfo().IsType(branchedOne));
    EXPECT_FALSE(branchedTwoDerived.GetTypeInfo().IsType(branchedOneDerived));
    EXPECT_FALSE(branchedTwoDerived.GetTypeInfo().IsType(branchedOneBase));
    EXPECT_FALSE(branchedTwoBase.GetTypeInfo().IsType(branchedOne));
    EXPECT_FALSE(branchedTwoBase.GetTypeInfo().IsType(branchedOneDerived));
    EXPECT_FALSE(branchedTwoBase.GetTypeInfo().IsType(branchedOneBase));
}

TEST(DynamicReflection, TypeCasting)
{
    BranchedOne branchedOne;
    branchedOne.inner.value = 42;

    BranchedOne* branchedOnePtr = Reflection::Cast<BranchedOne>(&branchedOne);
    EXPECT_NE(branchedOnePtr, nullptr);
    EXPECT_EQ(branchedOnePtr->inner.value, 42);
    EXPECT_TRUE(branchedOnePtr->GetTypeInfo().IsType<BranchedOne>());
    EXPECT_TRUE(branchedOnePtr->GetTypeInfo().IsType<Derived>());
    EXPECT_TRUE(branchedOnePtr->GetTypeInfo().IsType<Base>());

    Derived* derivedPtr = Reflection::Cast<Derived>(branchedOnePtr);
    EXPECT_NE(derivedPtr, nullptr);
    EXPECT_TRUE(derivedPtr->GetTypeInfo().IsType<BranchedOne>());
    EXPECT_TRUE(derivedPtr->GetTypeInfo().IsType<Derived>());
    EXPECT_TRUE(derivedPtr->GetTypeInfo().IsType<Base>());

    Base* basePtr = Reflection::Cast<Base>(branchedOnePtr);
    EXPECT_NE(basePtr, nullptr);
    EXPECT_TRUE(basePtr->GetTypeInfo().IsType<BranchedOne>());
    EXPECT_TRUE(basePtr->GetTypeInfo().IsType<Derived>());
    EXPECT_TRUE(basePtr->GetTypeInfo().IsType<Base>());

    derivedPtr = Reflection::Cast<Derived>(basePtr);
    EXPECT_NE(derivedPtr, nullptr);
    EXPECT_TRUE(derivedPtr->GetTypeInfo().IsType<BranchedOne>());
    EXPECT_TRUE(derivedPtr->GetTypeInfo().IsType<Derived>());
    EXPECT_TRUE(derivedPtr->GetTypeInfo().IsType<Base>());

    branchedOnePtr = Reflection::Cast<BranchedOne>(derivedPtr);
    EXPECT_NE(branchedOnePtr, nullptr);
    EXPECT_EQ(branchedOnePtr->inner.value, 42);
    EXPECT_TRUE(branchedOnePtr->GetTypeInfo().IsType<BranchedOne>());
    EXPECT_TRUE(branchedOnePtr->GetTypeInfo().IsType<Derived>());
    EXPECT_TRUE(branchedOnePtr->GetTypeInfo().IsType<Base>());

    branchedOnePtr = Reflection::Cast<BranchedOne>(basePtr);
    EXPECT_NE(branchedOnePtr, nullptr);
    EXPECT_EQ(branchedOnePtr->inner.value, 42);
    EXPECT_TRUE(branchedOnePtr->GetTypeInfo().IsType<BranchedOne>());
    EXPECT_TRUE(branchedOnePtr->GetTypeInfo().IsType<Derived>());
    EXPECT_TRUE(branchedOnePtr->GetTypeInfo().IsType<Base>());

    BranchedTwo* branchedTwoPtr = Reflection::Cast<BranchedTwo>(branchedOnePtr);
    EXPECT_EQ(branchedTwoPtr, nullptr);

    branchedTwoPtr = Reflection::Cast<BranchedTwo>(derivedPtr);
    EXPECT_EQ(branchedTwoPtr, nullptr);

    branchedTwoPtr = Reflection::Cast<BranchedTwo>(basePtr);
    EXPECT_EQ(branchedTwoPtr, nullptr);
}

TEST(DynamicReflection, ConstructFromIdentifier)
{
    EXPECT_TRUE(Reflection::StaticType<Derived>().IsConstructible());
    std::unique_ptr<Base> derived(Reflection::Construct<Derived>());
    EXPECT_TRUE(derived->GetTypeInfo().IsType<Derived>());

    EXPECT_TRUE(Reflection::DynamicType<BranchedOne>().IsConstructible());
    std::unique_ptr<Base> branchedOne(Reflection::Construct<Base>(Reflection::GetIdentifier<BranchedOne>()));
    EXPECT_TRUE(branchedOne->GetTypeInfo().IsType<BranchedOne>());

    EXPECT_TRUE(Reflection::DynamicType(Reflection::GetIdentifier<BranchedTwo>()).IsConstructible());
    std::unique_ptr<Base> branchedTwo(Reflection::Construct<Base>(Reflection::GetIdentifier<BranchedTwo>()));
    EXPECT_TRUE(branchedTwo->GetTypeInfo().IsType<BranchedTwo>());

    EXPECT_FALSE(Reflection::StaticType<Reflection::BaseAttribute>().IsConstructible());
    auto* baseAttribute = Reflection::Construct<Reflection::BaseAttribute>();
    EXPECT_EQ(baseAttribute, nullptr);

    EXPECT_FALSE(Reflection::StaticType<Reflection::GenericAttribute>().IsConstructible());
    auto* genericAttribute = Reflection::Construct<Reflection::GenericAttribute>();
    EXPECT_EQ(genericAttribute, nullptr);

    EXPECT_FALSE(Reflection::StaticType<Reflection::TypeAttribute>().IsConstructible());
    auto* typeAttribute = Reflection::Construct<Reflection::TypeAttribute>();
    EXPECT_EQ(typeAttribute, nullptr);

    EXPECT_FALSE(Reflection::StaticType<Reflection::MethodAttribute>().IsConstructible());
    auto* methodAttribute = Reflection::Construct<Reflection::MethodAttribute>();
    EXPECT_EQ(methodAttribute, nullptr);

    EXPECT_FALSE(Reflection::StaticType<Reflection::FieldAttribute>().IsConstructible());
    auto* fieldAttribute = Reflection::Construct<Reflection::FieldAttribute>();
    EXPECT_EQ(fieldAttribute, nullptr);

    std::unique_ptr<Base> base;
    base = Reflection::Cast<Derived>(derived);
    EXPECT_TRUE(base->GetTypeInfo().IsType<Derived>());
    base = Reflection::Cast<Base>(branchedOne);
    EXPECT_TRUE(base->GetTypeInfo().IsType<BranchedOne>());
    base = Reflection::Cast<Base>(branchedTwo);
    EXPECT_TRUE(base->GetTypeInfo().IsType<BranchedTwo>());
}
