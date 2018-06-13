#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# Specify minimum CMake version required.
cmake_minimum_required(VERSION 3.11.2)

#
# Tests
#

# Utility function for defining tests.
function(create_test testName)
    add_executable("Test${testName}" "Test${testName}.cpp")
    set_target_properties("Test${testName}" PROPERTIES FOLDER "Tests")
    target_link_libraries("Test${testName}" "Engine")
    include_directories("${CMAKE_SOURCE_DIR}/Include/")
    set_target_properties("Test${testName}" PROPERTIES COMPILE_DEFINITIONS TESTS_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")
    add_test("${testName}" "Test${testName}")
endfunction(create_test)

# Create unit tests.
create_test("Utilities")
create_test("ScopeGuard")
