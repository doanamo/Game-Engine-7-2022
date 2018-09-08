/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Precompiled.hpp"
#include "Game/Component.hpp"

/*
    Camera Component
*/

namespace Components
{
    // Forward declarations.
    class Transform;

    // Camera component class.
    class Camera : public Game::Component
    {
    public:
        Camera();
        ~Camera();

        // Setups orthogonal projection for the camera.
        void SetupOrthogonal(const glm::vec2& viewSize, float nearPlane, float farPlane);

        // Setups perspective projection for the camera.
        void SetupPerspective(float fov, float nearPlane, float farPlane);

        // Calculates the projection matrix.
        glm::mat4 CalculateTransform(const glm::ivec2& viewportSize);

        // Returns associated transform component.
        Transform* GetTransformComponent();

    protected:
        // Initializes the component.
        bool OnInitialize(Game::ComponentSystem* componentSystem,
            const Game::EntityHandle& entitySelf) override;

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
        Transform* m_transform;

        // Camera parameters.
        ProjectionTypes::Type m_projection;
        
        glm::vec2 m_viewSize;
        float m_nearPlane;
        float m_farPlane;
        float m_fov;
    };
}
