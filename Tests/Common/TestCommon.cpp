/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <Reflection/Reflection.hpp>

int main(int argc, char* argv[])
{
    Logger::SetMode(Logger::Mode::UnitTests);
    Reflection::Initialize();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
