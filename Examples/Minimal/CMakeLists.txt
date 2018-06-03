#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# Specify minimum CMake version required.
cmake_minimum_required(VERSION 3.11.2)

#
# Source
#

# Source files that will partake in the compilation process.
set(SOURCE_FILES
    "Source/Main.cpp"
)

#
# Target
#

# Declare an executable target that depends on the engine library.
add_executable("Minimal" ${SOURCE_FILES})
target_link_libraries("Minimal" "Engine")
include_directories("./../../Include")
