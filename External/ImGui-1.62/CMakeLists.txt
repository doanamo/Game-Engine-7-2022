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
)

# Create a static library.
add_library("ImGui" STATIC ${IMGUI_FILES})
target_include_directories("ImGui" PUBLIC "./" "./examples")
