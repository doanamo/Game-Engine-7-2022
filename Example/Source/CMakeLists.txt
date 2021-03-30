#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(SOURCE_FILES
    "Main.cpp"
    "Precompiled.hpp"
    "GameStates/SpriteDemo.hpp"
    "GameStates/SpriteDemo.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Application
#

set(PROJECT_NAME Project)
set(TARGET_NAME Example)

project(${PROJECT_NAME})
add_executable(${TARGET_NAME} ${SOURCE_FILES})
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_17)
target_precompile_headers(${TARGET_NAME} PRIVATE "Precompiled.hpp")

#
# Dependencies
#

set(ENGINE_DIR "../../")
add_subdirectory("${ENGINE_DIR}/Source" "Engine")
target_link_libraries(${TARGET_NAME} PRIVATE "Engine")

#
# Environment
#

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

get_filename_component(ABSOLUTE_GAME_DIR "../Deploy" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/GameDir.txt" ${ABSOLUTE_GAME_DIR})
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

if(EMSCRIPTEN)
    target_link_options(${TARGET_NAME} PUBLIC "SHELL: --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/../Deploy/Data@Data/")
endif()

#
# Packaging
#

install(TARGETS ${TARGET_NAME} DESTINATION .)
install(DIRECTORY "../Deploy/Data/" DESTINATION "Data")
install(DIRECTORY "${ENGINE_DIR}/Deploy/Data/" DESTINATION "Data")

set(CPACK_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)
