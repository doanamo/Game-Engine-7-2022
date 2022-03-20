#
# Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.21)

#
# Project
#

project(GameEngine)

#
# Compilers
#

# Specify required C++ standard version.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable RTTI and exceptions.
if(MSVC)
    string(REGEX REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_definitions("-D_HAS_EXCEPTIONS=0")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")
endif()

# Global Emscripten flags.
if(EMSCRIPTEN)
    add_link_options("$<$<CONFIG:DEBUG>: -s SAFE_HEAP=1>")
endif()

#
# Configurations
#

# Define custom set of configurations.
set(CMAKE_CONFIGURATION_TYPES "Debug;Develop;Release" CACHE STRING "" FORCE)

if(NOT EMSCRIPTEN)
    # Make Release same as removed RelWithDebInfo configuration.
    set(CMAKE_ASM_FLAGS_RELEASE ${CMAKE_ASM_FLAGS_RELWITHDEBINFO})
    set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELWITHDEBINFO})
    set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    set(CMAKE_RC_FLAGS_RELEASE ${CMAKE_RC_FLAGS_RELWITHDEBINFO})
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE ${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO})
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE ${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO})
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO})
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO})
else()
    # On Emscripten we want Release to be based off MinSizeRel.
    # We need smallest WASM size and we do not need any debugging info.
    set(CMAKE_ASM_FLAGS_RELEASE ${CMAKE_ASM_FLAGS_MINSIZEREL})
    set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_MINSIZEREL})
    set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_MINSIZEREL})
    set(CMAKE_RC_FLAGS_RELEASE ${CMAKE_RC_FLAGS_MINSIZEREL})
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE ${CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL})
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE ${CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL})
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE ${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL})
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE ${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL})
endif()

# Enable link-time optimization for Release configuration.
include(CheckIPOSupported)
check_ipo_supported()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

# Make Develop behave like original Release configuration.
set(CMAKE_ASM_FLAGS_DEVELOP ${CMAKE_ASM_FLAGS_RELEASE})
set(CMAKE_C_FLAGS_DEVELOP ${CMAKE_C_FLAGS_RELEASE})
set(CMAKE_CXX_FLAGS_DEVELOP ${CMAKE_CXX_FLAGS_RELEASE})
set(CMAKE_RC_FLAGS_DEVELOP ${CMAKE_RC_FLAGS_RELEASE})
set(CMAKE_MODULE_LINKER_FLAGS_DEVELOP ${CMAKE_MODULE_LINKER_FLAGS_RELEASE})
set(CMAKE_STATIC_LINKER_FLAGS_DEVELOP ${CMAKE_STATIC_LINKER_FLAGS_RELEASE})
set(CMAKE_SHARED_LINKER_FLAGS_DEVELOP ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
set(CMAKE_EXE_LINKER_FLAGS_DEVELOP ${CMAKE_EXE_LINKER_FLAGS_RELEASE})

# Add global defines for identifying each configuration from code.
add_compile_definitions(
    $<$<CONFIG:Debug>:CMAKE_CONFIG_DEBUG>
    $<$<CONFIG:Develop>:CMAKE_CONFIG_DEVELOP>
    $<$<CONFIG:Release>:CMAKE_CONFIG_RELEASE>
)

#
# Sources
#

add_subdirectory("Source")
add_subdirectory("Example")
add_subdirectory("Tests")

# This is here because it must be called from root CMakeLists file.
# Otherwise tests will fail to be run when CTest is invoked.
enable_testing()
