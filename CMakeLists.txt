#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)

#
# Project
#

project(GameEngine)
add_subdirectory("Source")
add_subdirectory("Example")
add_subdirectory("Tests")
enable_testing()
