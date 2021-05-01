/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include <Reflection/Reflection.hpp>

int main(const int argc, char* argv[])
{
    Reflection::Initialize();
    return doctest::Context(argc, argv).run();
}
