#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)

#
# Tests
#

project(Tests)
enable_testing()
add_subdirectory(Common)
add_subdirectory(Reflection)
add_subdirectory(StateMachine)
add_subdirectory(IdentitySystem)
