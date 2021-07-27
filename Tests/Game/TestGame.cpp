/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
#include <doctest/doctest.h>
#include <Reflection/Reflection.hpp>

int main(const int argc, char* argv[])
{
    Reflection::Initialize();
    return doctest::Context(argc, argv).run();
}
