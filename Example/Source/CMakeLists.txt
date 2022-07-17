#
# Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

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

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
source_group(TREE ${SOURCE_DIR} PREFIX "" FILES ${SOURCE_FILES})

#
# Target
#

add_executable(${TARGET_NAME}
    ${SOURCE_FILES}
)

target_sources(${TARGET_NAME} PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
target_precompile_headers(${TARGET_NAME} PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "${SOURCE_DIR}/Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../${ENGINE_DIR}/Source" "Engine")
target_link_libraries(${TARGET_NAME} PRIVATE "Engine")
enable_reflection(${TARGET_NAME} ${SOURCE_DIR})

#
# Environment
#

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

get_filename_component(ABSOLUTE_GAME_DIR "../Deploy" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/GameDir.txt" ${ABSOLUTE_GAME_DIR})
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

if(MSVC)
    set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS
        "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

if(EMSCRIPTEN)
    target_link_options(${TARGET_NAME} PUBLIC
        "SHELL: --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/../Deploy/Data@Data/")
endif()

#
# Packaging
#

install(TARGETS ${TARGET_NAME} DESTINATION .)
install(DIRECTORY "../Deploy/Data/" DESTINATION "Data")
install(DIRECTORY "../${ENGINE_DIR}/Deploy/Data/" DESTINATION "Data")

set(CPACK_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)
