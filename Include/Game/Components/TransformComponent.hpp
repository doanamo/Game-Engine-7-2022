/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/Component.hpp"

/*
    Transform Component
*/

namespace Game
{
    // Transform component.
    class TransformComponent : public Component
    {
    public:
        TransformComponent();
        ~TransformComponent();

        TransformComponent(TransformComponent&& other);
        TransformComponent& operator=(TransformComponent&& other);

        // Resets the transform interpolation.
        // Called at the beginning of each frame.
        // Should be called after e.g. teleporting an object.
        void ResetInterpolation();

        // Sets the current position.
        void SetPosition(const glm::vec3& position);

        // Set the current rotation.
        void SetRotation(const glm::quat& rotation);

        // Sets the current scale.
        void SetScale(const glm::vec3& scale);

        // Gets the current position.
        const glm::vec3& GetPosition() const;

        // Gets the current rotation.
        const glm::quat& GetRotation() const;

        // Gets the current scale.
        const glm::vec3& GetScale() const;

        // Calculates an interpolated transform matrix.
        // Alpha value of 0.0f returns a matrix representing the previous transform.
        // Alpha value of 1.0f returns a matrix representing the current transform.
        glm::mat4 CalculateMatrix(float alpha = 1.0f) const;

    public:
        // Transform parameters.
        glm::vec3 m_currentPosition;
        glm::quat m_currentRotation;
        glm::vec3 m_currentScale;

        // Transform interpolation.
        glm::vec3 m_previousPosition;
        glm::quat m_previousRotation;
        glm::vec3 m_previousScale;
    };
}
