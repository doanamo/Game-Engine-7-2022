#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# Specify minimum CMake version required.
cmake_minimum_required(VERSION 3.11.2)

#
# Examples
#

# Minimum example that can run the engine.
add_subdirectory("Minimal")
set_target_properties("Minimal" PROPERTIES FOLDER "Examples")
