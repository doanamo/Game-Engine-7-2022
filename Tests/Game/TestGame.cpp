/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <Reflection/Reflection.hpp>
#include <gtest/gtest.h>

int main(int argc, char* argv[])
{
    Reflection::Initialize();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
