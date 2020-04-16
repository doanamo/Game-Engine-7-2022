/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/Component.hpp"

/*
    Transform Component
*/

namespace Game
{
    class TransformComponent final : public Component
    {
    public:
        TransformComponent();
        ~TransformComponent();

        // Called at the beginning of each frame.
        // Should be called after teleportation.
        void ResetInterpolation();

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::quat& rotation);
        void SetScale(const glm::vec3& scale);

        const glm::vec3& GetPosition() const;
        const glm::quat& GetRotation() const;
        const glm::vec3& GetScale() const;

        // Alpha value of 0.0f returns a matrix representing the previous transform.
        // Alpha value of 1.0f returns a matrix representing the current transform.
        glm::mat4 CalculateMatrix(float alpha = 1.0f) const;

    private:
        glm::vec3 m_currentPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::quat m_currentRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
        glm::vec3 m_currentScale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 m_previousPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::quat m_previousRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
        glm::vec3 m_previousScale = glm::vec3(1.0f, 1.0f, 1.0f);
    };
}
