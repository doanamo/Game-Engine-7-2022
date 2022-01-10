/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <Common/Utility/ScopeGuard.hpp>

/*
    Scope Guard
*/

class ScopeGuard : public testing::Test
{
protected:
    void TearDown() override
    {
        EXPECT_TRUE(ptr == nullptr);
    }

protected:
    int* ptr = nullptr;
};

TEST_F(ScopeGuard, UsingLambda)
{
    ptr = new int(4);
    auto cleanup = Common::MakeScopeGuard([&]()
    {
        delete ptr;
        ptr = nullptr;
    });
}

TEST_F(ScopeGuard, UsingMacro)
{
    ptr = new int(4);
    SCOPE_GUARD([this]
    {
        delete ptr;
        ptr = nullptr;
    });
}
