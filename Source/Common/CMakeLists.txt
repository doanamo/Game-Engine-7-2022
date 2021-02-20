#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_FILES
    "Utility.hpp"
    "Debug.hpp"
    "NonCopyable.hpp"
    "Resettable.hpp"
    "ScopeGuard.hpp"
    "Result.hpp"
    "LinkedList.hpp"
    "StateMachine.hpp"
    "HandleMap.hpp"
    "Logger/Logger.hpp"
    "Logger/Message.hpp"
    "Logger/Sink.hpp"
    "Logger/Format.hpp"
    "Logger/Output.hpp"
    "Logger/History.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Common/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Common/"
    PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "Utility.cpp"
    "Debug.cpp"
    "Logger/Logger.cpp"
    "Logger/Message.cpp"
    "Logger/Sink.cpp"
    "Logger/Format.cpp"
    "Logger/Output.cpp"
    "Logger/History.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Common)
add_library(Common ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Common PUBLIC cxx_std_17)
target_include_directories(Common PUBLIC "../../Include" "../../Source")
target_precompile_headers(Common PRIVATE "Precompiled.hpp")
set_property(TARGET Common PROPERTY FOLDER "Engine")

#
# External
#

add_subdirectory("../../External/fmt" "External/fmt")
target_include_directories(Common PUBLIC "../../External/fmt/include")
target_link_libraries(Common PUBLIC fmt::fmt)
set_property(TARGET "fmt" PROPERTY PROJECT_LABEL "fmt")
set_property(TARGET "fmt" PROPERTY FOLDER "External")

