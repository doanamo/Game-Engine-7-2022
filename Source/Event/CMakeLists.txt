#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_FILES
    "Delegate.hpp"
    "Collector.hpp"
    "Dispatcher.hpp"
    "Receiver.hpp"
    "Policies.hpp"
    "Queue.hpp"
    "Broker.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Event/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Event/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Empty.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Event)
add_library(Event ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Event PUBLIC cxx_std_17)
target_include_directories(Event PUBLIC "../../Include")
