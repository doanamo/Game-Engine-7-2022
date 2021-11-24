#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include/Platform")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(FILES_FILESYSTEM
    "${INCLUDE_DIR}/FileSystem/FileSystem.hpp"
    "${INCLUDE_DIR}/FileSystem/FileHandle.hpp"
    "${INCLUDE_DIR}/FileSystem/FileDepot.hpp"
    "${INCLUDE_DIR}/FileSystem/NativeFileHandle.hpp"
    "${INCLUDE_DIR}/FileSystem/NativeFileDepot.hpp"
    "${INCLUDE_DIR}/FileSystem/MemoryFileHandle.hpp"
    "${INCLUDE_DIR}/FileSystem/MemoryFileDepot.hpp"
    "${INCLUDE_DIR}/FileSystem/ArchiveFileHandle.hpp"
    "${INCLUDE_DIR}/FileSystem/ArchiveFileDepot.hpp"
    "${SOURCE_DIR}/FileSystem/FileSystem.cpp"
    "${SOURCE_DIR}/FileSystem/FileHandle.cpp"
    "${SOURCE_DIR}/FileSystem/NativeFileHandle.cpp"
    "${SOURCE_DIR}/FileSystem/NativeFileDepot.cpp"
    "${SOURCE_DIR}/FileSystem/MemoryFileHandle.cpp"
    "${SOURCE_DIR}/FileSystem/MemoryFileDepot.cpp"
    "${SOURCE_DIR}/FileSystem/ArchiveFileHandle.cpp"
    "${SOURCE_DIR}/FileSystem/ArchiveFileDepot.cpp"
)

set(FILES_INPUTMANAGER
    "${INCLUDE_DIR}/InputDefinitions.hpp"
    "${INCLUDE_DIR}/InputState.hpp"
    "${INCLUDE_DIR}/InputManager.hpp"
    "${SOURCE_DIR}/InputDefinitions.cpp"
    "${SOURCE_DIR}/InputState.cpp"
    "${SOURCE_DIR}/InputManager.cpp"
)

set(FILES_WINDOW
    "${INCLUDE_DIR}/WindowEvents.hpp"
    "${INCLUDE_DIR}/Window.hpp"
    "${INCLUDE_DIR}/WindowSystem.hpp"
    "${SOURCE_DIR}/Window.cpp"
    "${SOURCE_DIR}/WindowSystem.cpp"
)

set(FILES_UTILITY
    "${INCLUDE_DIR}/Timer.hpp"
    "${INCLUDE_DIR}/TimerSystem.hpp"
    "${INCLUDE_DIR}/Image.hpp"
    "${SOURCE_DIR}/Timer.cpp"
    "${SOURCE_DIR}/TimerSystem.cpp"
    "${SOURCE_DIR}/Image.cpp"
)

set(FILES_SYSTEM
    "${INCLUDE_DIR}/Platform.hpp"
    "${SOURCE_DIR}/Platform.cpp"
)

source_group("FileSystem" FILES ${FILES_FILESYSTEM})
source_group("InputManager" FILES ${FILES_INPUTMANAGER})
source_group("Window" FILES ${FILES_WINDOW})
source_group("Utility" FILES ${FILES_UTILITY})
source_group("" FILES ${FILES_SYSTEM})

#
# Library
#

add_library(Platform
    ${FILES_FILESYSTEM}
    ${FILES_INPUTMANAGER}
    ${FILES_WINDOW}
    ${FILES_UTILITY}
    ${FILES_SYSTEM}
)

set_property(TARGET Platform PROPERTY FOLDER "Engine")
target_compile_features(Platform PUBLIC cxx_std_17)
target_include_directories(Platform PUBLIC
    "${PROJECT_SOURCE_DIR}/Include"
    "${PROJECT_SOURCE_DIR}/Source"
)

target_sources(Platform PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
target_precompile_headers(Platform PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "${SOURCE_DIR}/Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Platform PRIVATE Core)

enable_reflection(Platform ${INCLUDE_DIR} ${SOURCE_DIR})
