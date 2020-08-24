#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# Minimum version required.
cmake_minimum_required(VERSION 3.10)

# Declare the project.
project("Lua" C)

# List of source files.
set(LUA_LIB_SOURCE
    "src/lapi.c" 
    "src/lauxlib.c"
    "src/lbaselib.c"
    "src/lbitlib.c"
    "src/lcode.c"
    "src/lcorolib.c"
    "src/lctype.c"
    "src/ldblib.c"
    "src/ldebug.c"
    "src/ldo.c"
    "src/ldump.c"
    "src/lfunc.c"
    "src/lgc.c"
    "src/linit.c"
    "src/liolib.c"
    "src/llex.c"
    "src/lmathlib.c"
    "src/lmem.c"
    "src/loadlib.c"
    "src/lobject.c"
    "src/lopcodes.c"
    "src/loslib.c"
    "src/lparser.c"
    "src/lstate.c"
    "src/lstring.c"
    "src/lstrlib.c"
    "src/ltable.c"
    "src/ltablib.c"
    "src/ltm.c"
    "src/luac.c"
    "src/lundump.c"
    "src/lutf8lib.c"
    "src/lvm.c"
    "src/lzio.c"
)

# Create a library target.
add_library("lua_static" STATIC ${LUA_LIB_SOURCE})
target_include_directories("lua_static" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/")
