/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/Components/TransformComponent.hpp"
using namespace Game;

TransformComponent::TransformComponent() :
    m_currentPosition(0.0f, 0.0f, 0.0f),
    m_currentRotation(1.0f, 0.0f, 0.0f, 0.0f),
    m_currentScale(1.0f, 1.0f, 1.0f),
    m_previousPosition(0.0f, 0.0f, 0.0f),
    m_previousRotation(1.0f, 0.0f, 0.0f, 0.0f),
    m_previousScale(1.0f, 1.0f, 1.0f)
{
}

TransformComponent::~TransformComponent()
{
}

TransformComponent::TransformComponent(TransformComponent&& other) :
    TransformComponent()
{
    *this = std::move(other);
}

TransformComponent& TransformComponent::operator=(TransformComponent&& other)
{
    std::swap(m_currentPosition, other.m_currentPosition);
    std::swap(m_currentRotation, other.m_currentRotation);
    std::swap(m_currentScale, other.m_currentScale);

    std::swap(m_previousPosition, other.m_previousPosition);
    std::swap(m_previousRotation, other.m_previousRotation);
    std::swap(m_previousScale, other.m_previousScale);

    return *this;
}

void TransformComponent::ResetInterpolation()
{
    // Update the previous transform to match the current one.
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

glm::mat4 TransformComponent::CalculateMatrix(const glm::mat4& input, float alpha) const
{
    glm::mat4 output(input);

    output = glm::translate(output, glm::mix(m_previousPosition, m_currentPosition, alpha));
    output = output * glm::mat4_cast(glm::lerp(m_previousRotation, m_currentRotation, alpha));
    output = glm::scale(output, glm::mix(m_previousScale, m_currentScale, alpha));
    
    return output;
}
