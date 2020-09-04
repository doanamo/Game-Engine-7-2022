/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/Precompiled.hpp"
#include "Game/Components/TransformComponent.hpp"
using namespace Game;

TransformComponent::TransformComponent() = default;
TransformComponent::~TransformComponent() = default;

void TransformComponent::ResetInterpolation()
{
    // Update previous transform to match current one.
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

glm::mat4 TransformComponent::CalculateMatrix(float alpha) const
{
    glm::mat4 output(1.0f);
    output = glm::translate(output, glm::lerp(m_previousPosition, m_currentPosition, alpha));
    output = output * glm::mat4_cast(glm::slerp(m_previousRotation, m_currentRotation, alpha));
    output = glm::scale(output, glm::lerp(m_previousScale, m_currentScale, alpha));
    return output;
}
