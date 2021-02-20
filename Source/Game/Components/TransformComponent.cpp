/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Game/Precompiled.hpp"
#include "Game/Components/TransformComponent.hpp"
using namespace Game;

TransformComponent::TransformComponent() = default;
TransformComponent::~TransformComponent() = default;

void TransformComponent::ResetInterpolation()
{
    /*
        Update transform for interpolation in next frame range.
    */

    m_previousPosition = m_currentPosition;
    m_previousRotation = m_currentRotation;
    m_previousScale = m_currentScale;
}

void TransformComponent::SetPosition(const glm::vec3& position)
{
    m_currentPosition = position;
}

void TransformComponent::SetRotation(const glm::quat& rotation)
{
    m_currentRotation = rotation;
}

void TransformComponent::SetScale(const glm::vec3& scale)
{
    m_currentScale = scale;
}

const glm::vec3& TransformComponent::GetPosition() const
{
    return m_currentPosition;
}

const glm::quat& TransformComponent::GetRotation() const
{
    return m_currentRotation;
}

const glm::vec3& TransformComponent::GetScale() const
{
    return m_currentScale;
}

glm::mat4 TransformComponent::CalculateMatrix(float timeAlpha) const
{
    /*
        Calculate interpolated transform based on
        previous/current transforms and time alpha.
    */

    glm::mat4 output(1.0f);
    output = glm::translate(output, glm::lerp(m_previousPosition, m_currentPosition, timeAlpha));
    output = output * glm::mat4_cast(glm::slerp(m_previousRotation, m_currentRotation, timeAlpha));
    output = glm::scale(output, glm::lerp(m_previousScale, m_currentScale, timeAlpha));
    return output;
}
