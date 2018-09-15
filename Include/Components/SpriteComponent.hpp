/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/Component.hpp"
#include "Graphics/Sprite.hpp"

/*
    Sprite Component
*/

namespace Components
{
    // Forward declarations.
    class Transform;

    // Sprite component.
    class Sprite : public Game::Component
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
        bool OnInitialize(Game::ComponentSystem* componentSystem, const Game::EntityHandle& entitySelf) override;

    private:
        // Transform component reference.
        Transform* m_transform;
    };
}
