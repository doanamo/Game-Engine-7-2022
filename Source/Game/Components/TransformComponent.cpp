/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/Components/TransformComponent.hpp"
using namespace Game;

TransformComponent::TransformComponent() :
    position(0.0f, 0.0f, 0.0f),
    rotation(0.0f, 0.0f, 0.0f),
    scale(1.0f, 1.0f, 1.0f)
{
}

TransformComponent::~TransformComponent()
{
}

TransformComponent::TransformComponent(TransformComponent&& other)
{
    *this = std::move(other);
}

TransformComponent& TransformComponent::operator=(TransformComponent&& other)
{
    std::swap(position, other.position);
    std::swap(rotation, other.rotation);
    std::swap(scale, other.scale);

    return *this;
}

glm::mat4 TransformComponent::CalculateMatrix(const glm::mat4& input) const
{
    glm::mat4 output(input);

    output = glm::translate(output, position);
    output = glm::rotate(output, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    output = glm::rotate(output, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    output = glm::rotate(output, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    output = glm::scale(output, scale);
    
    return output;
}