/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Scene/SceneInterface.hpp"
using namespace Scene;

SceneDrawParams::SceneDrawParams() :
    cameraName("Camera"),
    viewportRect(0.0f, 0.0f, 0.0f, 0.0f),
    timeAlpha(1.0f)
{
}

glm::ivec2 SceneDrawParams::CalculateViewportSize() const
{
    glm::ivec2 output;
    output.x = viewportRect.z - viewportRect.x;
    output.y = viewportRect.w - viewportRect.y;

    ASSERT(output.x >= 0, "Viewport width is invalid!");
    ASSERT(output.y >= 0, "Viewport height is invalid!");

    return output;
}
