/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Game/Component.hpp"
#include <Graphics/Sprite/Sprite.hpp>
#include <Graphics/TextureView.hpp>

/*
    Sprite Component

    Graphical component representing textures quad shape.
*/

namespace Game
{
    class TransformComponent;

    class SpriteComponent final : public Component
    {
    public:
        SpriteComponent();
        ~SpriteComponent();

        void SetTextureView(Graphics::TextureView texture);
        void SetRectangle(const glm::vec4& rectangle);
        void SetColor(const glm::vec4& color);
        void SetTransparent(bool toggle);
        void SetFiltered(bool toggle);

        TransformComponent* GetTransformComponent() const;
        const Graphics::TextureView& GetTextureView() const;
        glm::vec4 GetRectangle() const;
        glm::vec4 GetColor() const;
        bool IsTransparent() const;
        bool IsFiltered() const;

    private:
        bool OnInitialize(ComponentSystem* componentSystem,
            const EntityHandle& entitySelf) override;

        TransformComponent* m_transformComponent = nullptr;
        Graphics::TextureView m_textureView;
        glm::vec4 m_rectangle = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        glm::vec4 m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        bool m_transparent = false;
        bool m_filtered = true;
    };
}
