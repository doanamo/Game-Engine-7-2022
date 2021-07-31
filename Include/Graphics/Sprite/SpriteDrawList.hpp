/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Graphics/Sprite/Sprite.hpp"

/*
    Sprite Draw List

    Holds two vectors of sprite info and data
    which can be sorted and then sent to be draw.
*/

namespace Graphics
{
    class SpriteDrawList final : private Common::NonCopyable
    {
    public:
        SpriteDrawList();
        ~SpriteDrawList();

        SpriteDrawList(SpriteDrawList&& other) = delete;
        SpriteDrawList& operator=(SpriteDrawList&& other) = delete;

        void ReserveSprites(std::size_t count);
        void AddSprite(const Sprite& sprite);
        void SortSprites();
        void ClearSprites();

        std::size_t GetSpriteCount() const
        {
            ASSERT(m_spriteInfo.size() == m_spriteData.size());
            return m_spriteInfo.size();
        }

        const std::vector<Sprite::Info>& GetSpriteInfo() const
        {
            return m_spriteInfo;
        }

        const std::vector<Sprite::Data>& GetSpriteData() const
        {
            return m_spriteData;
        }

    private:
        std::vector<Sprite::Info> m_spriteInfo;
        std::vector<Sprite::Data> m_spriteData;
        std::vector<std::size_t> m_spriteSort;
    };
}
