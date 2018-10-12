/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/Sprites/Sprite.hpp"

/*
    Sprite List

    Holds two vectors of sprite info and data which can be sorted.
*/

namespace Graphics
{
    // Sprite list class.
    class SpriteList
    {
    public:
        SpriteList();
        ~SpriteList();

        // Copy operations.
        // There should be no reason to copy a sprite list.
        SpriteList(const SpriteList& other) = delete;
        SpriteList& operator=(const SpriteList& other) = delete;

        // Move operations.
        // Sprite list is too short lived to require moving.
        SpriteList(SpriteList&& other) = delete;
        SpriteList& operator=(SpriteList&& other) = delete;

        // Reserves initial memory for a number of sprites.
        void ReserveSprites(std::size_t count);

        // Adds a sprite to the list.
        void AddSprite(const Sprite& sprite);

        // Sorts sprites for efficient rendering.
        void SortSprites();

        // Clears the sprite list.
        void ClearSprites();

        // Returns the number of held sprites.
        std::size_t GetSpriteCount() const;

        // Returns the sprite info vector.
        const std::vector<Sprite::Info>& GetSpriteInfo() const;

        // Returns the sprite data vector.
        const std::vector<Sprite::Data>& GetSpriteData() const;

    private:
        // Separate vectors for holding sprites.
        std::vector<Sprite::Info> m_spriteInfo;
        std::vector<Sprite::Data> m_spriteData;

        // List of sprite indices used for sorting.
        std::vector<std::size_t> m_spriteSort;
    };
}
