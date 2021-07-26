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

        void SetTextureView(Graphics::TextureView texture)
        {
            m_textureView = texture;
        }

        void SetRectangle(const glm::vec4& rectangle)
        {
            m_rectangle = rectangle;
        }

        void SetColor(const glm::vec4& color)
        {
            m_color = color;
        }

        void SetTransparent(bool toggle)
        {
            m_transparent = toggle;
        }

        void SetFiltered(bool toggle)
        {
            m_filtered = toggle;
        }

        TransformComponent* GetTransformComponent() const
        {
            ASSERT(m_transformComponent);
            return m_transformComponent;
        }

        const Graphics::TextureView& GetTextureView() const
        {
            return m_textureView;
        }

        glm::vec4 GetRectangle() const
        {
            return m_rectangle;
        }

        glm::vec4 GetColor() const
        {
            return m_color;
        }

        bool IsTransparent() const
        {
            return m_transparent;
        }

        bool IsFiltered() const
        {
            return m_filtered;
        }

    private:
        bool OnInitialize(ComponentSystem* componentSystem,
            const EntityHandle& entitySelf) override;

    private:
        TransformComponent* m_transformComponent = nullptr;
        Graphics::TextureView m_textureView;
        glm::vec4 m_rectangle = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        glm::vec4 m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        bool m_transparent = false;
        bool m_filtered = true;
    };
}
