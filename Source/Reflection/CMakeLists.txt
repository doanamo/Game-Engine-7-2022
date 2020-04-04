#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

add_library(Reflection STATIC
    "../../Include/Reflection/Reflection.hpp"
    "Empty.cpp"
)

target_compile_features(Reflection PUBLIC cxx_std_17)
target_include_directories(Reflection PUBLIC "../../Include")
target_include_directories(Reflection PRIVATE "../../Include/Reflection")
