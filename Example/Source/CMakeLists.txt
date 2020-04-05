#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(TARGET_NAME Example)

project(${TARGET_NAME})
add_executable(${TARGET_NAME}
    "Main.cpp"
    "Precompiled.hpp"
    "Scenes/GameScene.hpp"
    "Scenes/GameScene.cpp"
)

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

# Enable source folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# Set main build directory as working directory.
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

# Write file pointing at game directory.
get_filename_component(ABSOLUTE_GAME_DIR "../Deploy" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/GameDir.txt" ${ABSOLUTE_GAME_DIR})

#
# Packaging
#

# Install needed data files from both game and engine.
install(TARGETS ${TARGET_NAME} DESTINATION .)
install(DIRECTORY "../Deploy/Data/" DESTINATION "Data")
install(DIRECTORY "${ENGINE_DIR}/Deploy/Data/" DESTINATION "Data")

# Configure packaging command.
set(CPACK_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)
