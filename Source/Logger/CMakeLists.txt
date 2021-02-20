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
    "Logger.hpp"
    "Message.hpp"
    "Sink.hpp"
    "Format.hpp"
    "Output.hpp"
    "History.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Logger/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Logger/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "Logger.cpp"
    "Message.cpp"
    "Sink.cpp"
    "Format.cpp"
    "Output.cpp"
    "History.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Logger)
add_library(Logger ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Logger PUBLIC cxx_std_17)
target_include_directories(Logger PUBLIC "../../Include" "../../Source")
target_precompile_headers(Logger PRIVATE "Precompiled.hpp")
set_property(TARGET Logger PROPERTY FOLDER "Engine")

#
# External
#

add_subdirectory("../../External/fmt" "External/fmt")
target_include_directories(Logger PUBLIC "../../External/fmt/include")
target_link_libraries(Logger PUBLIC fmt::fmt)
set_property(TARGET "fmt" PROPERTY PROJECT_LABEL "fmt")
set_property(TARGET "fmt" PROPERTY FOLDER "External")
