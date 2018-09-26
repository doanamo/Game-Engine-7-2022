/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/Component.hpp"
#include "Graphics/Sprite.hpp"

/*
    Sprite Component
*/

namespace Game
{
    namespace Components
    {
        // Forward declarations.
        class Transform;

        // Sprite component.
        class Sprite : public Component
        {
        public:
            Sprite();
            ~Sprite();

            // Return the transform component.
            Transform* GetTransform() const;

        public:
            // Sprite info and data.
            Graphics::Sprite::Info info;
            Graphics::Sprite::Data data;

        protected:
            // Called on component initialization.
            bool OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf) override;

        private:
            // Transform component reference.
            Transform* m_transform;
        };
    }
}
