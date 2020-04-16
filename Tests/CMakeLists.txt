#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Tests
#

project(Tests)
enable_testing()
add_subdirectory(Utility)
add_subdirectory(ScopeGuard)
add_subdirectory(Result)
add_subdirectory(Reflection)
add_subdirectory(StateMachine)
add_subdirectory(Event)
