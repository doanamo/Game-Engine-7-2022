#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# Specify minimum CMake version required.
cmake_minimum_required(VERSION 3.11.2)

#
# Source
#

# Files that take part in the build process.
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include")
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/Source")
set(PRECOMPILED_HEADER "Precompiled")

set(INCLUDE_FILES
    "${INCLUDE_DIR}/Engine.hpp"

    "${INCLUDE_DIR}/Common/Build.hpp"
    "${INCLUDE_DIR}/Common/Debug.hpp"
    "${INCLUDE_DIR}/Common/NonCopyable.hpp"
    "${INCLUDE_DIR}/Common/ScopeGuard.hpp"
    "${INCLUDE_DIR}/Common/Utility.hpp"
)

set(SOURCE_FILES
    "${SOURCE_DIR}/${PRECOMPILED_HEADER}.hpp"
    "${SOURCE_DIR}/${PRECOMPILED_HEADER}.cpp"

    "${SOURCE_DIR}/Common/Build.cpp"
    "${SOURCE_DIR}/Common/Debug.cpp"
    "${SOURCE_DIR}/Common/Utility.cpp"
)

# Keep folder structure of include and source files in Visual Studio.
source_group(TREE ${INCLUDE_DIR} PREFIX "Include Files" FILES ${INCLUDE_FILES})
source_group(TREE ${SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Target
#

# Declare a statically linked library target.
add_library("Engine" STATIC ${INCLUDE_FILES} ${SOURCE_FILES})
include_directories("./" ${INCLUDE_DIR})

#
# Platform
#

# Use precompiled header for quicker compilation of static headers.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(PRECOMPILED_BINARY "Precompiled.pch")

    set_source_files_properties(${SOURCE_FILES} PROPERTIES
        COMPILE_FLAGS "/Yu\"${PRECOMPILED_HEADER}.hpp\" /Fp\"${PRECOMPILED_BINARY}\""
        OBJECT_DEPENDS "${PRECOMPILED_BINARY}"
    )

    set_source_files_properties("${PRECOMPILED_HEADER}.cpp" PROPERTIES
        COMPILE_FLAGS "/Yc\"${PRECOMPILED_HEADER}.hpp\" /Fp\"${PRECOMPILED_BINARY}\""
        OBJECT_DEPENDS "${PRECOMPILED_BINARY}"
    )
endif()

#
# Debug
#

# Save build information that will be later read in runtime.
file(WRITE "${CMAKE_BINARY_DIR}/WorkingDir.txt" "${CMAKE_BINARY_DIR}/")
file(WRITE "${CMAKE_BINARY_DIR}/IncludeDir.txt" "${INCLUDE_DIR}/")
file(WRITE "${CMAKE_BINARY_DIR}/SourceDir.txt" "${SOURCE_DIR}/")
