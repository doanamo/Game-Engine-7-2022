/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/SpriteList.hpp"
using namespace Graphics;

SpriteList::SpriteList()
{
}

SpriteList::~SpriteList()
{
}

void SpriteList::ReserveSprites(std::size_t count)
{
    m_spriteInfo.reserve(count);
    m_spriteData.reserve(count);
    m_spriteInstance.reserve(count);

    m_spriteSort.reserve(count);
}

void SpriteList::AddSprite(const Sprite& sprite)
{
    m_spriteInfo.push_back(sprite.info);
    m_spriteData.push_back(sprite.data);
    m_spriteInstance.push_back(sprite.instance);
}

void SpriteList::ClearSprites()
{
    m_spriteInfo.clear();
    m_spriteData.clear();
    m_spriteInstance.clear();

    m_spriteSort.clear();
}

void SpriteList::SortSprites()
{
    ASSERT(m_spriteInfo.size() == m_spriteData.size() == m_spriteInstance.size(),
        "Arrays of sprite info and data have different size!");

    // Define a sorting function.
    // Assumes typical orthogonal 2D projection.
    const std::vector<Sprite::Info>& spriteInfo = m_spriteInfo;
    const std::vector<Sprite::Data>& spriteData = m_spriteData;
    const std::vector<Sprite::Instance>& spriteInstance = m_spriteInstance;

    auto SpriteSort = [&spriteInfo, &spriteData, &spriteInstance]
        (const std::size_t& a, const std::size_t& b) -> bool
    {
        // Get sprite info and data.
        const auto& spriteInfoA = spriteInfo[a];
        const auto& spriteDataA = spriteData[a];
        const auto& spriteInstanceA = spriteInstance[a];

        const auto& spriteInfoB = spriteInfo[b];
        const auto& spriteDataB = spriteData[b];
        const auto& spriteInstanceB = spriteInstance[b];

        // Sort by transparency (opaque before transparent).
        if(spriteInfoA.transparent < spriteInfoB.transparent)
            return true;

        // Sort by texture (group same textures).
        if(spriteInfoA.texture < spriteInfoB.texture)
            return true;

        // Sort by filter (group same filters)
        if(spriteInfoA.filter < spriteInfoB.filter)
            return true;

        return false;
    };

    // Generate sprite indices.
    m_spriteSort.resize(m_spriteInfo.size());
    std::iota(m_spriteSort.begin(), m_spriteSort.end(), 0);

    // Create a sort permutation for the most efficient drawing.
    // We use stable sort to not introduce possible flickering in rendered images.
    std::stable_sort(m_spriteSort.begin(), m_spriteSort.end(), SpriteSort);

    // Sort sprite info and data arrays.
    Utility::ReorderWithIndices(m_spriteInfo, m_spriteSort);
    Utility::ReorderWithIndices(m_spriteData, m_spriteSort);
    Utility::ReorderWithIndices(m_spriteInstance, m_spriteSort);
}

std::size_t Graphics::SpriteList::GetSpriteCount() const
{
    ASSERT(m_spriteInfo.size() == m_spriteData.size() == m_spriteInstance.size(),
        "Arrays of sprite info and data have different size!");

    return m_spriteInfo.size();
}

const std::vector<Sprite::Info>& SpriteList::GetSpriteInfo() const
{
    return m_spriteInfo;
}

const std::vector<Sprite::Data>& SpriteList::GetSpriteData() const
{
    return m_spriteData;
}

const std::vector<Sprite::Instance>& SpriteList::GetSpriteInstance() const
{
    return m_spriteInstance;
}
