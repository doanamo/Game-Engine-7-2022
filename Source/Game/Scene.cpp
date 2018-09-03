#include "Precompiled.hpp"
#include "Game/Scene.hpp"
using namespace Game;

SceneDrawParams::SceneDrawParams() :
    cameraName("DefaultCamera"),
    viewportRect(0.0f, 0.0f, 0.0f, 0.0f),
    timeAlpha(1.0f)
{
}

glm::ivec2 SceneDrawParams::GetViewportSize() const
{
    glm::ivec2 output;
    output.x = viewportRect.z - viewportRect.x;
    output.y = viewportRect.w - viewportRect.y;

    ASSERT(output.x >= 0, "Viewport width is invalid!");
    ASSERT(output.y >= 0, "Viewport height is invalid!");

    return output;
}
