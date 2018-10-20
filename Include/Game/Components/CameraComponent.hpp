/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Precompiled.hpp"
#include "Game/Component.hpp"

/*
    Camera Component
*/

namespace Game
{
    // Forward declarations.
    class TransformComponent;

    // Camera component class.
    class CameraComponent : public Component
    {
    public:
        CameraComponent();
        ~CameraComponent();

        CameraComponent(CameraComponent&& other);
        CameraComponent& operator=(CameraComponent&& other);

        // Setups orthogonal projection for the camera.
        void SetupOrthogonal(const glm::vec2& viewSize, float nearPlane, float farPlane);

        // Setups perspective projection for the camera.
        void SetupPerspective(float fov, float nearPlane, float farPlane);

        // Calculates the projection matrix.
        glm::mat4 CalculateTransform(const glm::ivec2& viewportSize);

        // Returns associated transform component.
        TransformComponent* GetTransformComponent();

    protected:
        // Initializes the component.
        bool OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf) override;

    public:
        // Camera types.
        struct ProjectionTypes
        {
            enum
            {
                Perspective,
                Orthogonal,
            };

            using Type = unsigned int;
        };

    private:
        // Transform component.
        TransformComponent* m_transform;

        // Camera parameters.
        ProjectionTypes::Type m_projection;

        glm::vec2 m_viewSize;
        float m_nearPlane;
        float m_farPlane;
        float m_fov;
    };
}
