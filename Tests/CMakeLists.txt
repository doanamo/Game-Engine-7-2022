#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

# Specify minimum CMake version required.
cmake_minimum_required(VERSION 3.12)

#
# Tests
#

# Utility function for defining tests.
function(create_test test_name)
    if(NOT EXISTS "Test${test_name}.cpp")
        configure_file("TestEmpty.cpp" "Test${test_name}.cpp")
    endif()

    add_executable("Test${test_name}" "Test${test_name}.cpp" "TestHelpers.hpp")
    target_include_directories("Test${test_name}" PRIVATE "." "../Include/")
    target_link_libraries("Test${test_name}" "Engine")

    set_target_properties("Test${test_name}" PROPERTIES FOLDER "Tests")
    set_target_properties("Test${test_name}" PROPERTIES COMPILE_DEFINITIONS TESTS_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")

    add_test("${test_name}" "Test${test_name}")
endfunction(create_test)

# Listing of unit tests.
create_test("Utilities")
create_test("ScopeGuard")
create_test("Events")
