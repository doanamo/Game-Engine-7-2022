/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Game/Component.hpp"

/*
    Camera Component

    Projection transform component that represents camera view.
*/

namespace Game
{
    class TransformComponent;

    class CameraComponent final : public Component
    {
    public:
        struct ProjectionTypes
        {
            enum
            {
                Perspective,
                Orthogonal,
            };

            using Type = int8_t;
        };

    public:
        CameraComponent();
        ~CameraComponent();

        void SetupOrthogonal(const glm::vec2& viewSize, float nearPlane, float farPlane);
        void SetupPerspective(float fov, float nearPlane, float farPlane);
        glm::mat4 CalculateTransform(const glm::ivec2& viewportSize);

        TransformComponent* GetTransformComponent()
        {
            return m_transform;
        }

    private:
        bool OnInitialize(ComponentSystem* componentSystem,
            const EntityHandle& entitySelf) override;

    private:
        TransformComponent* m_transform = nullptr;
        ProjectionTypes::Type m_projection = ProjectionTypes::Perspective;
        glm::vec2 m_viewSize = glm::vec2(2.0f, 2.0f);
        float m_nearPlane = 0.1f;
        float m_farPlane = 1000.0f;
        float m_fov = 90.0f;
    };
}
