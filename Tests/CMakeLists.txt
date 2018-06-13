#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# Specify minimum CMake version required.
cmake_minimum_required(VERSION 3.11.2)

#
# Tests
#

# Define a target for building all tests.
add_custom_target("RUN_TESTS" COMMAND ${CMAKE_CTEST_COMMAND})
set_target_properties("RUN_TESTS" PROPERTIES FOLDER "CMake")

# Utility function for defining tests.
function(create_test testName)
    add_executable("Test${testName}" "Test${testName}.cpp")
    set_target_properties("Test${testName}" PROPERTIES FOLDER "Tests")
    target_link_libraries("Test${testName}" "Engine")
    include_directories("${CMAKE_SOURCE_DIR}/Include/")
    add_test("${testName}" "Test${testName}")
    add_dependencies("RUN_TESTS" "Test${testName}")
endfunction(create_test)

# Scope guard unit tests.
create_test("ScopeGuard")
