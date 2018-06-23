#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# Define source files.
set(IMGUI_FILES
    "imgui.h"
    "imgui.cpp"
    "imgui_internal.h"
    "imgui_draw.cpp"
    "imgui_demo.cpp"
    "imconfig.h"
    "stb_rect_pack.h"
    "stb_textedit.h"
    "stb_truetype.h"
    
    "examples/imgui_impl_glfw.h"
    "examples/imgui_impl_glfw.cpp"
    "examples/imgui_impl_opengl3.h"
    "examples/imgui_impl_opengl3.cpp"
)

# Create a static library.
add_library("ImGUI" STATIC ${IMGUI_FILES})
target_include_directories("ImGUI" PUBLIC "./" "./examples")
