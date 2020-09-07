/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/Precompiled.hpp"
#include "Game/Components/CameraComponent.hpp"
#include "Game/Components/TransformComponent.hpp"
#include "Game/ComponentSystem.hpp"
using namespace Game;

CameraComponent::CameraComponent() = default;
CameraComponent::~CameraComponent() = default;

bool CameraComponent::OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf)
{
    m_transform = componentSystem->Lookup<TransformComponent>(entitySelf);
    if(m_transform == nullptr)
        return false;

    return true;
}

void CameraComponent::SetupOrthogonal(const glm::vec2& viewSize, float nearPlane, float farPlane)
{
    m_projection = ProjectionTypes::Orthogonal;
    m_viewSize = viewSize;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
}

void CameraComponent::SetupPerspective(float fov, float nearPlane, float farPlane)
{
    m_projection = ProjectionTypes::Perspective;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    m_fov = fov;
}

glm::mat4 CameraComponent::CalculateTransform(const glm::ivec2& viewportSize)
{
    glm::mat4 output(1.0f);

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

    output = glm::translate(output, -m_transform->GetPosition());
    return output;
}

TransformComponent* CameraComponent::GetTransformComponent()
{
    return m_transform;
}
