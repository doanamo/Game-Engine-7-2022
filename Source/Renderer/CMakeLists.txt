#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

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

if(NOT TARGET Common)
    add_subdirectory("../Common" "Common" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(Renderer PRIVATE Common)

if(NOT TARGET System)
    add_subdirectory("../System" "System" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(Renderer PRIVATE System)

if(NOT TARGET Graphics)
    add_subdirectory("../Graphics" "Graphics" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(Renderer PRIVATE Graphics)
