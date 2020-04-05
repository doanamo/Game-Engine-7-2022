#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Library
#

project(Renderer)
add_library(Renderer STATIC
    "../../Include/Renderer/StateRenderer.hpp"
    "Precompiled.hpp"
    "StateRenderer.cpp"
)

target_compile_features(Renderer PUBLIC cxx_std_17)
target_include_directories(Renderer PUBLIC "../../Include")
target_precompile_headers(Renderer PRIVATE "Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Common" "Common" EXCLUDE_FROM_ALL)
target_link_libraries(Renderer PRIVATE Common)

add_subdirectory("../System" "System" EXCLUDE_FROM_ALL)
target_link_libraries(Renderer PRIVATE System)

add_subdirectory("../Graphics" "Graphics" EXCLUDE_FROM_ALL)
target_link_libraries(Renderer PRIVATE Graphics)
