#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include/Common")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(FILES_DEBUG
    "${INCLUDE_DIR}/Debug/Debug.hpp"
    "${SOURCE_DIR}/Debug/Debug.cpp"
)

set(FILES_PROFILING
    "${INCLUDE_DIR}/Profiling/Profiling.hpp"
)

set(FILES_UTILITY
    "${INCLUDE_DIR}/Utility/Utility.hpp"
    "${INCLUDE_DIR}/Utility/NonCopyable.hpp"
    "${INCLUDE_DIR}/Utility/Resettable.hpp"
    "${INCLUDE_DIR}/Utility/ScopeGuard.hpp"
    "${INCLUDE_DIR}/Utility/Delegate.hpp"
    "${INCLUDE_DIR}/Utility/Result.hpp"
    "${INCLUDE_DIR}/Utility/Name.hpp"
    "${INCLUDE_DIR}/Utility/NameRegistry.hpp"
    "${SOURCE_DIR}/Utility/Utility.cpp"
    "${SOURCE_DIR}/Utility/NameRegistry.cpp"
)

set(FILES_STRUCTURES
    "${INCLUDE_DIR}/Structures/LinkedList.hpp"
    "${INCLUDE_DIR}/Structures/StateMachine.hpp"
    "${INCLUDE_DIR}/Structures/Handle.hpp"
    "${INCLUDE_DIR}/Structures/HandleMap.hpp"
)

set(FILES_LOGGER
    "${INCLUDE_DIR}/Logger/Logger.hpp"
    "${INCLUDE_DIR}/Logger/LoggerSeverity.hpp"
    "${INCLUDE_DIR}/Logger/LoggerMessage.hpp"
    "${INCLUDE_DIR}/Logger/LoggerSink.hpp"
    "${INCLUDE_DIR}/Logger/LoggerFormat.hpp"
    "${INCLUDE_DIR}/Logger/LoggerOutput.hpp"
    "${INCLUDE_DIR}/Logger/LoggerHistory.hpp"
    "${SOURCE_DIR}/Logger/Logger.cpp"
    "${SOURCE_DIR}/Logger/LoggerSeverity.cpp"
    "${SOURCE_DIR}/Logger/LoggerSink.cpp"
    "${SOURCE_DIR}/Logger/LoggerFormat.cpp"
    "${SOURCE_DIR}/Logger/LoggerOutput.cpp"
    "${SOURCE_DIR}/Logger/LoggerHistory.cpp"
)

set(FILES_EVENTS
    "${INCLUDE_DIR}/Events/EventCollector.hpp"
    "${INCLUDE_DIR}/Events/EventDispatcher.hpp"
    "${INCLUDE_DIR}/Events/EventReceiver.hpp"
    "${INCLUDE_DIR}/Events/EventPolicies.hpp"
    "${INCLUDE_DIR}/Events/EventBase.hpp"
    "${INCLUDE_DIR}/Events/EventQueue.hpp"
    "${INCLUDE_DIR}/Events/EventBroker.hpp"
)

set(FILES_TESTING
    "${INCLUDE_DIR}/Testing/InstanceCounter.hpp"
)

set(FILES_COMMON
    "${INCLUDE_DIR}/Defines.hpp"
    "${INCLUDE_DIR}/Common.hpp"
)

source_group("Debug" FILES ${FILES_DEBUG})
source_group("Profiling" FILES ${FILES_PROFILING})
source_group("Utility" FILES ${FILES_UTILITY})
source_group("Structures" FILES ${FILES_STRUCTURES})
source_group("Logger" FILES ${FILES_LOGGER})
source_group("Events" FILES ${FILES_EVENTS})
source_group("Testing" FILES ${FILES_TESTING})
source_group("" FILES ${FILES_COMMON})

#
# Library
#

add_library(Common
    ${FILES_DEBUG}
    ${FILES_PROFILING}
    ${FILES_UTILITY}
    ${FILES_STRUCTURES}
    ${FILES_LOGGER}
    ${FILES_EVENTS}
    ${FILES_TESTING}
    ${FILES_COMMON}
)

set_property(TARGET Common PROPERTY FOLDER "Engine")
target_compile_features(Common PUBLIC cxx_std_17)
target_include_directories(Common PUBLIC
    "${PROJECT_SOURCE_DIR}/Include"
    "${PROJECT_SOURCE_DIR}/Source"
)

#
# Precompiled Header
#

target_sources(Common PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
target_precompile_headers(Common PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "${SOURCE_DIR}/Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Reflection" "Engine/Reflection")
target_link_libraries(Common PUBLIC Reflection)
enable_reflection(Common ${INCLUDE_DIR} ${SOURCE_DIR})

#
# External
#

target_include_directories(Common PUBLIC "../../External/utfcpp/source")

add_subdirectory("../../External/fmt" "External/fmt")
target_include_directories(Common PUBLIC "../../External/fmt/include")
target_link_libraries(Common PUBLIC fmt::fmt)
set_property(TARGET "fmt" PROPERTY PROJECT_LABEL "fmt")
set_property(TARGET "fmt" PROPERTY FOLDER "External")
