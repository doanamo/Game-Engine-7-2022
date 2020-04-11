#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_FILES
    "Common.hpp"
    "Utility.hpp"
    "Debug.hpp"
    "NonCopyable.hpp"
    "ScopeGuard.hpp"
    "LinkedList.hpp"
    "StateMachine.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Common/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Common/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "Utility.cpp"
    "Debug.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Common)
add_library(Common STATIC ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Common PUBLIC cxx_std_17)
target_include_directories(Common PUBLIC "../../Include")
target_precompile_headers(Common PRIVATE "Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Logger" "Logger")
target_link_libraries(Common PUBLIC Logger)

add_subdirectory("../Event" "Event")
target_link_libraries(Common PUBLIC Event)

add_subdirectory("../Reflection" "Reflection")
target_link_libraries(Common PUBLIC Reflection)

#
# Externals
#

target_include_directories(Common PUBLIC "../../External/utfcpp-2.3.6/source")
target_include_directories(Common PUBLIC "../../External/GLM-0.9.9.3")
