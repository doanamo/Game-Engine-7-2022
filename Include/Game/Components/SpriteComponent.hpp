/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Graphics/Sprite/Sprite.hpp>
#include <Graphics/TextureView.hpp>
#include "Game/Component.hpp"

/*
    Sprite Component
*/

namespace Game
{
    // Forward declarations.
    class TransformComponent;

    // Sprite component.
    class SpriteComponent : public Component
    {
    public:
        SpriteComponent();
        ~SpriteComponent();

        SpriteComponent(SpriteComponent&& other);
        SpriteComponent& operator=(SpriteComponent&& other);

        // Sets sprite texture.
        void SetTextureView(Graphics::TextureView texture);

        // Sets sprite rectangle.
        void SetRectangle(const glm::vec4& rectangle);

        // Sets sprite color.
        void SetColor(const glm::vec4& color);

        // Sets sprite transparency.
        void SetTransparent(bool toggle);

        // Sets sprite filtering.
        void SetFiltered(bool toggle);

        // Gets sprite texture.
        const Graphics::TextureView& GetTextureView() const;

        // Gets sprite rectangle.
        glm::vec4 GetRectangle() const;

        // Gets sprite color.
        glm::vec4 GetColor() const;

        // Gets sprite transparency.
        bool IsTransparent() const;

        // Gets sprite filtering.
        bool IsFiltered() const;

        // Returns the transform component.
        TransformComponent* GetTransformComponent() const;

    protected:
        // Called on component initialization.
        bool OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf) override;

    private:
        // Transform component reference.
        TransformComponent* m_transformComponent;

        // Sprite texture.
        Graphics::TextureView m_textureView;

        // Sprite description.
        glm::vec4 m_rectangle;
        glm::vec4 m_color;
        bool m_transparent;
        bool m_filtered;
    };
}
