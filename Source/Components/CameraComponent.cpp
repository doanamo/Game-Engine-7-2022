/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Game/ComponentSystem.hpp"
using namespace Components;

Camera::Camera() :
    m_transform(nullptr),
    m_projection(ProjectionTypes::Perspective),
    m_viewSize(2.0f, 2.0f),
    m_nearPlane(0.1f),
    m_farPlane(1000.0f)
{
}

Camera::~Camera()
{
}

bool Camera::OnInitialize(Game::ComponentSystem* componentSystem, const Game::EntityHandle& entitySelf)
{
    // Retrieve the transform component.
    m_transform = componentSystem->Lookup<Transform>(entitySelf);
    if(m_transform == nullptr) return false;

    return true;
}

void Camera::SetupOrthogonal(const glm::vec2& viewSize, float nearPlane, float farPlane)
{
    m_projection = ProjectionTypes::Orthogonal;
    m_viewSize = viewSize;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
}

void Camera::SetupPerspective(float fov, float nearPlane, float farPlane)
{
    m_projection = ProjectionTypes::Perspective;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    m_fov = fov;
}

glm::mat4 Camera::CalculateTransform(const glm::ivec2& viewportSize)
{
    glm::mat4 output(1.0f);

    // Calculate camera projection component.
    if(m_projection == ProjectionTypes::Orthogonal)
    {
        float viewportAspectRatio = (float)viewportSize.x / viewportSize.y;
        float cameraAspectRatio = (float)m_viewSize.x / m_viewSize.y;

        float widthCorrection = 1.0f;
        float heightCorrection = 1.0f;

        if(viewportAspectRatio > cameraAspectRatio)
        {
            widthCorrection  = viewportAspectRatio / cameraAspectRatio;
        }
        else
        {
            heightCorrection = cameraAspectRatio / viewportAspectRatio;
        }

        output *= glm::ortho(
            m_viewSize.x * -0.5f * widthCorrection,
            m_viewSize.x * 0.5f * widthCorrection,
            m_viewSize.y * -0.5f * heightCorrection,
            m_viewSize.y * 0.5f * heightCorrection,
            m_nearPlane, m_farPlane
        );
    }
    else if(m_projection == ProjectionTypes::Perspective)
    {
        float viewportAspectRatio = (float)viewportSize.x / viewportSize.y;

        output *= glm::perspective(
            m_fov, viewportAspectRatio,
            m_nearPlane, m_farPlane
        );
    }
    else
    {
        ASSERT(false, "Unknown camera projection type!");
    }

    // Calculate camera transform component.
    // #improvement: Calculate camera rotation transform for 3D movement.
    output = glm::translate(output, -m_transform->position);

    return output;
}

Transform* Camera::GetTransformComponent()
{
    return m_transform;
}
