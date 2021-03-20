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
    "Handle.hpp"
    "HandleMap.hpp"
    "Logger/Logger.hpp"
    "Logger/Message.hpp"
    "Logger/Sink.hpp"
    "Logger/Format.hpp"
    "Logger/Output.hpp"
    "Logger/History.hpp"
    "Event/Delegate.hpp"
    "Event/Collector.hpp"
    "Event/Dispatcher.hpp"
    "Event/Receiver.hpp"
    "Event/Policies.hpp"
    "Event/EventBase.hpp"
    "Event/Queue.hpp"
    "Event/Broker.hpp"
    "Test/InstanceCounter.hpp"
)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Common/")
list(TRANSFORM INCLUDE_FILES PREPEND ${INCLUDE_DIR})
source_group(TREE ${INCLUDE_DIR} PREFIX "Header Files" FILES ${INCLUDE_FILES})

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

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
list(TRANSFORM SOURCE_FILES PREPEND ${SOURCE_DIR})
source_group(TREE ${SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCE_FILES})

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
# Dependencies
#

add_subdirectory("../Reflection" "Reflection")
enable_reflection(Common ${SOURCE_DIR} ${INCLUDE_DIR})

#
# External
#

add_subdirectory("../../External/fmt" "External/fmt")
target_include_directories(Common PUBLIC "../../External/fmt/include")
target_link_libraries(Common PUBLIC fmt::fmt)
set_property(TARGET "fmt" PROPERTY PROJECT_LABEL "fmt")
set_property(TARGET "fmt" PROPERTY FOLDER "External")
