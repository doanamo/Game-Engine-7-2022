#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

project(Common)
add_library(Common STATIC
    "../../Include/Common/Common.hpp"
    "../../Include/Common/Utility.hpp"
    "../../Include/Common/Debug.hpp"
    "../../Include/Common/NonCopyable.hpp"
    "../../Include/Common/ScopeGuard.hpp"
    "../../Include/Common/LinkedList.hpp"
    "../../Include/Common/StateMachine.hpp"
    "Precompiled.hpp"
    "Utility.cpp"
    "Debug.cpp"
)

target_compile_features(Common PUBLIC cxx_std_17)
target_include_directories(Common PUBLIC "../../Include")
target_precompile_headers(Common PRIVATE "Precompiled.hpp")

#
# Dependencies
#

if(NOT TARGET Logger)
    add_subdirectory("../Logger" "Logger" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(Common PUBLIC Logger)

if(NOT TARGET Event)
    add_subdirectory("../Event" "Event" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(Common PUBLIC Event)

if(NOT TARGET Reflection)
    add_subdirectory("../Reflection" "Reflection" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(Common PUBLIC Reflection)

#
# Externals
#

target_include_directories(Common PUBLIC "../../External/utfcpp-2.3.6/source")
target_include_directories(Common PUBLIC "../../External/GLM-0.9.9.3")
