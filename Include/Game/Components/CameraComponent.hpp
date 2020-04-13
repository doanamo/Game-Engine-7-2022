/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/Component.hpp"

/*
    Camera Component
*/

namespace Game
{
    class TransformComponent;

    class CameraComponent : public Component
    {
    public:
        struct ProjectionTypes
        {
            enum
            {
                Perspective,
                Orthogonal,
            };

            using Type = unsigned int;
        };

    public:
        CameraComponent() = default;
        ~CameraComponent() = default;

        CameraComponent(CameraComponent&& other);
        CameraComponent& operator=(CameraComponent&& other);

        void SetupOrthogonal(const glm::vec2& viewSize, float nearPlane, float farPlane);
        void SetupPerspective(float fov, float nearPlane, float farPlane);
        glm::mat4 CalculateTransform(const glm::ivec2& viewportSize);

        TransformComponent* GetTransformComponent();

    protected:
        bool OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf) override;

    private:
        TransformComponent* m_transform = nullptr;
        ProjectionTypes::Type m_projection = ProjectionTypes::Perspective;
        glm::vec2 m_viewSize = glm::vec2(2.0f, 2.0f);
        float m_nearPlane = 0.1f;
        float m_farPlane = 1000.0f;
        float m_fov = 90.0f;
    };
}
