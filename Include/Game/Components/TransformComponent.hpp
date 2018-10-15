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
    namespace Components
    {
        // Transform component.
        class Transform : public Component
        {
        public:
            Transform();
            ~Transform();

            Transform(Transform&& other);
            Transform& operator=(Transform&& other);

            // Calculates the transform matrix.
            glm::mat4 CalculateMatrix(const glm::mat4& input = glm::mat4(1.0f)) const;

            // Transform parameters.
            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 scale;
        };
    }
}
