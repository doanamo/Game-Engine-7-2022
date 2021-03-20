/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Reflection

    Based on implementation and articles from:
    - https://github.com/veselink1/refl-cpp
    - https://medium.com/@vesko.karaganev/compile-time-reflection-in-c-17-55c14ee8106b
    - https://veselink1.github.io/blog/cpp/metaprogramming/2019/07/13/refl-cpp-deep-dive.html
*/

#include "Reflection/ReflectionDetail.hpp"
#include "Reflection/ReflectionStatic.hpp"
#include "Reflection/ReflectionDynamic.hpp"
#include "Reflection/ReflectionRegistry.hpp"
#include "Reflection/ReflectionDeclare.hpp"
#include "Reflection/ReflectionUtility.hpp"
#include "Reflection/ReflectionTypes.hpp"
