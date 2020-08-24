#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

# Define source files.
set(IMGUI_FILES
    "imgui.h"
    "imgui.cpp"
    "imgui_internal.h"
    "imgui_widgets.cpp"
    "imgui_draw.cpp"
    "imgui_demo.cpp"
    "imconfig.h"
    "imstb_rectpack.h"
    "imstb_textedit.h"
    "imstb_truetype.h"

    "misc/cpp/imgui_stdlib.h"
    "misc/cpp/imgui_stdlib.cpp"
)

# Create a static library.
add_library("ImGui" STATIC ${IMGUI_FILES})
target_include_directories("ImGui" PUBLIC "./" "./examples/" "./misc/cpp/")
