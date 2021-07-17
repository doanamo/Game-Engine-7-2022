#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include/Common")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(FILES_DEBUG
    "${INCLUDE_DIR}/Debug.hpp"
    "${SOURCE_DIR}/Debug.cpp"
)

set(FILES_UTILITY
    "${INCLUDE_DIR}/Utility.hpp"
    "${INCLUDE_DIR}/NonCopyable.hpp"
    "${INCLUDE_DIR}/Resettable.hpp"
    "${INCLUDE_DIR}/ScopeGuard.hpp"
    "${INCLUDE_DIR}/Delegate.hpp"
    "${INCLUDE_DIR}/Result.hpp"
    "${INCLUDE_DIR}/Name.hpp"
    "${INCLUDE_DIR}/NameRegistry.hpp"
    "${SOURCE_DIR}/Utility.cpp"
    "${SOURCE_DIR}/NameRegistry.cpp"
)

set(FILES_STRUCTURES
    "${INCLUDE_DIR}/LinkedList.hpp"
    "${INCLUDE_DIR}/StateMachine.hpp"
    "${INCLUDE_DIR}/Handle.hpp"
    "${INCLUDE_DIR}/HandleMap.hpp"
)

set(FILES_LOGGER
    "${INCLUDE_DIR}/Logger/Logger.hpp"
    "${INCLUDE_DIR}/Logger/LoggerMessage.hpp"
    "${INCLUDE_DIR}/Logger/LoggerSink.hpp"
    "${INCLUDE_DIR}/Logger/LoggerFormat.hpp"
    "${INCLUDE_DIR}/Logger/LoggerOutput.hpp"
    "${INCLUDE_DIR}/Logger/LoggerHistory.hpp"
    "${SOURCE_DIR}/Logger/Logger.cpp"
    "${SOURCE_DIR}/Logger/LoggerMessage.cpp"
    "${SOURCE_DIR}/Logger/LoggerSink.cpp"
    "${SOURCE_DIR}/Logger/LoggerFormat.cpp"
    "${SOURCE_DIR}/Logger/LoggerOutput.cpp"
    "${SOURCE_DIR}/Logger/LoggerHistory.cpp"
)

set(FILES_EVENT
    "${INCLUDE_DIR}/Event/EventCollector.hpp"
    "${INCLUDE_DIR}/Event/EventDispatcher.hpp"
    "${INCLUDE_DIR}/Event/EventReceiver.hpp"
    "${INCLUDE_DIR}/Event/EventPolicies.hpp"
    "${INCLUDE_DIR}/Event/EventBase.hpp"
    "${INCLUDE_DIR}/Event/EventQueue.hpp"
    "${INCLUDE_DIR}/Event/EventBroker.hpp"
)

set(FILES_TEST
    "${INCLUDE_DIR}/Test/InstanceCounter.hpp"
)

source_group("Debug" FILES ${FILES_DEBUG})
source_group("Utility" FILES ${FILES_UTILITY})
source_group("Structures" FILES ${FILES_STRUCTURES})
source_group("Logger" FILES ${FILES_LOGGER})
source_group("Event" FILES ${FILES_EVENT})
source_group("Test" FILES ${FILES_TEST})

#
# Library
#

add_library(Common
    ${FILES_DEBUG}
    ${FILES_UTILITY}
    ${FILES_STRUCTURES}
    ${FILES_LOGGER}
    ${FILES_EVENT}
    ${FILES_TEST}
)

set_property(TARGET Common PROPERTY FOLDER "Engine")
target_compile_features(Common PUBLIC cxx_std_17)
target_include_directories(Common PUBLIC
    "${PROJECT_SOURCE_DIR}/Include"
    "${PROJECT_SOURCE_DIR}/Source"
)

target_sources(Common PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
target_precompile_headers(Common PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "${SOURCE_DIR}/Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Reflection" "Reflection")
target_link_libraries(Common PUBLIC Reflection)
enable_reflection(Common ${INCLUDE_DIR} ${SOURCE_DIR})

#
# External
#

add_subdirectory("../../External/fmt" "External/fmt")
target_include_directories(Common PUBLIC "../../External/fmt/include")
target_link_libraries(Common PUBLIC fmt::fmt)
set_property(TARGET "fmt" PROPERTY PROJECT_LABEL "fmt")
set_property(TARGET "fmt" PROPERTY FOLDER "External")
