/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Game/Component.hpp"

/*
    Transform Component

    Interpolated transform that represents position, rotation and scale in the world.
*/

namespace Game
{
    class TransformComponent final : public Component
    {
    public:
        TransformComponent();
        ~TransformComponent();

        void ResetInterpolation();
        glm::mat4 CalculateMatrix(float timeAlpha = 1.0f) const;

        void SetPosition(const glm::vec3& position)
        {
            m_currentPosition = position;
        }

        void SetRotation(const glm::quat& rotation)
        {
            m_currentRotation = rotation;
        }

        void SetScale(const glm::vec3& scale)
        {
            m_currentScale = scale;
        }

        const glm::vec3& GetPosition() const
        {
            return m_currentPosition;
        }

        const glm::quat& GetRotation() const
        {
            return m_currentRotation;
        }

        const glm::vec3& GetScale() const
        {
            return m_currentScale;
        }

    private:
        glm::quat m_currentRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
        glm::quat m_previousRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
        glm::vec3 m_currentPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 m_previousPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 m_currentScale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 m_previousScale = glm::vec3(1.0f, 1.0f, 1.0f);
    };
}
